#pragma once

#include "BasicTypes.h"
#include "LibUtilExportOs.h"
#include <list>
#include <memory>
#include <string>

#ifdef OS_WINDOWS
#pragma warning(disable : 4251)
#endif

namespace WS
{
namespace Util
{
/// Installs, temporarily, a private font in the OS tables for private use
/// by the application. The font remains until this object is destroyed.
class LIBUTIL_EXPORT PluginLoader
{
public:
    class LIBUTIL_EXPORT Module
    {
    public:
        Module(std::string const& modName, void* entryPt);
        Module(Module const& rhs) = delete;
        Module(Module&& rhs) = default;
        virtual ~Module();

        /// Retrieves the ptr to a function pr variable in the module.
        virtual void* getProcAddress(std::string const& funcName) = 0;
        virtual void unload() = 0;

        inline std::string const& getName() const;
        inline void* getEntryPoint() const;

    private:
        Module& operator=(Module const& rhs) = delete;

    private:
        std::string mName;
        void* mEntryPoint;
    };

    /// Constructor/destructor
    PluginLoader();
    PluginLoader(PluginLoader const& rhs) = delete;
    PluginLoader(PluginLoader&& rhs) = default;
    virtual ~PluginLoader();

    /// Loads in the given module (DLL, plugin, etc). moduleName is the full path name.
    /// You can also specify a function entry name and a suggestedFolder as to where
    /// to look for the module (Windows mostly).
    Module* loadModule(std::string const& moduleName, std::string const* entryName = nullptr,
        std::string const* suggestedFolder = nullptr);
    Module* getModule(std::string const& moduleName);

    /// Unloads the given module (by name or reference) (and removes it from the list of loaded modules)
    bool unloadModule(std::string const& moduleName);
    bool unloadModule(Module* module);

private:
    /// Keep a list of all of the modules loaded in memory.
    std::list<std::unique_ptr<Module>> mModList;
};

//-----------------------------------------------------------------------------
//
std::string const&
PluginLoader::Module::getName() const
{
    return mName;
}

//-----------------------------------------------------------------------------
//
void* PluginLoader::Module::getEntryPoint() const
{
    return mEntryPoint;
}

namespace OS
{
std::unique_ptr<WS::Util::PluginLoader::Module> loadPluginModule(std::string const& moduleName, std::string const* entryName, std::string const*);
}

} // namespace Util
} // namespace WS