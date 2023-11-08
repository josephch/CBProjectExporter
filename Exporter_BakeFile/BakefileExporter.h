#ifndef BAKEFILE_EXPORTER_H
#define BAKEFILE_EXPORTER_H

#include "../ExporterBase.h"


class BakefileExporter : public ExporterBase
{
    public:
        /** Default constructor */
        BakefileExporter();
        /** Default destructor */
        virtual ~BakefileExporter();
        void RunExport();
    protected:
    private:
        wxArrayString EmitFlags(const wxString & compilerID, const wxArrayString & compilerFlags, wxArrayString & unrecognizedFlags);
        wxString GetOptions(const wxString & source);

        wxArrayString m_options;
};

#endif // BAKEFILE_EXPORTER_H
