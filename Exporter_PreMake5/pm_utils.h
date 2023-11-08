#ifndef PM_UTILS_H
#define PM_UTILS_H

#include <vector>
#include <set>
#include <memory>
#include <wx/string.h>
class pm_project;
class pm_file;
class pm_config;

using pm_project_vec      = std::vector<std::shared_ptr<pm_project>>;
using pm_project_iterator = pm_project_vec::iterator;

using pm_file_vec         = std::vector<std::shared_ptr<pm_file>>;
using pm_file_iterator    = pm_file_vec::iterator;

using pm_config_vec       = std::vector<std::shared_ptr<pm_config>>;
using pm_config_iterator  = pm_config_vec::iterator;

using string_vec          = std::vector<wxString>;
using string_set          = std::set<wxString>;

#endif // PM_UTILS_H
