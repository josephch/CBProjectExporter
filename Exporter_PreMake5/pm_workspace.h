#ifndef PM_WORKSPACE_H
#define PM_WORKSPACE_H

#include "pm_base.h"
#include <wx/filename.h>
#include "pm_utils.h"
class pm_project;
class pm_defaults;
class pm_settings;
class cbProject;

// workspace class
class pm_workspace : public pm_base
{
    public:
        pm_workspace();
        virtual ~pm_workspace();

        // return full filename of the original cb workspace
        virtual wxFileName filename() const = 0;

        // return true if all projects are in subdirs of the workspace path
        virtual bool is_local_workspace() const = 0;

        // return workspace name only
        virtual wxString name()  const
        {
            return filename().GetName();
        }

        // return name of build location
        virtual wxString location_path() const = 0;

        // return number of projects in workspace
        virtual size_t size() const = 0;

        // project traversal
        virtual pm_project_iterator begin() = 0;
        virtual pm_project_iterator end() = 0;

        // default settings
        virtual std::shared_ptr<pm_defaults> defaults() = 0;

        // return workspace settings
        virtual std::shared_ptr<pm_settings> settings() = 0;

        // return dependencies for project
        virtual pm_project_vec dependencies(const cbProject * cbproject) const = 0;

        // export to premake
        virtual void premake_export(std::ostream & out);
};

#endif // PM_WORKSPACE_H
