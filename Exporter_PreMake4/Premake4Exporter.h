#ifndef PREMAKE_EXPORTER_H
#define PREMAKE_EXPORTER_H

#include "ExporterBase.h"


class PremakeExporter : public ExporterBase
{
    public:
        /** Default constructor */
        PremakeExporter();
        /** Default destructor */
        virtual ~PremakeExporter();
        void RunExport(bool EvaluateVars, bool UpgrTargs);
    protected:
    private:
        /** @brief Emit Premake4 flags given a wxArrayString of compiler flags and
          * the ID of the compiler.
          *
          * @param compilerID const wxString& The ID of the compiler.
          * @param compilerFlags const wxArrayString& An array of compiler flags.
          * @return wxString A quoted, comma-separated list of Premake4 flags.
          *
          */
        wxString EmitFlags(const wxString& compilerID, const wxArrayString& compilerFlags);

        /** @brief Emit a quoted, comma-separated list of defines. */
        wxString EmitDefines(const wxArrayString& compilerFlags);

        /** @brief Add escape sequences to quotes and backslashes. */
        wxString AddEscapes(const wxString& source);

        /** @brief Replace Code::Blocks variables if @c repl is true. */
        wxString ReplVars(const wxString& source, bool repl);

        /** @brief Export project as-is. */
        void ExportStraight(bool EvaluateVars);

        /** @brief Upgrade targets to projects (work-around for Premake's
          * lack of support for per-target files).
          */
        void ExportUpgrade(bool EvaluateVars);

        wxString m_content;
};

#endif // PREMAKE_EXPORTER_H
