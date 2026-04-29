//=======================================================================
/** @file FileHelper.h
 *  @author Adam Stark
 *  @copyright Copyright (C) 2017  Adam Stark
 *
 * This file is part of the 'AudioFile' library
 *
 * MIT License
 *
 * Copyright (c) 2017 Adam Stark
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
 * of the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
 * PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
 * OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */
//=======================================================================

#pragma once

#if defined(_MSC_VER)
#undef max
#undef min
#define NOMINMAX
#endif

#include "AudioPacket.h"
#include <algorithm>
#include <cassert>
#include <cstdint>
#include <cstring>
#include <fstream>
#include <iostream>
#include <iterator>
#include <limits>
#include <string>
#include <unordered_map>
#include <vector>

// disable some warnings on Windows
#if defined(_MSC_VER)
__pragma(warning(push))
    __pragma(warning(disable : 4244))
        __pragma(warning(disable : 4457))
            __pragma(warning(disable : 4458))
                __pragma(warning(disable : 4389))
                    __pragma(warning(disable : 4996))
#elif defined(__GNUC__)
_Pragma("GCC diagnostic push")
    _Pragma("GCC diagnostic ignored \"-Wconversion\"")
        _Pragma("GCC diagnostic ignored \"-Wsign-compare\"")
            _Pragma("GCC diagnostic ignored \"-Wshadow\"")
#endif

namespace WS
{
namespace Util
{
/// Basic file helper to encode endianness and binary data.
class FileHelper
{
public:
    /// The different types of audio file, plus some other types to
    /// indicate a failure to load a file.
    enum AudioFileFormat {
        NotLoaded,
        Wave,
        Aiff
    };

    enum WavAudioFormat
    {
        PCM = 0x0001,
        IEEEFloat = 0x0003,
        ALaw = 0x0006,
        MULaw = 0x0007,
        Extensible = 0xFFFE
    };

    enum AIFFAudioFormat
    {
        Uncompressed,
        Compressed,
        Error
    };

    enum class Endianness
    {
        LittleEndian,
        BigEndian
    };

    //=============================================================
    // Make a FileHelper empty. 
    FileHelper(Endianness endianness, uint32_t bitDepth) : mFileData{}, mXMLChunk{}, mBitDepth{bitDepth}, mEndianness{endianness}
    {
    }

    bool writeDataToFile(std::string const&filePath, bool writeHeader);

    void setXMLChunk(std::string const&xmlChunk) { mXMLChunk = xmlChunk; }


    //=============================================================
    template <class T>
    T clamp(T value, T minValue, T maxValue)
    {
        value = std::min(value, maxValue);
        value = std::max(value, minValue);
        return value;
    }

    template <typename SAMPLE_TYPE, typename TO_TYPE>
    TO_TYPE addSampleForBitDepthType(SAMPLE_TYPE sample);

    template<typename BITDEPTH_TYPE>
    bool writeDataChunkToFile(WS::Util::AudioPacket const&packet)
    {
        mFileData.clear();
        for(u32 i{0U}; i < packet.accumSamples(); ++i)
        {
            addSampleForBitDepthType<float, BITDEPTH_TYPE>(*packet.bufL(i));
            if(packet.isStereo())
            {
                addSampleForBitDepthType<float, BITDEPTH_TYPE>(*packet.bufR(i));
            }
        }
        return true;
    }

    //=============================================================
    void addStringToFileData(std::string const&s);
    void addInt32ToFileData(int32_t i, Endianness endianness = Endianness::LittleEndian);
    void addInt16ToFileData(int16_t i, Endianness endianness = Endianness::LittleEndian);

    //=============================================================
    void reportError(std::string const& errorMessage);

    //=============================================================
private:
    std::vector<uint8_t> mFileData;
    /// An optional iXML chunk that can be added to the AudioFile.
    std::string mXMLChunk;
    uint32_t mBitDepth;
    Endianness mEndianness;
    bool logErrorsToConsole{true};
};

//=============================================================
// Explicit specializations of FileHelper::addSampleForBitDepthType (must be at
// namespace scope — explicit specializations inside a class body are ill-formed).

template <>
inline int8_t FileHelper::addSampleForBitDepthType<float, int8_t>(float sample)
{
    sample = clamp<float>(sample, -1., 1.);
    sample = (sample + 1.) / 2.;
    int8_t value{static_cast<int8_t>(sample * 255.)};
    mFileData.push_back(value);
    return value;
}

template <>
inline int16_t FileHelper::addSampleForBitDepthType<float, int16_t>(float sample)
{
    sample = clamp<float>(sample, -1., 1.);
    sample = (sample + 1.) / 2.;
    int16_t value{static_cast<int16_t>(sample * 32767.)};
    addInt16ToFileData(value, mEndianness);
    return value;
}

/// Use this version when in PCM 32
template <>
inline int32_t FileHelper::addSampleForBitDepthType<float, int32_t>(float sample)
{
    if(mBitDepth == 24)
    {
        int32_t sampleAsIntAgain = static_cast<int32_t>(sample * 8388608.f);
        uint8_t bytes[3];
        bytes[2] = static_cast<uint8_t>(sampleAsIntAgain >> 16) & 0xFF;
        bytes[1] = static_cast<uint8_t>(sampleAsIntAgain >> 8) & 0xFF;
        bytes[0] = static_cast<uint8_t>(sampleAsIntAgain) & 0xFF;
        mFileData.push_back(bytes[0]);
        mFileData.push_back(bytes[1]);
        mFileData.push_back(bytes[2]);
        return sampleAsIntAgain;
    }
    else if(mBitDepth == 32)
    {
        int32_t sampleAsInt = static_cast<int32_t>(sample * std::numeric_limits<int32_t>::max());
        addInt32ToFileData(sampleAsInt, mEndianness);
        return sampleAsInt;
    }
    return 0;
}

// Use this version when in IEEEFloat
template <>
inline float FileHelper::addSampleForBitDepthType<float, float>(float sample)
{
    int32_t sampleAsInt{};
    std::memcpy(&sampleAsInt, &sample, sizeof(int32_t));
    addInt32ToFileData(sampleAsInt, mEndianness);
    return sample;
}

//=============================================================
// Pre-defined 10-byte representations of common sample rates
static std::unordered_map<uint32_t, std::vector<uint8_t>> aiffSampleRateTable = {
    {8000, {64, 11, 250, 0, 0, 0, 0, 0, 0, 0}},
    {11025, {64, 12, 172, 68, 0, 0, 0, 0, 0, 0}},
    {16000, {64, 12, 250, 0, 0, 0, 0, 0, 0, 0}},
    {22050, {64, 13, 172, 68, 0, 0, 0, 0, 0, 0}},
    {32000, {64, 13, 250, 0, 0, 0, 0, 0, 0, 0}},
    {37800, {64, 14, 147, 168, 0, 0, 0, 0, 0, 0}},
    {44056, {64, 14, 172, 24, 0, 0, 0, 0, 0, 0}},
    {44100, {64, 14, 172, 68, 0, 0, 0, 0, 0, 0}},
    {47250, {64, 14, 184, 146, 0, 0, 0, 0, 0, 0}},
    {48000, {64, 14, 187, 128, 0, 0, 0, 0, 0, 0}},
    {50000, {64, 14, 195, 80, 0, 0, 0, 0, 0, 0}},
    {50400, {64, 14, 196, 224, 0, 0, 0, 0, 0, 0}},
    {88200, {64, 15, 172, 68, 0, 0, 0, 0, 0, 0}},
    {96000, {64, 15, 187, 128, 0, 0, 0, 0, 0, 0}},
    {176400, {64, 16, 172, 68, 0, 0, 0, 0, 0, 0}},
    {192000, {64, 16, 187, 128, 0, 0, 0, 0, 0, 0}},
    {352800, {64, 17, 172, 68, 0, 0, 0, 0, 0, 0}},
    {2822400, {64, 20, 172, 68, 0, 0, 0, 0, 0, 0}},
    {5644800, {64, 21, 172, 68, 0, 0, 0, 0, 0, 0}}};


bool FileHelper::writeDataToFile(std::string const&filePath, bool writeHeader)
{
    std::ofstream outputFile;
    if(writeHeader)
    {
        outputFile.open(filePath, std::ios::binary);
    }
    else
    {
        outputFile.open(filePath, std::ios::app);
    }

    if(outputFile.is_open())
    {
        for(size_t i = 0; i < mFileData.size(); i++)
        {
            char value = static_cast<char>(mFileData[i]);
            outputFile.write(&value, sizeof(char));
        }

        outputFile.close();
        return true;
    }
    return false;
}

//=============================================================
void FileHelper::addStringToFileData(std::string const&s)
{
    for(size_t i = 0; i < s.length(); i++)
        mFileData.push_back(static_cast<uint8_t>(s[i]));
}

//=============================================================
void FileHelper::addInt32ToFileData(int32_t i, Endianness endianness)
{
    uint8_t bytes[4];

    if(endianness == Endianness::LittleEndian)
    {
        bytes[3] = (i >> 24) & 0xFF;
        bytes[2] = (i >> 16) & 0xFF;
        bytes[1] = (i >> 8) & 0xFF;
        bytes[0] = i & 0xFF;
    }
    else
    {
        bytes[0] = (i >> 24) & 0xFF;
        bytes[1] = (i >> 16) & 0xFF;
        bytes[2] = (i >> 8) & 0xFF;
        bytes[3] = i & 0xFF;
    }

    for(int i = 0; i < 4; i++)
        mFileData.push_back(bytes[i]);
}

//=============================================================
void FileHelper::addInt16ToFileData(int16_t i, Endianness endianness)
{
    uint8_t bytes[2];

    if(endianness == Endianness::LittleEndian)
    {
        bytes[1] = (i >> 8) & 0xFF;
        bytes[0] = i & 0xFF;
    }
    else
    {
        bytes[0] = (i >> 8) & 0xFF;
        bytes[1] = i & 0xFF;
    }

    mFileData.push_back(bytes[0]);
    mFileData.push_back(bytes[1]);
}

//=============================================================
void FileHelper::reportError(std::string const&errorMessage)
{
    if(logErrorsToConsole)
        std::cout << errorMessage << std::endl;
}

#if defined(_MSC_VER)
__pragma(warning(pop))
#elif defined(__GNUC__)
_Pragma("GCC diagnostic pop")
#endif

} // namespace Util
} // namespace WS
