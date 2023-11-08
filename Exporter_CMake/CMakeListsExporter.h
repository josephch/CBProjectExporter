#ifndef __CMAKELISTSEXPORTER_H__
#define __CMAKELISTSEXPORTER_H__

// System include files
#include <wx/regex.h>
#include <wx/textfile.h>

// ProjectExporter include files
#include "../ExporterBase.h"

class CMakeListsExporter : public ExporterBase
{
    public:
        /** Default constructor */
        CMakeListsExporter();
        /** Default destructor */
        virtual ~CMakeListsExporter();
        void RunExport();

    protected:

    private:
        enum ConvertMacro_DirSeperator
        {
            NoConversion,
            WindowsToLinux,
            WindowsExpand,
            WindowsExpandKeepTrailing
        };

        enum ExportMode
        {
            GVS_EXPORT_ALL	= 0x00,
            GVS_EXPORT_DEFAULT_ONLY,
            GVS_EXPORT_NON_DEFAULT
        };

        wxString GetTargetRootDirectory(ProjectBuildTarget * buildTarget);
        void ExpandMacros(wxString & buffer);
        void ConvertMacros(wxString & buffer, ConvertMacro_DirSeperator eConvertDirSeperator);
        void ExportGlobalVariableSets(ExportMode eMode);
        wxString ExportMacros(ProjectBuildTarget * buildTarget);
        void ExportGlobalVariables();
        void ExportBuildTarget(cbProject * project, ProjectBuildTarget * buildTarget);

        wxString ValidateFilename(const wxString & iFileName);
        wxString GetHumanReadableOptionRelation(ProjectBuildTarget * buildTarget, OptionsRelationType type);

        LogManager * m_LogMgr;
        ConvertMacro_DirSeperator m_eMacroConvertDirectorySeperator;
        const wxChar * EOL = wxTextFile::GetEOL();
        wxString    m_CBProjectRootDir;
        wxString    m_ContentCMakeListTarget;
        wxString    m_ContentCMakeListGlobalVariables;
        wxString    m_sGlobalVariableFileName;
        wxString    m_ContentCMakeListTopLevel;
        wxRegEx     m_RE_Unix;
        wxRegEx     m_RE_DOS;
};

#endif // __CMAKELISTSEXPORTER_H__
