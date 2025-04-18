#pragma once

#include "BasicTypes.h"
#include "DynArray.h"
#include "LibCoreExportOs.h"
#include <functional>
#include <string>

#ifdef OS_WINDOWS
#pragma warning(disable : 4251)
#endif

class CmdLineParserUT;

namespace TL
{
namespace LibCore
{
/// Used to track, parse and validate the given command-line based on the
/// different given parameters (of 3 types: arguments / swtiches /  options).
/// Application usage is generated automatically if incorrect number of paramaters
/// or wrong option names are given - everything outputs to console.
class LIBCORE_EXPORT CmdLineParser
{
    static constexpr u32 MAX_PARAMS{20};
    struct Param
    {
        /// Ctor for either an mandatory argument or optional switch
        Param() = default;
        Param(const std::string& paramName, const std::string& desc, bool isSwitch,
            bool checkPartialName, bool isHidden = false);
        /// Ctor for an option with a default value (isOptional=true)
        Param(const std::string& paramName, const std::string& defValue, const std::string& desc,
            bool checkPartialName, bool isHidden = false);
        Param(Param const& rhs) = default;
        ~Param() = default;

        void outputParamName() const;
        inline bool isMandatoryArgument() const { return !isOptional && !isSwitch; }

        std::string name;
        std::string value;
        std::string desc;
        bool isSwitch;
        bool isOptional;
        bool checkPartialParamName;
        std::string actualPartialArgName;

        /// The order-rank in which the paramter was read off the command-line.
        u32 rank;
        bool hidden;
    };

    using Visitor = std::function<void(std::string const& paramName, std::string const& paramValue)>;
    using Params = TL::LibCore::DynArray<Param, MAX_PARAMS>;

public:
    CmdLineParser();
    virtual ~CmdLineParser();

    /// \verbatim
    /// Method to setup supported parameters. We define parameters in 3 categories:
    /// Arguments, in the cmd-line, are mandatory. Desc should be: <Arg1>
    /// Options, in the cmd-line, are optional. They usually have a ass. default value. Desc should be: [-option1 value] where value is ....
    /// Switches, in the cmd-line, are optional. They usually are booleans. Desc should be: [-option1] Turn on....
    /// The mandatory arguments must be in the same order in the parsed cmd-line as the order they are defined here. \verbatim
    bool addArgument(const std::string& argName, const std::string& desc, bool allowPartialName = false,
        bool isHidden = false);
    bool addOption(const std::string& optionName, const std::string& optDefValue, const std::string& desc,
        bool allowPartialName = false, bool isHidden = false);
    bool addSwitch(const std::string& switchName, const std::string& desc,
        bool allowPartialName = false, bool isHidden = false);

    /// Parses the actual given command-line and matches argument/switch/option
    /// given. If a visitor is given, the associated handler of the argument/swith/option
    /// will be called immediately, as it is found and parsed from the cmd-line.
    bool validateCmdLine(u32 argc, const char** argv, Visitor visitor = Visitor{nullptr}, bool ignoreTM = false);

    /// Once ValidateCmdLine() has been called, prints the values of all knonw parameters to sdt::cout.
    bool showParameterValues(std::string const& title);

    /// Query of values
    /// Returns an actual argument/option/switch value. Arguments, in the cmd-line, are mandatory.
    /// Options are optional (they require an associated value). Switches are usually booleans -
    /// so they are optional. They don't require an associated value.
    /// Returns false if argument/option/switch name is not found when validating the cmd-line.
    /// When getting a value for an option or swith, returning false means use default value.
    /// If rank is provided, is returns the rank order (1st, 2nd, 3rd, etc) in which the
    /// argument/option/switch was read off the command-line.
    bool getValue(std::string const& name, std::string& value, u32* rank = nullptr);

    /// A simple query method for switch whcih returns true or false, based on switch from cmd line.
    bool hasSwitch(std::string const& switchName);

private:
    void showUsageInConsole();
    Param* findParam(std::string const& arg);
    Param* findParam(u32 index);
    inline u32 getArgumentCnt() const
    {
        u32 argCnt = 0;
        for(auto const& param : _params)
        {
            if(param.isMandatoryArgument())
            {
                // This is an argument.
                ++argCnt;
            }
        }
        return argCnt;
    }

    // Data members
    Params _params;
    std::string _execName;

    friend class ::CmdLineParserUT;
};
} // namespace LibCore
} // namespace TL
