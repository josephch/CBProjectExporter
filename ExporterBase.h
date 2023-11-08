#ifndef __EXPORTERBASE_H__
#define __EXPORTERBASE_H__

// System include files
#include <wx/arrstr.h>

// CB include files
#include "cbproject.h"

class ExporterBase
{
    public:
        /** Default constructor */
        ExporterBase();
        /** Default destructor */
        virtual ~ExporterBase();
    protected:
        /** @brief List of included files. */
        wxArrayString IncludeFilesArray();

        wxArrayString AppendOptionsArray(const wxArrayString & proj, const wxArrayString & targ, const OptionsRelation relation = orAppendToParentOptions);

        /** @brief Convert backslashes to forward-slashes. */
        wxString ConvertSlash(const wxString & source);

        /** @brief Replace spaces with underscores. */
        wxString ReplSpace(const wxString & source);

        /** @brief Removes the lib prefix if it exists. */
        wxString RemLib(const wxString & source);

        /** @brief Returns true if @c target string is found in @c source array. */
        bool StringExists(const wxArrayString & source, wxString target);

        cbProject * m_project;
        /** @brief Always clear (or assign) before usage. */
        //wxString m_tmpString;
        /** @brief Always clear (or assign) before usage. */
        //wxFileName m_tmpFileName;
    private:
};

#endif // __EXPORTERBASE_H__
