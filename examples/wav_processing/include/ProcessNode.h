#pragma once

#include "JsonReader.h"
#include "JsonWriter.h"
#include "OutputResult.h"
#include "ParamContext.h"
#include "ScriptObject.h"
#include "Subject.h"
#include <array>
#include <string>

namespace WS
{
namespace Util
{
class ParamContext;

/// Basic node element part of a pipeline.
class LIBUTIL_EXPORT ProcessNode : public ScriptObject
{
public:
    static const u32 MAX_INPUT_NODES{10};
    using InputsArray = std::array<ProcessNode*, MAX_INPUT_NODES>;

    explicit ProcessNode(u32 uniqueId, std::string const& name);
    explicit ProcessNode(u32 uniqueId);
    virtual ~ProcessNode() = default;

    /// Operations on the class
    virtual bool initialize();

    /// Generates the requested (through ParamContext) output packets
    /// cached in this node itself and outputting whatever this processing node
    /// is generating. Document what your derived node is producing so it is
    /// easy to link to your node if necessary.
    /// Returns if processing was successful (true) or aborted (false).
    virtual bool computeResult(ParamContext& ctx, OutputResult& outResult) = 0;

    // Returns/sets the overall progress of this PN and its input nodes.
    int getProcessingCompletion() const;
    void setProgressCompletion(u32 percent) { mProcessingProgress = percent; }

    /// Configures the number of inputs necessary for this ProcessNode.
    /// relativeInputsProgPercent is set as int [0-100]. Default 60%.
    bool configInputs(u32 maxInputCnt, u32 relativeInputsProgPercent = 40);

    /// Connects this node with other ProcessNode(s), with an input node index
    /// smaller than the maxInputCnt set with ConfigInputs(). Anything higher
    /// or null otherProcNode returns false. inputIndex is 0 based, so 0 is the
    /// first input.
    bool connectInput(u32 inputIndex, ProcessNode& otherProcNode);

    /// Disconnects this node with the given ProcessNode.
    /// Attemps to find the given node from mInputs.
    /// If found, remove it from mInputs then fills the hole by
    /// shifting the following inputs.
    /// @param otherProcNode The processNode to disconnect with this node.
    /// @return True if found and removed. False otherwise.
    bool disconnectInput(ProcessNode& otherProcNode);

    // Accessors
    inline Subject<bool>& getInputChangedSub() { return mInputChangedSub; }
    inline u32 getInputCnt() const { return mInputCnt; }
    inline InputsArray& getInputs() { return mInputs; }
    inline ProcessNode& getInput(u32 index) { return *mInputs[index]; }
    inline std::string getName() { return mName; }
    inline bool processingCompleted() const { return mProcessingProgress == 100; }

protected:
    /// The main method to process all the inputs and return their results
    /// into the given "outInputResults". The OutputResult type must be the same for
    /// all ProcessNode(s) wihtin a system. "mapping" refers to a vector that
    /// matches an input index (0-based) with the index of the outInputResults.
    /// Not providing a "mapping" would mean that 1 output of the input maps
    /// to the first index (at 0) of the outInputResults.
    template <class OUTPUTRESULT>
    bool computeInputResults(ParamContext& ctx, OUTPUTRESULT& outInputResults);

    bool readFromStream(JsonReader& reader) override;
    bool writeToStream(JsonWriter& writer) override;

    /// To handle notifications of the update of an input node
    virtual void onInputChanged(bool);

    /// Accessor for derived class
    void setCacheDirty();
    virtual void cleanupCachedPacket() {};

private:
    std::string mName;
    u32 mInputCnt;

    /// To notify that this node has been updated
    Subject<bool> mInputChangedSub;

    /// Relative input progress (compare to this processing progress).
    /// Value is in percent [0.F-1.F]. Inputs+this progress = 1;
    float mRelInputsProgress;
    u32 mProcessingProgress;

    /// The actual input nodes to this one.
    InputsArray mInputs;
};

template <class OUTPUTRESULT>
bool ProcessNode::computeInputResults(ParamContext& ctx, OUTPUTRESULT& outInputResults)
{
    bool packetsValid{false};
    u32 index{0};
    if(getInputCnt() == 1U)
    {
        packetsValid = getInput(0U).computeResult(ctx, outInputResults);
    }
    else
    {
        for(auto iter : mInputs)
        {
            if(iter != nullptr)
            {
                OUTPUTRESULT inOR;
                if(!iter->computeResult(ctx, inOR))
                {
                    packetsValid = false;
                    break;
                }
                appendPacket0AtIndex(outInputResults, index++, inOR);
                packetsValid = true;
            }
        }
    }
    return packetsValid;
}

} // namespace Util
} // namespace WS
