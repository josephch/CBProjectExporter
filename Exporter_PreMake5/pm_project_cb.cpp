#include "pm_project_cb.h"
#include "pm_regex.h"
#include <cbproject.h>

#include "pm_workspace.h"
#include "pm_file_cb.h"
#include "pm_config_cb.h"
#include "pm_defaults.h"
#include "pm_settings.h"

pm_project_cb::pm_project_cb(pm_workspace * ws, cbProject * cbproject)
    : m_ws(ws)
    , m_cbproject(cbproject)
    , m_regx(pm_regex::default_cpp())
    , m_settings(std::make_shared<pm_settings>())
{
    if (m_ws->defaults()->get_bool_flag("use_project_defaults"))
    {
        m_settings = m_ws->defaults()->get_settings("project_defaults");
    }

    // include this project's path as include path explicitly
    // because sometimes we include files from subdirs of this project
    m_settings->push_back("includedirs", name());
    get_files();
    get_configs();
    get_defines();
}

pm_project_cb::~pm_project_cb()
{}

std::shared_ptr<pm_defaults> pm_project_cb::defaults()
{
    return m_ws->defaults();
}

wxString pm_project_cb::location_path()
{
    // this contains a LUA concatenation using '..' because
    // m_ws->location_path() returns a LUA function call statement
    // and the final LUA statement becomes something like
    //    location ( build_location().."/myproject" )
    return m_ws->location_path() + "..\"/" + relative_path() + "\"";
}

wxString pm_project_cb::relative_path() const
{
    // this is the relative (from workspace) path to this project folder
    wxString ws_path = m_ws->filename().GetPath();
    wxString pr_path = wxFileName(m_cbproject->GetFilename()).GetPath();
    size_t lws = ws_path.length();
    wxString path = pr_path.Mid(lws + 1);

    if (path == "")
    {
        path = ".";
    }

    return path;
}

wxString pm_project_cb::relative_parent_path() const
{
    // this is the relative (from workspace) path to the parent of this project folder
    wxFileName rel_path(relative_path());
    rel_path.SetName("");
    wxString path = rel_path.GetFullPath();

    if (path == "")
    {
        path = ".";
    }

    return path;
}

bool pm_project_cb::is_workspace_subdir() const
{
    wxString ws_path = m_ws->filename().GetPath();
    wxString pr_path = wxFileName(m_cbproject->GetFilename()).GetPath();

    if (pr_path.Find(ws_path) == 0)
    {
        return true;
    }

    return false;
}

wxFileName pm_project_cb::filename() const
{
    return  m_cbproject->GetFilename();
}

void pm_project_cb::get_files()
{
    // traverse project files
    int nfiles = m_cbproject->GetFilesCount();
    m_files.clear();
    m_files.reserve(nfiles);

    for (int i = 0; i < nfiles; i++)
    {
        ProjectFile * file = m_cbproject->GetFile(i);

        // filter out some files
        if (m_regx->regex_match(file->relativeFilename))
        {
            m_files.push_back(std::make_shared<pm_file_cb>(file));
        }
    }
}

void pm_project_cb::get_configs()
{
    int nconfig = m_cbproject->GetBuildTargetsCount();

    for (int i = 0; i < nconfig; i++)
    {
        ProjectBuildTarget * cbtarget = m_cbproject->GetBuildTarget(i);
        // obtain the C::B build target output filename
        wxString lib_name     = wxFileName(cbtarget->GetOutputFilename()).GetName();
        wxString project_name = filename().GetName();
        // insert project name as alias for the target output filename
        m_ws->defaults()->insert_alias(lib_name, project_name);

        // if lib_name is prefixed, also insert without prefix
        if (lib_name.Left(3) == "lib")
        {
            lib_name = lib_name.Mid(3);
        }

        m_ws->defaults()->insert_alias(lib_name, project_name);
        m_configs.push_back(std::make_shared<pm_config_cb>(cbtarget, m_ws->defaults()));
    }
}

void pm_project_cb::get_defines()
{
    const wxArrayString & opts = m_cbproject->GetCompilerOptions();

    for (size_t i = 0; i < opts.GetCount(); i++)
    {
        wxString str = opts[i];
        wxString sub = str.SubString(0, 1);

        if (sub == "-D" || sub == "/D")
        {
            wxString def = str.Mid(2);
            m_settings->push_back("defines", def);
        }
    }
}

pm_project_vec pm_project_cb::dependencies() const
{
    return m_ws->dependencies(m_cbproject);
}

void pm_project_cb::resolve_includes()
{
    pm_project_vec deps = dependencies();

    for (auto d : deps)
    {
        // if includes look like "dep_header.h"
        m_settings->push_back("includedirs", d->relative_path());
        // if includes look like "dep/dep_header.h"
        m_settings->push_back("includedirs", d->relative_parent_path());
        m_settings->push_back("dependson", d->name());
    }
}

