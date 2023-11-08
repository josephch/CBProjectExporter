#ifndef PM_REGEX_H
#define PM_REGEX_H

#include <wx/string.h>
#include <string>
#include <vector>
#include <memory>

// helper class for regular expresion file name matching
class pm_regex
{
    public:
        pm_regex();
        virtual ~pm_regex();

        // add a mask to check against
        void push_back(wxString mask);

        // perform check of name against all masks and return true if at least one match
        bool regex_match(const wxString & name);

        // create pm_regex for typical c/c++ project
        static std::shared_ptr<pm_regex> default_cpp();

    private:
        std::vector<std::string> m_masks;
};

#endif // PM_REGEX_H
