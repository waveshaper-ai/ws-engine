#pragma once

#include "BasicTypes.h"
#include "LibUtilExportOs.h"
#include <functional>
#include <thread>

namespace WS
{
namespace Util
{
class ProcessNode;
class ParamContext;
class OutputResult;

/// Encapsulates a boost thread and using the ProcessThread
/// given a set of ParamContent/OutputResult at construction.
/// The AsyncRequest object is used to perform an async ComputeResult()
/// call using the ProcessThread object.
/// the ParamContext/OutputResult and ProcessNode must have object lifetime
/// longer than this request.
/// When the object is destructed, the thread is guaranteed to be completed
/// and dead. The ProcessNode's ProcessingInProgress() is reset to false
/// (and reports 100% when done).
class LIBUTIL_EXPORT AsyncComputeRequest
{
public:
    using ProcessCompletion = std::function<void(bool success, u32 completion, OutputResult& result)>;
    ProcessNode& mProcessNode;
    ParamContext& mParamContext;
    OutputResult& mOutputResult;
    ProcessCompletion mProcessCompletion;
    std::thread mThread;

    AsyncComputeRequest(ProcessNode& procNode, ParamContext& paramCtx, OutputResult& outResult, ProcessCompletion const& procCompletion);
    ~AsyncComputeRequest();

private:
    void threadEntryPoint();
};

} // namespace Util
} // namespace WS
