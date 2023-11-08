#include "pm_file_cb.h"
#include <projectfile.h>

pm_file_cb::pm_file_cb(ProjectFile * cbfile)
    : m_cbfile(cbfile)
{}

pm_file_cb::~pm_file_cb()
{}

wxFileName pm_file_cb::filename() const
{
    return m_cbfile->file;
}

wxFileName pm_file_cb::relative_filename() const
{
    return m_cbfile->relativeFilename;
}
