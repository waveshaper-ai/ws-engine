#pragma once

#include "AudioSpec.h"
#include "OutputResult.h"
#include "OutputSpec.h"
#include "ProcessNode.h"
#include <chrono>
#include <cmath>

namespace WS
{
class PipelineTopo;

namespace AI
{
class AIProcessNode : public Util::ProcessNode
{
public:
    AIProcessNode(u32 id, std::string const& name) : Util::ProcessNode(id, name)
    {
    }

    /// Similar to AudioModel::prepare() method, this prepare() superclass implementation
    /// does nothing but return true. Derived class nodes should use this as a one-time
    /// initialization before process() is called for each packet of audio to be processed.
    virtual bool prepare(PipelineTopo const&) { return true; }
    virtual void cleanupCachedPacket() override {}

    /// A method use to get the specifications of the outputted packets.
    virtual bool getOutputSpecs(Util::OutputSpec&outSpecs)
    {
        getChildSpecs(outSpecs);
        return true;
    }    

protected:
    virtual void getChildSpecs(Util::OutputSpec& outInputSpecs)
    {
        // Optimize for 1 input configuration
        if(getInputCnt() == 1U)
        {
            AI::AIProcessNode& aipn{dynamic_cast<AI::AIProcessNode&>(getInput(0U))};
            aipn.getOutputSpecs(outInputSpecs);
        }
        else
        {
            for(auto iter : getInputs())
            {
                if(iter != nullptr)
                {
                    Util::OutputSpec inOS;
                    AI::AIProcessNode* aipn{dynamic_cast<AI::AIProcessNode*>(iter)};
                    if(aipn != nullptr && aipn->getOutputSpecs(inOS))
                    {
                        // For now, assume this PrcoessNode has only one input
                        outInputSpecs = inOS;
                    }
                }
            }
        }
    }
};

} // namespace AI
} // namespace WS