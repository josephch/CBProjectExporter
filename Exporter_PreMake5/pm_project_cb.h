#ifndef PM_PROJECT_CB_H
#define PM_PROJECT_CB_H

#include "pm_project.h"
#include "pm_utils.h"
class pm_workspace;
class cbProject;
class pm_regex;
class pm_settings;
class pm_defaults;

// Code::Blocks aware project class
class pm_project_cb : public pm_project
{
    public:
        pm_project_cb(pm_workspace * ws, cbProject * cbproject);
        virtual ~pm_project_cb();

        // return full filename of the original cb project
        virtual wxFileName filename() const;

        // return true if project is in a subdir of the workspace path
        bool is_workspace_subdir() const;

        // premake5 location name
        virtual wxString location_path();

        // return relative path from workspace to project directory
        virtual wxString relative_path() const;

        // return relative path from workspace to parent of project directory
        virtual wxString relative_parent_path() const;

        // traverse files in project
        virtual size_t size() const
        {
            return m_files.size();
        }
        pm_file_iterator begin()
        {
            return m_files.begin();
        }
        pm_file_iterator end()
        {
            return m_files.end();
        }

        // settings on project level
        virtual std::shared_ptr<pm_settings> settings()
        {
            return m_settings;
        }

        // default settings
        virtual std::shared_ptr<pm_defaults> defaults();

        // traverse configurations in project
        pm_config_iterator config_begin()
        {
            return m_configs.begin();
        }
        pm_config_iterator config_end()
        {
            return m_configs.end();
        }

        // project dependencies, i.e. only build dependencies as declared in C::B
        virtual pm_project_vec dependencies() const;

        // add some "includedirs" based on dependencies. Not perfect...
        virtual void resolve_includes();

    protected:
        void get_files();
        void get_configs();
        void get_defines();

    private:
        pm_workspace       *      m_ws;           // parent workspace
        cbProject        *        m_cbproject;    // C::B project

        std::shared_ptr<pm_regex> m_regx;         // regular expression for project file inclusion

        pm_file_vec               m_files;        // project files
        pm_config_vec             m_configs;      // project configurations (=build targets)

        std::shared_ptr<pm_settings> m_settings;  // settings on project level
};

#endif // PM_PROJECT_CB_H
