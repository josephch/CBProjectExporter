#ifndef pm_settings_H
#define pm_settings_H

#include <ostream>
#include <wx/string.h>
#include <vector>
#include <map>
#include "pm_utils.h"

// helper class to contain settings of various kinds
class pm_settings
{
    public:
        using settings_map = std::map<wxString, string_set>;
        using iterator     = settings_map::iterator;

        pm_settings();
        virtual ~pm_settings();

        // assign a vector of values to a setting
        void assign(const wxString & key, const string_vec & v);

        // return values from a setting, empty if not found
        string_set values(const wxString & key);

        // assign a set of values to a setting
        void assign(const wxString & key, const string_set & s)
        {
            m_settings[key] = s;
        }

        // push a single value to a named setting
        void push_back(const wxString & key, const wxString & value)
        {
            m_settings[key].insert(value);
        }

        // look up a particiular setting
        iterator find(const wxString & key)
        {
            return m_settings.find(key);
        }

        // traverse settings
        iterator begin()
        {
            return m_settings.begin();
        }
        iterator end()
        {
            return m_settings.end();
        }

        // export to premake5
        void premake_export(size_t tabs, std::ostream & out);

    private:
        settings_map m_settings;
};

#endif // pm_settings_H
