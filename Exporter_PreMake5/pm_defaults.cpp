#include "pm_defaults.h"
#include <wx/tokenzr.h>
#include <vector>
#include "pm_settings.h"

// for debugging
// #include <manager.h>
// #include <logmanager.h>


static std::vector<wxString> tokenize(const wxString & input,  const wxString & delimiters)
{
    std::vector<wxString> tokens;
    wxStringTokenizer tokenizer(input, delimiters);

    while (tokenizer.HasMoreTokens())
    {
        tokens.push_back(tokenizer.GetNextToken());
    }

    return tokens;
}

pm_defaults::pm_defaults(ConfigManager * cfgmgr)
    : m_cfgmgr(cfgmgr)
{
    factory_settings();
    FromConfigManager();
}

pm_defaults::~pm_defaults()
{}

void pm_defaults::factory_settings()
{
    //   Manager::Get()->GetLogManager()->Log("premake5cb: factory_settings()");
    m_settings_map["FileMasks"] =
    {
        "*.cpp"
        , "*.h"
        , "*.c"
    };
    m_bool_map["export_on_build"]        = true;
    m_bool_map["save_all_on_export"]     = false;
    m_bool_map["post_build_copy"]        = false;
    m_bool_map["use_workspace_prefix"]   = true;
    m_bool_map["use_workspace_defaults"] = true;
    m_bool_map["use_project_defaults"]   = true;
    m_bool_map["export_cb_targetname"]   = true;
    m_bool_map["ExportConsoleApp"]   = true;
    m_bool_map["ExportSharedLib"]    = true;
    m_bool_map["ExportStaticLib"]    = true;
    m_bool_map["ExportWindowedApp"]  = false;
    m_settings_map["workspace_defaults"] =
    {
    };
    m_settings_map["project_defaults"] =
    {
        "language (\"c++\")"
        , "cppdialect (\"c++17\")"
        , "architecture (\"x86_64\")"
        , "exceptionhandling (\"on\")"
        , "pic (\"on\")"
        , "rtti (\"on\")"
        , "staticruntime (\"off\")"
    };
    m_settings_map["configurations_release"] =
    {
        "defines (\"NDEBUG\")"
        , "optimize (\"on\")"
    };
    m_settings_map["configurations_debug"] =
    {
        "defines (\"DEBUG\")"
        , "symbols (\"on\")"
    };
    m_alias_map.clear();
}

void pm_defaults::FromConfigManager()
{
    for (auto & p : m_settings_map)
    {
        const wxString & key  = p.first;

        // only if the key exist in the config manager, we override the factory settings with those values
        if (m_cfgmgr->Exists(key))
        {
            std::vector<wxString> & vals = p.second;
            vals.clear();
            wxArrayString arr    = m_cfgmgr->ReadArrayString(key);
            // convert to vector
            size_t nval = arr.Count();

            for (size_t i = 0; i < nval; i++)
            {
                vals.push_back(arr[i]);
            }
        }
    }

    for (auto & p : m_bool_map)
    {
        const wxString & key = p.first;
        bool      &     val = p.second;
        m_cfgmgr->Read(key, &val);
        //   Manager::Get()->GetLogManager()->Log("premake5cb: Read " + key + " " + std::to_string(val));
    }
}

void pm_defaults::ToConfigManager()
{
    // traverse the settings and save with ConfigManager
    for (auto & p : m_settings_map)
    {
        const wxString & key                = p.first;
        const std::vector<wxString> & vals = p.second;
        // convert to wxArrayString because ConfigManager supports that
        wxArrayString arr;

        for (auto s : vals)
        {
            arr.Add(s);
        }

        m_cfgmgr->Write(key, arr);
    }

    for (auto & p : m_bool_map)
    {
        const wxString & key = p.first;
        bool            val = p.second;
        m_cfgmgr->Write(key, val);
        //  Manager::Get()->GetLogManager()->Log("premake5cb: Write " + key + " " + std::to_string(val));
    }
}

std::shared_ptr<pm_settings> pm_defaults::get_settings(const wxString & category)
{
    std::shared_ptr<pm_settings> settings;
    auto it = m_settings_map.find(category);

    if (it != m_settings_map.end())
    {
        settings = std::make_shared<pm_settings>();
        const std::vector<wxString> & vals = it->second;

        for (auto v : vals)
        {
            std::vector<wxString> tokens = tokenize(v, "(){},");

            if (tokens.size() > 1)
            {
                wxString key = tokens[0];
                tokens.erase(tokens.begin());

                // remove quotes in tokens
                for (auto & t : tokens)
                {
                    t.Replace("\"", "");
                    t.Replace("[", "{"); // this to allow: $[GLOBALVAR]/include   =>  ${GLOBALVAR}/include
                    t.Replace("]", "}");
                    t.Trim(true);  // from right
                    t.Trim(false); // from left
                }

                settings->assign(key, tokens);
            }
        }
    }

    return settings;
}

wxString pm_defaults::get_alias(const wxString & libname) const
{
    auto it = m_alias_map.find(libname);

    if (it != m_alias_map.end())
    {
        return it->second;
    }

    return libname;
}

bool pm_defaults::get_bool_flag(const wxString & key, bool defval) const
{
    auto it = m_bool_map.find(key);

    if (it != m_bool_map.end())
    {
        return it->second;
    }

    return defval;
}

void pm_defaults::put_bool_flag(const wxString & key, bool value)
{
    m_bool_map[key] = value;
}
