#include "CmdLineParser.h"
#include "StreamManager.h"
#include <iostream>
#include <string>

int main(int argc, char* argv[])
{
    int retValue{0};

    std::string version{WS::StreamManager::getVersion()};
    std::cout << "WaveShaper AI Audio Processing Command-line Utility version " << version << "\n\n";

    WS::Util::CmdLineParser parser;
    parser.addArgument("inputFileWAV", "is the full path and name of the file to process. It is a .wav file.");
    parser.addArgument("outputFileWAV", "is the full path and name of the processed file name to output. It is a .wav file.");
    parser.addOption("-m", "data/PodcastFix_V1", "is the name of the model folder, as found in the output/data folder.");
    parser.addOption("-pf", "0.0", "is the value of the parameter of the model.");
    if(parser.validateCmdLine(argc, (char const**)argv))
    {
        parser.showParameterValues("All given values at cmd line:");
        retValue = WS::StreamManager::processFile(parser);
        return retValue;
    } 
    return 1;
}
