#ifndef PM_FILE_H
#define PM_FILE_H

#include "pm_base.h"
#include <wx/filename.h>

// project (source) file
class pm_file : public pm_base
{
    public:
        pm_file();
        virtual ~pm_file();

        // return full filename of the source file
        virtual wxFileName filename() const = 0;

        // return filename relative to the project
        virtual wxFileName relative_filename() const = 0;

        // export to premake5
        virtual void premake_export(std::ostream & out);
};

#endif // PM_FILE_H
