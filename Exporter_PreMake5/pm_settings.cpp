#include "pm_settings.h"
#include <logmanager.h>

pm_settings::pm_settings()
{}

pm_settings::~pm_settings()
{}

void pm_settings::assign(const wxString & key, const string_vec & vec)
{
    auto & s = m_settings[key];

    for (auto v : vec)
    {
        s.insert(v);
    }
}

string_set pm_settings::values(const wxString & key)
{
    auto it = m_settings.find(key);

    if (it != m_settings.end())
    {
        return it->second;
    }

    return string_set();
}

void pm_settings::premake_export(size_t tabs, std::ostream & out)
{
    // traverse all contained settings
    for (auto & p : *this)
    {
        const wxString & key    = p.first;
        const string_set & vals = p.second;
        // looks like "()" is for single values and "{}" is for multiple
        char left = '(';
        char right = ')';

        if (vals.size() > 1)
        {
            left = '{';
            right = '}';
        }

        if (key == "links")
        {
            for (size_t i = 0; i < tabs; i++)
            {
                out << '\t';
            }

            out << "-- When linking within workspace, 'links' refer to project name." << std::endl;
        }

        for (size_t i = 0; i < tabs; i++)
        {
            out << '\t';
        }

        out << key << " " << left << " ";
        size_t i = 0;

        for (auto v : vals)
        {
            if (i > 0)
            {
                out << ',';
            }

            out << "\"" << v << "\"";
            i++;
        }

        out << " " << right << " " << std::endl;
    }
}
