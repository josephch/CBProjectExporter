#ifndef PM_WORKSPACE_CB_H
#define PM_WORKSPACE_CB_H

#include <map>
#include "pm_workspace.h"
#include "pm_utils.h"
class pm_settings;
class cbProject;
class pm_project_cb;
class pm_defaults;

// Code::Blocks aware workspace class
class pm_workspace_cb : public pm_workspace
{
    public:
        pm_workspace_cb(std::shared_ptr<pm_defaults> defaults);
        virtual ~pm_workspace_cb();

        // return full filename of the original cb workspace
        virtual wxFileName filename() const;

        // return true if all projects are in subdirs of the workspace path
        bool is_local_workspace() const;

        // return name of build location
        virtual wxString location_path() const;

        // return number of projects in workspace
        virtual size_t size() const
        {
            return m_projects.size();
        }

        // project traversal
        virtual pm_project_iterator begin()
        {
            return m_projects.begin();
        }
        virtual pm_project_iterator end()
        {
            return m_projects.end();
        }

        // default settings
        virtual std::shared_ptr<pm_defaults> defaults()
        {
            return m_defaults;
        }

        // workspace level settings
        virtual std::shared_ptr<pm_settings> settings()
        {
            return m_settings;
        }

        // resolve project dependencies as declared in workspace
        pm_project_vec dependencies(const cbProject * cbproject) const;

    protected:
        void get_projects();
        void get_dependencies();

    private:
        using pm_project_map =  std::map<cbProject *, std::shared_ptr<pm_project_cb>>;

        std::shared_ptr<pm_defaults> m_defaults;  // default settings from gui

        pm_project_vec               m_projects;
        pm_project_map               m_pmap;

        std::shared_ptr<pm_settings> m_settings;  // settings on workspace level
};

#endif // PM_WORKSPACE_CB_H
