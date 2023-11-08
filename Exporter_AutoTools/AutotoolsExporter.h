#ifndef AUTOTOOLSEXPORTER_H
#define AUTOTOOLSEXPORTER_H

#include "../ExporterBase.h"


class AutotoolsExporter : public ExporterBase
{
    public:
        /** Default constructor */
        AutotoolsExporter();
        /** Default destructor */
        virtual ~AutotoolsExporter();

        void RunExport();
    protected:
    private:
        /** configure.ac */
        void CreateConfig();
        wxString m_configure;

        /** Makefile.am */
        void CreateMake();
        wxString m_makefile;

        wxArrayString IdentifyLibs();
};

#endif // AUTOTOOLSEXPORTER_H
