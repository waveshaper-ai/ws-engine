#include "WavReader.h"
#include "JsonParser.h"
#include "JsonWriter.h"
#include "ScriptException.h"
#include <climits>
#include <cstring>
#include <memory>

/* MAX_READ_BUF_LEN is a constant for how many bytes to read per sample.
 * Should be enough fo:
 * (bytes per sample) * (num channels)
 *
 * (3 bytes) * (2) = 6 bytes per read, as we are reading one sample at a time
 * 256 is overkill.. but there could be many channels
 */

#define MAX_READ_BUF_LEN 256

namespace
{
using WavReader = WS::WavReader;

float convert16BitsToFloat(unsigned char* b)
{
    /* Takes 2 bytes from the data stream and combines them to a signed int, and then casts to a float */
    /* Similar to the above function but slightly more compact syntax. */

    // short = (b[0] & 0x00ff) | ((unsigned char)b[1] << 8);

    int16_t int16 = b[1] << 8 | b[0];
    return static_cast<float>(int16) / static_cast<float>(32768.); /* MAX_SHORT - no need to account for extra 0 */
}

float convert24BitsToFloat(unsigned char* b)
{
    /* Takes 3 bytes from the data stream and reverses them to convert to signed int (4 bytes).
     *
     * Since we only used 3 bytes, the last byte of the int is 0, so to get a float value scaled to
     * [-1,1] we divide the int by MAX_INT-256 (signed MAX_INT), and that operation evaluates to a float,
     * which we return to the caller. */

    int int32 = b[2] << 24 | b[1] << 16 | b[0] << 8;
    return int32 / static_cast<float>(INT_MAX - 256); /* scale to [-1,1] */
}

void convertFloatTo24Bits(unsigned char* buf, float* f)
{
    int32_t reint = static_cast<int32_t>(*(f) * 8388608.F);

    buf[2] = static_cast<uint8_t>(reint >> 16) & 0xFF;
    buf[1] = static_cast<uint8_t>(reint >> 8) & 0xFF;
    buf[0] = static_cast<uint8_t>(reint & 0xFF);
}

float clamp(float value, float minValue, float maxValue)
{
    value = std::min(value, maxValue);
    value = std::max(value, minValue);
    return value;
}

// Force compilation of templated class for conformity.
// std::unique_ptr<AudioFile<float>> gAudioFile{};
} // namespace

WavReader::WavReader()
{
}

WavReader::WavReader(std::string const& filePath)
{
    load(filePath);
}

WavReader::WavReader(std::string const& filePath, std::string const& outputFile)
{
    load(filePath, outputFile);
}

WavReader::~WavReader()
{
    if(bufferToFillR != nullptr)
        delete[] bufferToFillR;

    if(outWavFile != nullptr)
    {
        fseek(outWavFile, dataHdrSizeOffset, 0);
        unsigned int new_data_size = writtenSamples * (bitsPerSample / 8) * numChannels;
        fwrite(&new_data_size, 4, 1, outWavFile);
        fseek(outWavFile, riffSizeOffset, 0);
        unsigned int new_riff_size = srcRiffSize - (srcDataSize - new_data_size);
        fwrite(&new_riff_size, 4, 1, outWavFile);
        fclose(outWavFile);
    }
}

bool WavReader::load(std::string const& filePath)
{
    wavFile.open(filePath, std::ios_base::binary);

    checkForRiffHeader();
    readToWaveChunkHeader();
    updateInfo();
    findDataPosition();

    return wavFile.is_open();
}

bool WavReader::load(std::string const& filePath, std::string const& outputFile)
{
    writeToOutput = true;

    wavFile.open(filePath, std::ios_base::binary);

    outWavFile = fopen(outputFile.c_str(), "wb");

    checkForRiffHeader();

    readToWaveChunkHeader();

    fwrite(&riff, 4, 1, outWavFile);
    srcRiffSize = riff.size;
    riffSizeOffset = ftell(outWavFile);
    srcDataSize = 0;
    fwrite(&(riff.size), 4, 1, outWavFile);

    /* Check to make sure the chunk ID is 'W' 'A' 'V' 'E' */
    if(!isChunkID(wave, 'W', 'A', 'V', 'E'))
        throw std::runtime_error("invalid format, not a WAV file");
    fwrite("WAVE", 4, 1, outWavFile);

    updateInfo();
    findDataPosition();

    bool outputFileOpened = false;
    if(outWavFile != nullptr)
        outputFileOpened = true;

    return (wavFile.is_open() && outputFileOpened);
}

bool WavReader::loadOut(std::string const& outputFile)
{
    writeToOutput = true;

    outWavFile = fopen(outputFile.c_str(), "wb");

    fwrite(&riff, 4, 1, outWavFile);
    srcRiffSize = riff.size;
    riffSizeOffset = ftell(outWavFile);
    srcDataSize = 0;
    fwrite(&(riff.size), 4, 1, outWavFile);

    fwrite("WAVE", 4, 1, outWavFile);

    if(writeToOutput)
    {
        // the blank character after "fmt" MUST be placed (do not remove space)
        fwrite("fmt ", 4, 1, outWavFile);
        fwrite(&(fmtPosition), 4, 1, outWavFile);
    }

    /* 2 byte format code */
    if(writeToOutput)
        fwrite(&audioFormat, 2, 1, outWavFile);

    /* 2 byte short storing the number of interleaved channels */
    if(writeToOutput)
        fwrite(&numChannels, 2, 1, outWavFile);

    /* 4 byte integer encoded in little-endian byte order storing the sample rate */
    if(writeToOutput)
        fwrite(&sampleRate, 4, 1, outWavFile);
    if(writeToOutput)
        fwrite(&byteRate, 4, 1, outWavFile);
    if(writeToOutput)
        fwrite(&blockAlign, 2, 1, outWavFile);

    if(fmtPosition >= 16)
    {
        if(writeToOutput)
            fwrite(&bitsPerSample, 2, 1, outWavFile);
    }

    /* If the fmt header size is >= 18, then read some extra bytes
     * and do nothing with them
     */

    if(fmtPosition >= 18)
    {
        if(writeToOutput)
            fwrite(&extraSize, 2, 1, outWavFile);

        if(extraSize > 0)
        {
            // read and process ExtraSize number of bytes as needed...
            void* xbuf = malloc(extraSize);
            read(wavFile, xbuf, extraSize, &chk);
            if(writeToOutput)
                fwrite(&xbuf, extraSize, 1, outWavFile);
            free(xbuf);
        }
    }

    if(writeToOutput)
    {
        fwrite("data", 4, 1, outWavFile);
        dataHdrSizeOffset = ftell(outWavFile);
        srcDataSize = chk.size;
        fwrite(&(chk.size), 4, 1, outWavFile);
    }

    bool outputFileOpened = false;
    if(outWavFile != nullptr)
        outputFileOpened = true;

    return outputFileOpened;
}

bool WavReader::createFileFromData(std::string const& filePath, struct WavReader::chunkHdr& priff, struct WavReader::chunkHdr& pwave, struct WavReader::chunkHdr& pchk,
    int pfmtPosition, int pdataPosition, int pextraSize, unsigned short paudioFormat, unsigned short pnumChannels, unsigned short psampleRate, unsigned int pbyteRate,
    unsigned short pbitsPerSample, unsigned short pblockAlign)
{
    writeToOutput = true;

    this->audioFormat = paudioFormat;
    this->numChannels = pnumChannels;
    this->sampleRate = psampleRate;
    this->byteRate = pbyteRate;
    this->bitsPerSample = pbitsPerSample;
    this->blockAlign = pblockAlign;
    this->chk = pchk;
    this->wave = pwave;
    this->riff = priff;
    this->dataPosition = pdataPosition;
    this->fmtPosition = pfmtPosition;
    this->extraSize = pextraSize;

    loadOut(filePath);

    return true;
}

WavReader::chunkHdr WavReader::getRiff() const
{
    return riff;
}

WavReader::chunkHdr WavReader::getWave() const
{
    return wave;
}

WavReader::chunkHdr WavReader::getChk() const
{
    return chk;
}

unsigned int WavReader::getFmtPosition() const
{
    return fmtPosition;
}

unsigned int WavReader::getDataPosition() const
{
    return dataPosition;
}

void WavReader::setFmtPosition(unsigned int position)
{
    fmtPosition = position;
}

void WavReader::setDataPosition(unsigned int position)
{
    dataPosition = position;
}

unsigned short WavReader::getAudioFormat()
{
    return audioFormat;
}

unsigned short WavReader::getNumChannels()
{
    return numChannels;
}

unsigned short WavReader::getBlockAlign()
{
    return blockAlign;
}

unsigned int WavReader::getByteRate()
{
    return byteRate;
}

unsigned short WavReader::getBitsPerSample()
{
    return bitsPerSample;
}

unsigned short WavReader::getExtraSize()
{
    return extraSize;
}

bool WavReader::writeSpecToJSON(std::string const& jsonFileName)
{
    if(!wavFile.is_open())
        return false;
/*
{
    "header_data": [
        {
            "id": "RIFF",
            "size": 53365588,
            "pos": 4
        },
        {
            "id": "WAVE",
            "size": 53365584,
            "pos": 16
        },
        {
            "id": "data",
            "size": 53157888,
            "pos": 0
        }
    ],
    "fmt_position": 16,
    "data_position": 53157888,
    "audio_format": 1,
    "num_channels": 1,
    "sample_rate": 44100,
    "byte_rate": 88200,
    "extra_size": 42712,
    "block_align": 2,
    "bits_per_sample": 16,
    "number_of_samples": 26578944
}
*/
    // Now write the stuff.
    TL::LibScript::RapidWrapper rwrap;
    TL::LibScript::JsonWriter writer{&rwrap};

    writer.writeArrayBegin("header_data");

    writer.writeObjectBegin("");
    writer.writeValuePair("id", std::string{"RIFF"});
    writer.writeValuePair("size", riff.size);
    writer.writeValuePair("pos", riff.pos);
    writer.writeObjectEnd();

    writer.writeObjectBegin("");
    writer.writeValuePair("id", std::string{"WAVE"});
    writer.writeValuePair("size", wave.size);
    writer.writeValuePair("pos", wave.pos);
    writer.writeObjectEnd();

    writer.writeObjectBegin("");
    writer.writeValuePair("id", std::string{"data"});
    writer.writeValuePair("size", chk.size);
    writer.writeValuePair("pos", chk.pos);
    writer.writeObjectEnd();

    writer.writeArrayEnd();

    writer.writeValuePair("fmt_position", fmtPosition);
    writer.writeValuePair("data_position", dataPosition);
    writer.writeValuePair("audio_format", u32{audioFormat});
    writer.writeValuePair("num_channels", u32{numChannels});
    writer.writeValuePair("sample_rate", sampleRate);
    writer.writeValuePair("byte_rate", byteRate);
    writer.writeValuePair("extra_size", u32{extraSize});
    writer.writeValuePair("block_align", u32{blockAlign});
    writer.writeValuePair("bits_per_sample", u32{bitsPerSample});
    writer.writeValuePair("data_header_size_offset", dataHdrSizeOffset);
    writer.writeValuePair("number_of_samples", getNumSamplesPerChannel());

    writer.writeToFile(jsonFileName);
    return true;
}

// bool WavReader::createFileFromJson(std::string const &inJsonConfigPathName, std::string outputFileName)
// {
//   JsonConfig config;
//   try
//   {
//     TL::LibScript::JsonParser::parseScript(inJsonConfigPathName, "", config);
//   }
//   catch (TL::LibScript::ScriptException &e)
//   {
//     std::cout << "Could not find the file: " << e.what() << std::endl;
//     return 1;
//   }
//
//   writeToOutput = true;
//
//   chunkHdr tRiff;
//   tRiff.id[0] = 'R';
//   tRiff.id[1] = 'I';
//   tRiff.id[2] = 'F';
//   tRiff.id[3] = 'F';
//   tRiff.size = config.mHeaderData[0].mSize;
//   tRiff.pos = config.mHeaderData[0].mPos;
//
//   chunkHdr tWave;
//   tWave.id[0] = 'W';
//   tWave.id[1] = 'A';
//   tWave.id[2] = 'V';
//   tWave.id[3] = 'E';
//   tWave.size = config.mHeaderData[1].mSize;
//   tWave.pos = config.mHeaderData[1].mPos;
//
//   chunkHdr tChk;
//   tChk.id[0] = 'd';
//   tChk.id[1] = 'a';
//   tChk.id[2] = 't';
//   tChk.id[3] = 'a';
//   tChk.size = config.mHeaderData[2].mSize;
//   tChk.pos = config.mHeaderData[2].mPos;
//
//   createFileFromData(outputFileName, tRiff, tWave, tChk, config.mAudioFormat, config.mNumChannels,
//                      config.mFmtPosition, config.mDataPosition, config.mExtraSize, config.mSampleRate, config.mByteRate,
//                      config.mBitsPerSample, config.mBlockAlign);
//
//   return true;
// }

void WavReader::checkForRiffHeader()
{
    read(wavFile, riff, NULL);

    /* Make sure it is a RIFF header */
    if(!isChunkID(riff, 'R', 'I', 'F', 'F'))
        throw std::runtime_error("invalid format, not a RIFF file");
}

void WavReader::readToWaveChunkHeader()
{
    /* Read 4 bytes to the ID field of the WAVE chunk header */
    read(wavFile, wave.id, 4, &riff);
    wave.size = riff.size - 4;
    wave.pos = 0;
}

void WavReader::findDataPosition()
{
    while(!isChunkID(chk, 'd', 'a', 't', 'a'))
    {
        read(wavFile, chk, &wave);

        if(isChunkID(chk, 'd', 'a', 't', 'a'))
        {
            dataPosition = chk.size;
            if(writeToOutput)
            {
                fwrite("data", 4, 1, outWavFile);
                dataHdrSizeOffset = ftell(outWavFile); // 40
                srcDataSize = chk.size; // 119948544
                fwrite(&(chk.size), 4, 1, outWavFile);
            }
            break;
        }

        /* Nope .. don't need it.. not a fmt or data */
        skip(wavFile, chk.size, &chk);

        /* Done with chunk */
        wave.pos += chk.pos;

        /* Check for padding.. need to investigate this more */
        if(chk.size % 2)
            skip(wavFile, 1, &wave);
    }
}

bool WavReader::getNextAudioBlock(float* bufferToFill, int channel, size_t bufferSize)
{
    if(bufferToFill == nullptr)
        return false;

    if(numChannels > 1 && bufferToFillR == nullptr)
        bufferToFillR = new float[bufferSize];

    checkForExceptions(channel, bufferSize);

    mBufferSize = bufferSize;

    /*TODO : user needs to call first channel 0 if there is more than
     one channel.
     If user tries to call channel 1 before channel 0, output will be
     zero. We need to solve this*/

    // Check if buffer has memory

    // std::cout << " samples written : " << writtenSamples << std::endl;

    if(channel == 0)
    {
        if(enoughSamplesLeft())
            fillDataBuffer(bufferToFill);
        else
        {
            fillDataBuffer(bufferToFill, false);
            isEofLReached = true;
        }
    }

    if(channel == maxNumOfChannels - 1)
    {
        memcpy(bufferToFill, bufferToFillR, mBufferSize * sizeof(float));
        if(isEofLReached)
            isEofReached = true;
    }

    return true;
}

bool WavReader::writeToFile(float* bufferToFileL, float* bufferToFileR, size_t bufferSize)
{
    if(bufferToFileL == nullptr)
    {
        return false;
    }

    auto buffSize = 0;
    auto remainingSamples = getNumSamplesPerChannel() - writtenSamples;

    mBufferSize = bufferSize;

    if(remainingSamples < mBufferSize)
        buffSize = remainingSamples;
    else
        buffSize = mBufferSize;

    if(bitsPerSample == 16)
    {
        for(int writeSampleCtr = 0; writeSampleCtr < buffSize; writeSampleCtr++)
        {
            float chanTmp = clamp(bufferToFileL[writeSampleCtr], -1., 1.);
            sampleToFile(chanTmp);

            if(bufferToFillR != nullptr)
            {
                chanTmp = clamp(bufferToFileR[writeSampleCtr], -1., 1.);
                sampleToFile(chanTmp);
            }
        }
    }
    else if(bitsPerSample == 24)
    {
        for(int writeSampleCtr = 0; writeSampleCtr < buffSize; writeSampleCtr++)
        {
            /* Sample for chan 0 */
            float chanTmp = clamp(bufferToFileL[writeSampleCtr], -1., 1.);
            uint8_t chanBytes[3];
            convertFloatTo24Bits(chanBytes, &chanTmp);

            fwrite(&chanBytes[0], 1, 1, outWavFile);
            fwrite(&chanBytes[1], 1, 1, outWavFile);
            fwrite(&chanBytes[2], 1, 1, outWavFile);

            /* Sample for chan 1 */
            if(bufferToFileR != nullptr)
            {
                chanTmp = clamp(bufferToFileR[writeSampleCtr], -1.0f, 1.0f);
                convertFloatTo24Bits(chanBytes, &chanTmp);

                fwrite(&chanBytes[0], 1, 1, outWavFile);
                fwrite(&chanBytes[1], 1, 1, outWavFile);
                fwrite(&chanBytes[2], 1, 1, outWavFile);
            }
        }
    }

    writtenSamples += buffSize;

    return true;
}

void WavReader::sampleToFile(float sample)
{
    int16_t chanIntSample = static_cast<int16_t>(sample * static_cast<float>(32767.));

    uint8_t chanBytes[3];
    uint8_t elementSize{2};
    if(bitsPerSample == 16)
    {
        chanBytes[1] = (chanIntSample >> 8) & 0xff;
        chanBytes[0] = chanIntSample & 0xff;
    }
    else
    {
        elementSize = 3;
        convertFloatTo24Bits(chanBytes, &sample);
    }

    fwrite(&chanBytes[0], elementSize, 1, outWavFile);
}

bool WavReader::enoughSamplesLeft()
{
    size_t numBytes = (bitsPerSample / 8);
    if(chk.pos + mBufferSize * numBytes * numChannels > chk.size)
        return false;
    return true;
}

size_t WavReader::getWrittenSamples()
{
    return writtenSamples;
}

void WavReader::updateInfo()
{
    /* We read chunk into the current sub chunk header, and update the wave pos */

    while(wave.pos < wave.size)
    {
        read(wavFile, chk, &wave);

        /* Check to see if this is a fmt sub chunk */

        if(isChunkID(chk, 'f', 'm', 't', ' '))
        {
            fmtPosition = chk.size;
            if(writeToOutput)
            {
                // the blank character after "fmt" MUST be placed (do not remove space)
                fwrite("fmt ", 4, 1, outWavFile);
                fwrite(&(chk.size), 4, 1, outWavFile);
            }

            /* A file should only have one fmt chunk */
            if(fmtFound)
                throw std::runtime_error("invalid format, extra fmt chunk");

            /* Set flag to true */
            fmtFound = true;

            getInfo();

            /* These must be extensions. If the size of the fmt
             * header is >= 16, there must be a bit depth field.
             * Maybe in some older formats, this field is not always there,
             * or is optional.
             */

            if(chk.size >= 16)
            {
                read(wavFile, &bitsPerSample, 2, &chk);
                if(writeToOutput)
                    fwrite(&bitsPerSample, 2, 1, outWavFile);
            }

            /* If the fmt header size is >= 18, then read some extra bytes
             * and do nothing with them
             */

            if(chk.size >= 18)
            {
                read(wavFile, &extraSize, 2, &chk);
                if(writeToOutput)
                    fwrite(&extraSize, 2, 1, outWavFile);

                if(extraSize > 0)
                {
                    // read and process ExtraSize number of bytes as needed...
                    void* xbuf = malloc(extraSize);
                    read(wavFile, xbuf, extraSize, &chk);
                    if(writeToOutput)
                        fwrite(&xbuf, extraSize, 1, outWavFile);
                    free(xbuf);
                }
            }

            /* If there are any further extra bytes in the fmt header chunk, skip */
            if(chk.pos < chk.size)
                skip(wavFile, chk.size - chk.pos, &chk);

            /* Ftm found no need to look further */
            break;
        }

        else
        {
            /* Nope .. don't need it.. not a fmt or data */
            skip(wavFile, chk.size, &chk);
        }

        /* Done with chunk */
        wave.pos += chk.pos;

        /* Check for padding.. need to investigate this more */
        if(chk.size % 2)
            skip(wavFile, 1, &wave);
    }
}

void WavReader::getInfo()
{
    /* 2 byte format code */
    read(wavFile, &audioFormat, 2, &chk);
    if(writeToOutput)
        fwrite(&audioFormat, 2, 1, outWavFile);

    /* 2 byte short storing the number of interleaved channels */
    read(wavFile, &numChannels, 2, &chk);
    if(writeToOutput)
        fwrite(&numChannels, 2, 1, outWavFile);

    /* 4 byte integer encoded in little-endian byte order storing the sample rate */
    read(wavFile, &sampleRate, 4, &chk);
    if(writeToOutput)
        fwrite(&sampleRate, 4, 1, outWavFile);
    read(wavFile, &byteRate, 4, &chk);
    if(writeToOutput)
        fwrite(&byteRate, 4, 1, outWavFile);
    read(wavFile, &blockAlign, 2, &chk);
    if(writeToOutput)
        fwrite(&blockAlign, 2, 1, outWavFile);
}

void WavReader::skip(std::ifstream& f, std::streamsize size, chunkHdr* parent)
{
    if(!f.seekg(size, std::ios_base::cur))
        throw std::runtime_error("unable to read from file");
    if(parent)
        parent->pos += size;
}

bool WavReader::isChunkID(const chunkHdr& c, char id1, char id2, char id3, char id4)
{
    return ((c.id[0] == id1) &&
        (c.id[1] == id2) &&
        (c.id[2] == id3) &&
        (c.id[3] == id4));
}

void WavReader::read(std::ifstream& f, chunkHdr& c, chunkHdr* parent)
{
    /* Read function that reads a chunk header */
    /* Note: This read function is called if the callers use the signature where the second argument is a chunk header */
    read(f, c.id, 4, parent);
    read(f, &(c.size), 4, parent);
    c.pos = 0;
    /* Note this is just for reading a chunk header: ID and Size. Position is initialized to 0 for the new chunk */
}

void WavReader::read(std::ifstream& f, void* buffer, std::streamsize size, chunkHdr* parent)
{
    if(!f.read(static_cast<char*>(buffer), size))
    {
        if(f.eof())
            throw std::runtime_error("unexpected eof reading from file");
        throw std::runtime_error("unable to read from file");
    }
    if(parent)
        parent->pos += size; /* Keeps track of offset in the parent chunk, increases it with every read */
}

size_t WavReader::getNumSamplesPerChannel()
{
    auto samples = static_cast<size_t>(8 * chk.size / bitsPerSample);
    return samples / numChannels;
}

int WavReader::getNumberOfChannels()
{
    return numChannels;
}

int WavReader::getSampleRate()
{
    return sampleRate;
}

int WavReader::getBitDepth()
{
    return bitsPerSample;
}

void WavReader::fillDataBuffer(float* bufferToFill, bool enoughSamplesToFillBuffer)
{
    checkForExceptions(0, mBufferSize);

    /*
     * Reads one sample at a time, which will be more than one
     * byte at a time.
     *
     * How many bytes to read for sample:
     * Number of bytes per sample * number of channels
     *
     * Number of bytes per sample = bits per sample / 8
     */

    size_t numBytes = (bitsPerSample / 8);
    size_t bytesToRead = numBytes * numChannels;

    if(enoughSamplesToFillBuffer)
        copyToBuffer(bytesToRead, bufferToFill, mBufferSize);
    else
    {
        auto samplesLeft = (chk.size - chk.pos) / bytesToRead;
        copyToBuffer(bytesToRead, bufferToFill, samplesLeft);
    }
}

void WavReader::checkForExceptions(int channel, size_t bufferSize)
{
    if(channel > numChannels - 1)
        throw std::runtime_error("max number of channel are : " + std::to_string(numChannels));

    if(bufferSize == 0)
        throw std::runtime_error("buffer size must be different than zero");

    /* Now we are where we want to be: the data sub-chunk
     * Ensure the fmt chunk was processed so we know how to interpret
     * the data */

    if(!fmtFound)
        throw std::runtime_error("no fmt chunk before data chunk");

    /* Let's kill the program if we get a 24-bit audio file or one
     * with > 2 channels */

    if(bitsPerSample > 24)
        throw std::runtime_error("currently 16-bit and 24-bit supported");

    if(numChannels > 2)
        throw std::runtime_error("currently mono or stereo only");

    if(isEofReached)
        throw std::runtime_error("unexpected eof reading from file");
}

void WavReader::copyToBuffer(size_t bytesToRead, float* bufferToFill, size_t numSamplesToCopy)
{
    size_t dataBytesRead = 0;

    /* Declare a buffer that is reasonably sized */
    uint8_t buffer[MAX_READ_BUF_LEN];

    /* Shouldnt' be more than 3 bytes in the case of a 24-bit depth sample */
    if(bytesToRead > sizeof(buffer))
        throw std::runtime_error("inadequate read buffer size");

    /* Flag to reduce the number of continous clipping reports..*/
    dataBytesRead = 0;

    float sampleCh0 = 0.0;
    float sampleCh1 = 0.0;

    // clear bufferToFill before use it
    memset(bufferToFill, 0, mBufferSize * sizeof(float));

    if(bufferToFillR != nullptr)
        memset(bufferToFill, 0, mBufferSize * sizeof(float));

    int index = 0;
    int numBytes = bitsPerSample / 8;

    // this is the whole file for JUST data
    while(dataBytesRead + bytesToRead < numSamplesToCopy * numBytes * numChannels + 1)
    {
        /* Read sample bytes to the buffer */
        read(wavFile, buffer, bytesToRead, &chk);

        /* Increment the dataBytesRead counter */
        dataBytesRead += bytesToRead;

        /* Need to convert the bytes to float in 2 cases.. 16 bit and 24 bit.. */
        switch(bitsPerSample)
        {
        case 16:
            sampleCh0 = convert16BitsToFloat(buffer);
            if(numChannels == maxNumOfChannels)
                sampleCh1 = convert16BitsToFloat(&buffer[numBytes]);
            break;
        case 24:
            sampleCh0 = convert24BitsToFloat(buffer);
            if(numChannels == maxNumOfChannels)
                sampleCh1 = convert24BitsToFloat(&buffer[numBytes]);
            break;
        default:
            std::cout << "ERROR! Unsupported bit depth of: " << bitsPerSample << std::endl;
            exit(0);
            break;
        }

        bufferToFill[index] = sampleCh0;
        if(numChannels == maxNumOfChannels)
            bufferToFillR[index] = sampleCh1;

        numSamplesRead++;
        index++;
    }
}
