#include "pm_config_cb.h"
#include <projectbuildtarget.h>

#include "pm_defaults.h"
#include "pm_settings.h"

pm_config_cb::pm_config_cb(ProjectBuildTarget * cbtarget, std::shared_ptr<pm_defaults> defaults)
    : m_cbtarget(cbtarget)
    , m_defaults(defaults)
    , m_is_debug(false)
{
    m_name     = m_cbtarget->GetTitle();
    m_compiler = m_cbtarget->GetCompilerID();
    get_defines();
}

pm_config_cb::~pm_config_cb()
{}

wxString pm_config_cb::kind()
{
    wxString stype = "Unknown";
    TargetType type = m_cbtarget->GetTargetType();

    switch (type)
    {
        case ttExecutable:
        {
            stype = "WindowedApp";
            break;
        }

        case ttConsoleOnly:
        {
            stype = "ConsoleApp";
            break;
        }

        case ttStaticLib:
        {
            stype = "StaticLib";
            break;
        }

        case ttDynamicLib:
        {
            stype = "SharedLib";
            break;
        }

        case ttCommandsOnly:
        {
            stype = "None";
            break;
        }

        case ttNative:
        {
            stype = "None";
            break;
        }

        default:
        {
            stype = "None";
        }
    };

    return stype;
}

void pm_config_cb::get_defines()
{
    // traverse compiler options and figure out some key info
    const wxArrayString & opts = m_cbtarget->GetCompilerOptions();

    // first determine if this is a debug target
    for (size_t i = 0; i < opts.GetCount(); i++)
    {
        wxString str = opts[i];
        bool MDd   = (str.Find("MDd")   != wxNOT_FOUND);
        bool debug = (str.Find("DEBUG") != wxNOT_FOUND);

        if (MDd || debug)
        {
            m_is_debug = true;
            break;
        }
    }

    // now we can assign the defaults
    if (m_is_debug)
    {
        m_name     = "configurations:debug";
        m_settings =  m_defaults->get_settings("configurations_debug");
    }
    else
    {
        m_name     =  "configurations:release";
        m_settings =  m_defaults->get_settings("configurations_release");
    }

    if (m_defaults->get_bool_flag("export_cb_targetname"))
    {
        wxString targetname = wxFileName(m_cbtarget->GetOutputFilename()).GetName();

        // if targetname is prefixed with "lib", remove that prefix because
        // a) not used with MSVC
        // b) for g++ it will be added by make (we don't want "liblib")
        if (targetname.Left(3) == "lib")
        {
            targetname = targetname.Mid(3);
        }

        m_settings->push_back("targetname", targetname);
    }

    for (size_t i = 0; i < opts.GetCount(); i++)
    {
        wxString str = opts[i];
        wxString sub = str.SubString(0, 2);

        if (sub == "-D" || sub == "/D")
        {
            // this is a #define
            wxString def = sub.erase(0, 2);
            m_settings->push_back("defines", def);
        }
        else
        {
            // set C++ level, e.g. C++17
            int j = str.Find("c++");

            if (j != wxNOT_FOUND)
            {
                m_settings->push_back("cppdialect", str.Mid(j));
            }
        }
    }

    // set "project type" on this level instead of on project level
    m_settings->push_back("kind", kind());
    // add link libraries
    const wxArrayString & links = m_cbtarget->GetLinkLibs();

    for (size_t i = 0; i < links.GetCount(); i++)
    {
        // anything but MSVC libraries
        if (links[i].Find("msvc") == wxNOT_FOUND)
        {
            m_settings->push_back("links", wxFileName(links[i]).GetName());
        }
    }
}

bool pm_config_cb::is_debug() const
{
    return m_is_debug;
}
