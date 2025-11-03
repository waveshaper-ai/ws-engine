#include "StreamManager.h"
#include "AudioModel.h"
#include "Average.h"
#include "BasicTypes.h"
#include "HannFilter.h"
#include "WavReader.h"
#include "util.h"
#include <chrono>
#include <cmath>
#include <cstring>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <math.h>
#include <stdexcept>
#include <string>

namespace
{
using StreamManager = WS::StreamManager;
} // namespace

std::string StreamManager::getVersion()
{
    std::string version{"0.0"};
    std::ifstream ifs{"version.txt", std::ios_base::in};
    if(ifs.is_open())
    {
        char line[256];
        ifs.read(line, 256);
        ifs.close();
        version = line;
    }
    return version;
}

int StreamManager::processFile(WS::Util::CmdLineParser& parser)
{
    static const std::string ACTIVATION{"tanh"};
    std::unique_ptr<AudioModel> audioModel{new AudioModel(ACTIVATION)};
    if(!audioModel)
    {
        std::string error{"Could not allocate properly the AudioModel class."};
        std::cout << "ERROR: " << error << std::endl;
        return 1;
    }

    audioModel->setLicense("Demo Waveshaper");

    std::string modelName;
    parser.getValue("-m", modelName);
#ifdef OS_WINDOWS
    modelName += "\\";
#else
    modelName += "/";
#endif
    if(!audioModel->prepare(modelName))
    {
        std::string error{"Could not prepare the model properly. Check model file name as -m option."};
        std::cout << "ERROR: " << error << std::endl;
        return 1;
    }

    size_t numOfParams{audioModel->getNumberOfParams()};
    if(numOfParams > 0)
    {
        std::string paramValueStr;
        parser.getValue("-pf", paramValueStr);
        float pVal{std::stof(paramValueStr)};
        if(pVal > 1.0F)
            pVal = 1.0F;
        else if(pVal < 0.0F)
            pVal = 0.0F;

        audioModel->setParamValueAt(0, pVal);
    }

    WS::WavReader streamer;
    std::string inWavPathName, outWavPathName;
    std::string modelType = audioModel->getModelType();

    parser.getValue("inputFileWAV", inWavPathName);
    parser.getValue("outputFileWAV", outWavPathName);
    if(!streamer.load(inWavPathName, outWavPathName))
    {
        std::string error{"Could not load the given file for input: "};
        error += inWavPathName;
        error += " or create new file: ";
        error += outWavPathName;
        error += "\nCheck model file name as second argument.";
        std::cout << "ERROR: " << error << std::endl;
        return 1;
    }

    // Output the analyzer build
    std::cout << "Start processing file: " << inWavPathName << std::endl;

    unsigned int sampleRate, byteRate = 0U;
    unsigned short audioFormat, numChannels, blockAlign, bitsPerSample;

    audioFormat = 1;
    blockAlign = 2;
    sampleRate = streamer.getSampleRate();
    numChannels = streamer.getNumberOfChannels();
    bitsPerSample = streamer.getBitDepth();

    u32 samplesBufferSize{static_cast<u32>(audioModel->getFrameLength())};

    // Output a JSON object with those fields
    std::cout << "Format: " << audioFormat
              << "\nChannels: " << numChannels
              << "\nSample Rate: " << sampleRate
              << "\nByte Rate: " << byteRate
              << "\nBlock Align: " << blockAlign
              << "\nBits Per Sample: " << bitsPerSample
              << "\nSample Buffer Size: " << samplesBufferSize

              << std::endl;

    std::unique_ptr<float> bufferL{new float[samplesBufferSize]{0.0F}};
    std::unique_ptr<float> bufferR{new float[samplesBufferSize]{0.0F}};

    WS::HannFilter hannL{samplesBufferSize}, hannR{samplesBufferSize};
    std::unique_ptr<float> chan0Output{new float[samplesBufferSize]{0.0F}};
    std::unique_ptr<float> chan1Output{new float[samplesBufferSize]{0.0F}};
    u64 outputSamples{0U};
    u64 totalSamples{streamer.getNumSamplesPerChannel()};
    std::cout << "Total samples: " << totalSamples << std::endl;

    WS::Util::Average<float> averager;
    float mean;

    averager.init(totalSamples / 1000);
    bool firstProcess = true;
    bool fileCreated{true};
    while(outputSamples < totalSamples)
    {

        if(!streamer.getNextAudioBlock(bufferL.get(), 0, samplesBufferSize)) // on last call, reminder unused samples are set to 0
        {
            // Abort reading file.
            break;
        }

        if(numChannels > 1)
        {
            if(!streamer.getNextAudioBlock(bufferR.get(), 1, samplesBufferSize))
            {
                // Abort reading file.
                break;
            }
        }

        // --------- Apply Hann Windowing and process using the AudioModel -------------- //

        auto start = std::chrono::high_resolution_clock::now();

        auto remainingSamples = totalSamples - outputSamples;
        if((remainingSamples < samplesBufferSize) && modelType == "CRAFx")
        {
            hannL.applyLastFilter(chan0Output.get());
            if(numChannels > 1)
            {
                hannR.applyLastFilter(chan1Output.get());
            }
        }
        else
        {
            hannL.applyFilter(bufferL.get(), samplesBufferSize, *audioModel, chan0Output.get());
            if(numChannels > 1)
            {
                hannR.applyFilter(bufferR.get(), samplesBufferSize, *audioModel, chan1Output.get());
            }
        }
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
        averager.add(duration);

        // // due use of overlap-add, the first audio block will have a delay half samplesBufferSize
        // // then, we remove that delay by just writting the second half of the processed block
        if(firstProcess)
        {
            firstProcess = false;
            if((modelType == "CRAFx") || (modelType == "uni_CRAFx"))
            {
                continue;
            }
            else
            {
                fileCreated &= streamer.writeToFile(chan0Output.get() + samplesBufferSize / 2,
                    (numChannels > 1) ? chan1Output.get() + samplesBufferSize / 2 : nullptr,
                    samplesBufferSize / 2);
            }
        }
        else
        {
            fileCreated &= streamer.writeToFile(chan0Output.get(), (numChannels > 1) ? chan1Output.get() : nullptr, samplesBufferSize);
        }
        // Show completion
        outputSamples = streamer.getWrittenSamples() + samplesBufferSize / 2;
    }

    mean = averager.computeMean();
    std::cout << "Completion: " << std::fixed << std::setprecision(2) << std::setfill('0') << "100 % / " << "Average chunk process time: " << mean << " ms" << std::endl;
    std::cout.flush();

    return !fileCreated;
}
