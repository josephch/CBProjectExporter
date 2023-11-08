#include "pm_config.h"
#include "pm_settings.h"

pm_config::pm_config()
{}

pm_config::~pm_config()
{}


void pm_config::premake_export(std::ostream & out)
{
    out << std::endl << "\t\tfilter { \"" << name() << "\" }" << std::endl;
    settings()->premake_export(3, out);
    out << "\t\tfilter { }"
        << std::endl;
}
