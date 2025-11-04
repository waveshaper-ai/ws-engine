/**
 * @file AudioModel.h
 * @brief Main interface for the WaveShaper Infernce Engine
 * 
 * The WaveShaper Inference Engine provides developers with simple access to powerful
 * audio processing AI models (such as MicUpgrade, SpectralEnhancement, etc).
 * The WaveShaper Inference Engine is a lightweight and cross-platform library that can 
 * be easily integrated into applications. The library can load pre-trained AI models and use these for
 * audio processing to perform various tasks.
 */

#ifndef _WS_AUDIOMODEL_H
#define _WS_AUDIOMODEL_H

#include "LibAiExportOs.h"
#include <functional>
#include <memory>
#include <string>

class AudioModelUT;

namespace WS
{
class Logger;

/**
 * @class AudioModel
 * @brief Main entry point class to handle AI audio processing
 * 
 * The AudioModel class provides the primary interface for loading and using
 * AI models for audio processing. It supports various activation functions,
 * parameter setting, and model processing capabilities.
 * 
 * @code{.cpp}
 * // Basic usage example
 * static const std::string ACTIVATION{"tanh"};
 * std::unique_ptr<AudioModel> audioModel{new AudioModel(ACTIVATION)};
 * 
 * // Load and prepare the model binaries (.dat files)
 * std::string modelName = "/path/to/model/";
 * if(!audioModel->prepare(modelName)) {
 *     std::cout << "ERROR: Could not prepare the model properly." << std::endl;
 *     return 1;
 * }
 * 
 * // Set parameters if needed
 * if(audioModel->getNumberOfParams() > 0) {
 *     audioModel->setParamValueAt(0, 0.5f);
 * }
 * 
 * // Process audio
 * audioModel->process(inputBuffer, outputBuffer);
 * @endcode
 */
class LIB_AI_EXPORT AudioModel
{
public:
    /**
     * @brief Constructor for AudioModel
     * @param activation The activation function to use (default: "tanh")
     * 
     * Creates a new AudioModel instance with the specified activation function.
     * Unless otherwise stated, most of the WS AI models use "tanh"
     */
    AudioModel(std::string const& activation = "tanh", unsigned int sampleRate = 44100);

    /**
     * @brief Destructor for AudioModel
     */
    ~AudioModel();

    AudioModel(const AudioModel&) = delete;
    AudioModel& operator=(const AudioModel&) = delete;


      /**
     * @brief Loads and prepares the model for processing
     * @param modelName Path to the model directory
     * @return true if successful, false if the model cannot be found or loaded
     * 
     * Prepare needs to be called once to allocate the proper layers before
     * calling process() multiple times for processing audio frames.
     * 
     * @code{.cpp}
     * std::string modelPath = "/path/to/model/";
     * if(!audioModel->prepare(modelPath)) {
     *     std::cout << "Error: Could not prepare the model properly." << std::endl;
     *     return 1;
     * }
     * @endcode
     */
    bool prepare(std::string const& modelName);

    /**
     * @brief Adds a new parameter to the model
     * @param param Name of the parameter
     * 
     * Parameters will be added in order and indexed according to their order:
     * - First parameter added is index 0
     * - Second parameter added is index 1
     * - And so on...
     */
    void setNewParam(std::string const& param);

    /**
     * @brief Adds license 
     * @param license License to use
     * 
     * This is just a placeholder for a license checker
     * We need a way to manage the license
     */
    bool setLicense(std::string const& license);

    /**
     * @brief Sets the value of a parameter by index
     * @param param Index of the parameter
     * @param value Double value to set (0.0 to 1.0 recommended)
     */
    void setParamValueAt(size_t param, double value);

    /**
     * @brief Sets the value of a parameter by index
     * @param param Index of the parameter
     * @param value Float value to set (0.0 to 1.0 recommended)
     */
    void setParamValueAt(size_t param, float value);

    /**
     * @brief Sets the value of a parameter by name
     * @param param Name of the parameter
     * @param value Double value to set (0.0 to 1.0 recommended)
     */
    void setParamValueAt(std::string const& param, double value);

    /**
     * @brief Sets the value of a parameter by name
     * @param param Name of the parameter
     * @param value Float value to set (0.0 to 1.0 recommended)
     */
    void setParamValueAt(std::string const& param, float value);

    void setBufferManual(float* inputPrev, float* input, float* inputNext=nullptr);
    
    /**
     * @brief Processes audio through the model
     * @param input Pointer to input buffer of audio samples (size must match getFrameLength())
     * @param output Pointer to output buffer where processed audio will be stored

     * @return true if processing was successful, false otherwise
     * 
     * @code{.cpp}
     * // Process example
     * float* inputBuffer = new float[audioModel->getFrameLength()];
     * float* outputBuffer = new float[audioModel->getFrameLength()];
     * // Fill inputBuffer with audio samples...
     * 
     * if(audioModel->process(inputBuffer, outputBuffer)) {
     *     // Use processed audio in outputBuffer...
     * }
     * @endcode
     */
    bool process(const float* input, float* output);

    // Accessors

    std::string const& getModelType() const;
    
     /**
     * @brief Gets the frame length required for processing
     * @return Number of samples per frame
     */
    size_t getFrameLength() const;

    size_t getHopLength() const;

    /**
     * @brief Gets the number of filters in the model
     * @return Number of filters
     */
    size_t getNumberOfFilters() const;

    /**
     * @brief Gets the dense local size of the model
     * @return Dense local size
     */
    size_t getDenseLocalSize() const;

    /**
     * @brief Gets the max pooling pool size
     * @return Max pooling pool size
     */
    size_t getMaxPoolingPoolSize() const;

    /**
     * @brief Gets the number of parameters supported by the model
     * @return Number of parameters
     */
    size_t getNumberOfParams() const;

    /**
     * @brief Sets a logger for the model
     * @param logger Pointer to a Logger instance
     */
    void setLogger(WS::Logger* logger);
    
    /**
     * @brief Gets validation values for a specific buffer
     * @param bufferName Name of the buffer
     * @param filterCnt Output parameter to receive filter count
     * @param sampleCnt Output parameter to receive sample count
     * @return Pointer to the validation values array
     */
    float* getValidationValues(const std::string& bufferName, size_t& filterCnt, size_t& sampleCnt ,size_t ctxIdx = 1);

    class Impl;

private:
    Impl* pImpl;

    float oneminus(float x);
    float softplus(float x);
    float* getBreakPoints(size_t numBreakPoints);

    WS::Logger* mLogger{nullptr};

    friend class ::AudioModelUT;
};
} // namespace WS

#endif
