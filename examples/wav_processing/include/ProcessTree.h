#pragma once

#include "LibUtilExportOs.h"
#include "ScriptSchema.h"

namespace WS
{
namespace Util
{
class ProcessNode;

class LIBUTIL_EXPORT ProcessTree : public ScriptSchema
{
public:
    using Nodes = std::vector<ProcessNode*>;
    using NodeUniqueIdFilter = std::function<u32(std::string const&, u32)>;

    ProcessTree(DescriptorRegistry& registry);
    virtual ~ProcessTree() = default;

    /// Simple method to return a process node according to its id.
    ProcessNode* getNodeById(u32 id);

    // Accessors
    inline Nodes const& getNodes() const { return mNodes; }
    inline ProcessNode* getMainOutputNode() const { return mMainOutputNode; }
    inline void setMainOutputNode(ProcessNode* mon) { mMainOutputNode = mon; }

    /// Allows to specify a custom filter/id modifier on the nodes to be
    /// saved. If filter returns 0, the visited node won't be saved.
    inline void setNodeIdFilter(NodeUniqueIdFilter const& filter) { mUniqueIdFilter = filter; }
    inline void setSectionInfo(std::string const& sectionName, bool skipMiscObject = false)
    {
        mSectionNameOverride = sectionName;
        mSkipMiscObject = skipMiscObject;
    }

private:
    bool createNewNodeReaderCmd(JsonReader& reader);
    bool createNewNodeWriterCmd(JsonWriter& writer);
    bool deleteNodeReaderCmd(JsonReader& reader);
    bool deleteNodeWriterCmd(JsonWriter& writer);
    bool newConnectionReaderCmd(JsonReader& reader);
    bool newConnectionWriterCmd(JsonWriter& writer);
    bool setMainOutputNodeReaderCmd(JsonReader& reader);
    bool setMainOutputNodeWriterCmd(JsonWriter& writer);
    bool writeToStream(JsonWriter& writer) override;
    bool readFromStream(JsonReader& reader) override;

    // Data members
private:
    Nodes mNodes;
    Nodes::iterator mNodeIter;

    /// Filter to be applied when writing this process tree. This allow
    /// to save partial parts of the process tree into specific sections.
    NodeUniqueIdFilter mUniqueIdFilter;

    /// Section information. This allows the user to override the section
    /// name and if the MiscObject section must be outputted, all of this
    /// to be read/written on the upcoming call to writeToStream/readFromStream
    std::string mSectionNameOverride;
    bool mSkipMiscObject;

    /// A reference to the main output node of this process tree.
    /// Set by script, maybe never set and therefore can be nullptr.
    ProcessNode* mMainOutputNode;
};

} // namespace Util
} // namespace WS
