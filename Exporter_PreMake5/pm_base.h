#ifndef PM_BASE_H
#define PM_BASE_H

#include <wx/string.h>
#include <memory>
#include <ostream>

// abstract base class for several premake5 classes
class pm_base
{
    public:
        pm_base();
        virtual ~pm_base();

        // export to premake5
        virtual void premake_export(std::ostream & out) = 0;
};

#endif // PM_BASE_H
