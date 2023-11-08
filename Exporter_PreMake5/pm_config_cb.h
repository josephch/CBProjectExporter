#ifndef PM_CONFIG_CB_H
#define PM_CONFIG_CB_H

#include "pm_config.h"
class ProjectBuildTarget;
class pm_defaults;

// Code::Blocks aware config (build target) class
class pm_config_cb : public pm_config
{
    public:
        pm_config_cb(ProjectBuildTarget * cbtarget, std::shared_ptr<pm_defaults> defaults);
        virtual ~pm_config_cb();

        // return config name
        virtual wxString name() const
        {
            return m_name;
        }

        // settings for config
        virtual std::shared_ptr<pm_settings> settings()
        {
            return m_settings;
        }

        // kind of config (=type of build target)
        wxString kind();

        // is this config using debug settings?
        bool is_debug() const;

        // return name of compiler for this config
        wxString compiler() const
        {
            return m_compiler;
        }

    protected:

        void get_defines();

    private:
        ProjectBuildTarget     *     m_cbtarget;  // C::B build target
        std::shared_ptr<pm_defaults> m_defaults;
        std::shared_ptr<pm_settings> m_settings;  // settings on config/target level
        bool                         m_is_debug;  // this is a debug config
        wxString                     m_name;      // name of config
        wxString                     m_compiler;  // name of compiler for this config
};

#endif // PM_CONFIG_CB_H
