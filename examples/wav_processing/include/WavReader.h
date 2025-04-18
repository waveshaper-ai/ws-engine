#pragma once
#include <fstream>
#include <iostream>

/* MAX_READ_BUF_LEN is a constant for how many bytes to read per sample.
 * Should be enough fo:
 * (bytes per sample) * (num channels)
 *
 * (3 bytes) * (2) = 6 bytes per read, as we are reading one sample at a time
 * 256 is overkill.. but there could be many channels
 */
#define MAX_READ_BUF_LEN 256

namespace WS
{
class WavReader
{
public:
    struct chunkHdr
    {
        char id[4]; /* Label of the chunk, e.g. d a t a */
        unsigned int size; /* Size of the chunk */
        unsigned int pos; /* Current position used for tracking */
    };

    WavReader();
    WavReader(std::string const& filePath);
    WavReader(std::string const& filePath, std::string const& outputFile);
    ~WavReader();

    bool load(std::string const& filePath);
    bool load(std::string const& filePath, std::string const& outputFile);
    size_t getNumSamplesPerChannel();
    int getNumberOfChannels();
    int getSampleRate();
    int getBitDepth();
    bool getNextAudioBlock(float* bufferToFill, int channel = 0, size_t bufferSize = 0);
    bool writeToFile(float* bufferToFileL, float* bufferToFileR = nullptr, size_t bufferSize = 0);
    size_t getWrittenSamples();
    size_t getBufferSize();

private:
    void checkForRiffHeader();
    void readToWaveChunkHeader();
    void read(std::ifstream& f, chunkHdr& c, chunkHdr* parent);
    void read(std::ifstream& f, void* buffer, std::streamsize size, chunkHdr* parent);
    bool isChunkID(const chunkHdr& c, char id1, char id2, char id3, char id4);
    void writeToHeader();
    void updateInfo();
    void getInfo();
    void findDataPosition();
    void skip(std::ifstream& f, std::streamsize size, chunkHdr* parent);
    bool enoughSamplesLeft();
    void fillDataBuffer(float* bufferToFill, bool enoughSamplesToFillBuffer = true);
    void copyToBuffer(size_t bytesToRead, float* bufferToFill, size_t numSamplesToCopy);
    void checkForExceptions(int channel = 0, size_t bufferSize = 0);
    void sampleToFile(float sample);

    chunkHdr riff;
    chunkHdr wave;
    chunkHdr chk;
    bool fmtFound = false;
    std::ifstream wavFile;
    unsigned int sampleRate, byteRate;
    unsigned short audioFormat, numChannels, blockAlign, bitsPerSample, extraSize;
    FILE* outWavFile = nullptr;
    size_t srcRiffSize = 0;
    unsigned int srcDataSize = 0;
    size_t dataHdrSizeOffset = 0;
    size_t riffSizeOffset = 0;
    size_t writtenSamples = 0;
    size_t mBufferSize = 0;
    size_t numSamplesRead = 0;
    bool isEofLReached = false;
    bool isEofReached = false;
    int maxNumOfChannels = 2;
    float* bufferToFillR = nullptr;
    bool writeToOutput = false;
    size_t headerDataSizeOffset = 0;
    unsigned int fmtPosition = 0;
    unsigned int dataPosition = 0;

public:
    bool createFileFromData(std::string const& filePath, struct WavReader::chunkHdr& riff, struct WavReader::chunkHdr& wave, struct WavReader::chunkHdr& chk,
        int fmtPosition, int dataPosition, int extraSize, unsigned short audioFormat, unsigned short numChannels, unsigned short sampleRate, unsigned int byteRate, unsigned short bitsPerSample, unsigned short blockAlign);
    bool loadOut(std::string const& outputFile);
    chunkHdr getRiff() const;
    chunkHdr getWave() const;
    chunkHdr getChk() const;
    unsigned int getFmtPosition() const;
    unsigned int getDataPosition() const;
    void setFmtPosition(unsigned int position);
    void setDataPosition(unsigned int position);
    unsigned short getAudioFormat();
    unsigned short getNumChannels();
    unsigned short getBlockAlign();
    unsigned int getByteRate();
    unsigned short getBitsPerSample();
    unsigned short getExtraSize();
    bool writeSpecToJSON(std::string const& jsonFileName);
    // bool createFileFromJson(std::string const &inJsonConfigPathName, std::string outputFileName);
};
} // namespace WS