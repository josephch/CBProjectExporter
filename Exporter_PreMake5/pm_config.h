#ifndef PM_CONFIG_H
#define PM_CONFIG_H

#include "pm_base.h"
class pm_settings;

// project configuration (a.k.a. build target)
class pm_config : public pm_base
{
    public:
        pm_config();
        virtual ~pm_config();

        // return C::B config name
        virtual wxString name() const = 0;

        // settings on config level
        virtual std::shared_ptr<pm_settings> settings() = 0;

        // type of build target "ConsoleApp", "SharedLib" etc.
        virtual wxString kind() = 0;

        // return name of compiler for this config
        virtual wxString compiler() const = 0;

        // is this a debug configuration?
        virtual bool is_debug() const = 0;

        // export to premake5
        virtual void premake_export(std::ostream & out);
};

#endif // PM_CONFIG_H
