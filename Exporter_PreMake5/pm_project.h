#ifndef PM_PROJECT_H
#define PM_PROJECT_H

#include <wx/filename.h>

#include "pm_base.h"
#include "pm_utils.h"
class pm_settings;
class pm_defaults;

// project class
class pm_project : public pm_base
{
    public:
        pm_project();
        virtual ~pm_project();

        // return full filename of the original cb project
        virtual wxFileName filename()  const = 0;

        // return true if project is in a subdir of the workspace path
        virtual bool is_workspace_subdir() const = 0;

        // return project name only
        virtual wxString name()
        {
            return filename().GetName();
        }

        // premake5 location path
        virtual wxString location_path() = 0;

        // return relative path from workspace to project directory
        virtual wxString relative_path() const = 0;

        // return relative path from workspace to parent of project directory
        virtual wxString relative_parent_path() const = 0;

        // iterate over project (source) files
        virtual size_t size() const = 0;
        virtual pm_file_iterator begin() = 0;
        virtual pm_file_iterator end() = 0;

        // project level settings
        virtual std::shared_ptr<pm_settings> settings() = 0;

        // default settings
        virtual std::shared_ptr<pm_defaults> defaults() = 0;

        // iterate over configurations
        virtual pm_config_iterator config_begin() = 0;
        virtual pm_config_iterator config_end() = 0;

        // return project dependencies as declared in workspace
        virtual pm_project_vec dependencies() const = 0;

        // resolve includes based on dependencies
        virtual void resolve_includes() = 0;

        // export to premake5
        virtual void premake_export(std::ostream & out);
};

#endif // PM_PROJECT_H
