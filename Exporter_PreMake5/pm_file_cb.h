#ifndef PM_FILE_CB_H
#define PM_FILE_CB_H

#include "pm_file.h"
class ProjectFile;

// Code::Blocks aware project (=source) file
class pm_file_cb : public pm_file
{
    public:
        pm_file_cb(ProjectFile * cbfile);
        virtual ~pm_file_cb();

        // return full filename of the source file
        virtual wxFileName filename() const;

        // return filename relative to the project
        virtual wxFileName relative_filename() const;

    private:
        ProjectFile * m_cbfile;
};

#endif // PM_FILE_CB_H
