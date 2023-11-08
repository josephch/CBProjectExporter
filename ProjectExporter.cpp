// System include files

// CB include files
#include <sdk.h> // Code::Blocks SDK
#include "manager.h"
#include <logmanager.h>
#include <configurationpanel.h>

// ProjectExporter include files
#include "ProjectExporter.h"
#include "Exporter_AutoTools/AutotoolsExporter.h"
#include "Exporter_BakeFile/BakefileExporter.h"
#include "Exporter_CMake/CMakeListsExporter.h"
#include "Exporter_PreMake4/Premake4Dlg.h"
#include "Exporter_PreMake5/premake5cb.h"

// Register the plugin with Code::Blocks.
// We are using an anonymous namespace so we don't litter the global one.
namespace
{
PluginRegistrant<ProjectExporter> reg("ProjectExporter");
}

int ID_Menu_ExportProject = wxNewId();
int ID_Menu_AutotoolsExport = wxNewId();
int ID_Menu_BakefileExport = wxNewId();
int ID_Menu_Premake4Export = wxNewId();
int ID_Menu_Premake5Export = wxNewId();
int ID_Menu_CMakeExport = wxNewId();

// events handling
BEGIN_EVENT_TABLE(ProjectExporter, cbPlugin)
    // add any events you want to handle here
END_EVENT_TABLE()

// constructor
ProjectExporter::ProjectExporter()
{
    // Make sure our resources are available.
    // In the generated boilerplate code we have no resources but when
    // we add some, it will be nice that this code is in place already ;)
    if (!Manager::LoadResource("ProjectExporter.zip"))
    {
        NotifyMissingFile("ProjectExporter.zip");
    }

    pm5ExportClass = new premake5cb();
}

// destructor
ProjectExporter::~ProjectExporter()
{
    delete pm5ExportClass;
}

void ProjectExporter::OnAttach()
{
    // do whatever initialization you need for your plugin
    // NOTE: after this function, the inherited member variable
    // m_IsAttached will be TRUE...
    // You should check for it in other functions, because if it
    // is FALSE, it means that the application did *not* "load"
    // (see: does not need) this plugin...
    Manager::Get()->RegisterEventSink(cbEVT_PROJECT_CLOSE,      new cbEventFunctor<ProjectExporter, CodeBlocksEvent>(this, &ProjectExporter::OnProjectClose));
    Manager::Get()->RegisterEventSink(cbEVT_PROJECT_OPEN,       new cbEventFunctor<ProjectExporter, CodeBlocksEvent>(this, &ProjectExporter::OnProjectOpen));
    Manager::Get()->RegisterEventSink(cbEVT_PROJECT_ACTIVATE,   new cbEventFunctor<ProjectExporter, CodeBlocksEvent>(this, &ProjectExporter::OnProjectActivate));
    Manager::Get()->RegisterEventSink(cbEVT_APP_STARTUP_DONE,   new cbEventFunctor<ProjectExporter, CodeBlocksEvent>(this, &ProjectExporter::OnStartupDone));
    Manager::Get()->RegisterEventSink(cbEVT_MENUBAR_CREATE_END, new cbEventFunctor<ProjectExporter, CodeBlocksEvent>(this, &ProjectExporter::OnMenuCreateEnd));
}

void ProjectExporter::OnRelease(bool appShutDown)
{
    // do de-initialization for your plugin
    // if appShutDown is true, the plugin is unloaded because Code::Blocks is being shut down,
    // which means you must not use any of the SDK Managers
    // NOTE: after this function, the inherited member variable
    // m_IsAttached will be FALSE...
}

int ProjectExporter::Execute()
{
    return pm5ExportClass->Execute();
}

void ProjectExporter::BuildMenu(wxMenuBar * menuBar)
{
    //The application is offering its menubar for your plugin,
    //to add any menu items you want...
    //Append any items you need in the menu...
    //NOTE: Be careful in here... The application's menubar is at your disposal.
}

//Delayed menu build (change this if projects importer recognizes this menu item)
void ProjectExporter::OnStartupDone(CodeBlocksEvent & event)
{
    wxMenu * submenu = Manager::Get()->GetAppFrame()->GetMenuBar()->GetMenu(0);

    if (submenu->FindItem(_("E&xport project")) != wxNOT_FOUND)
    {
        return;
    }

    wxMenu * SubmenuExportProject = new wxMenu;
    wxMenuItem * MenuItemExportAutotools = new wxMenuItem(SubmenuExportProject, ID_Menu_AutotoolsExport, _("&Autotools build system"),  _("Set up an Autotools build system for the active project"));
    wxMenuItem * MenuItemExportBakefile = new wxMenuItem(SubmenuExportProject,  ID_Menu_BakefileExport,  _("&Bakefile"),                _("Export active project as a Bakefile"));
    wxMenuItem * MenuItemExportPremake4 = new wxMenuItem(SubmenuExportProject,  ID_Menu_Premake4Export,  _("&Premake4 script..."),      _("Export active project as a Premake4 script"));
    wxMenuItem * MenuItemExportPremake5 = new wxMenuItem(SubmenuExportProject,  ID_Menu_Premake5Export,  _("Premake&5 script..."),      _("Export active project as a Premake5 script"));
    wxMenuItem * MenuItemExportCMake    = new wxMenuItem(SubmenuExportProject,  ID_Menu_CMakeExport,     _("C&MakeLists.txt (WIP)..."), _("Export active project to a simple CMakeLists.txt file"));
    SubmenuExportProject->Append(MenuItemExportAutotools);
    SubmenuExportProject->Append(MenuItemExportBakefile);
    SubmenuExportProject->Append(MenuItemExportPremake4);
    SubmenuExportProject->Append(MenuItemExportPremake5);
    SubmenuExportProject->Append(MenuItemExportCMake);
    int idx = submenu->GetMenuItems().IndexOf(submenu->FindItem(submenu->FindItem(_("&Import project"))));

    if (idx == wxNOT_FOUND)
    {
        idx = submenu->GetMenuItems().IndexOf(submenu->FindItem(submenu->FindItem(_("R&ecent files"))));

        if (idx++ == wxNOT_FOUND)
        {
            idx = 7;
        }

        submenu->Insert(++idx, ID_Menu_ExportProject, _("E&xport project"), SubmenuExportProject);
        //submenu->(++idx, SubmenuExportProject);
        submenu->InsertSeparator(++idx);
    }
    else
    {
        submenu->Insert(++idx, ID_Menu_ExportProject, _("E&xport project"), SubmenuExportProject);
        //submenu->Insert(++idx, SubmenuExportProject);
    }

    MenuItemExportAutotools->Enable(IsProjectOpen());
    MenuItemExportBakefile->Enable(IsProjectOpen());
    MenuItemExportPremake4->Enable(IsProjectOpen());
    MenuItemExportPremake5->Enable(IsProjectOpen());
    MenuItemExportCMake->Enable(IsProjectOpen());
    Connect(ID_Menu_AutotoolsExport, wxEVT_MENU, (wxObjectEventFunction)&ProjectExporter::RunExportAutotools);
    Connect(ID_Menu_BakefileExport,  wxEVT_MENU, (wxObjectEventFunction)&ProjectExporter::RunExportBakefile);
    Connect(ID_Menu_Premake4Export,  wxEVT_MENU, (wxObjectEventFunction)&ProjectExporter::RunExportPremake4);
    Connect(ID_Menu_Premake5Export,  wxEVT_MENU, (wxObjectEventFunction)&ProjectExporter::RunExportPremake5);
    Connect(ID_Menu_CMakeExport,     wxEVT_MENU, (wxObjectEventFunction)&ProjectExporter::RunExportCMake);
}

void ProjectExporter::RunExportAutotools(wxCommandEvent & event)
{
    AutotoolsExporter * exportObjectTmp = new AutotoolsExporter();
    exportObjectTmp->RunExport();
    Manager::Get()->GetLogManager()->Log(_("Autotools build system created"));
}

void ProjectExporter::RunExportBakefile(wxCommandEvent & event)
{
    BakefileExporter * exportObjectTmp = new BakefileExporter();
    exportObjectTmp->RunExport();
    Manager::Get()->GetLogManager()->Log(_("Bakefile exported"));
}

void ProjectExporter::RunExportPremake4(wxCommandEvent & event)
{
    Premake4Dlg SettingsDialog(Manager::Get()->GetAppWindow());

    if (SettingsDialog.ShowModal() == wxID_OK)
    {
        Manager::Get()->GetLogManager()->Log(_("Premake4 script exported"));
    }
    else
    {
        Manager::Get()->GetLogManager()->Log(_("Premake4 export canceled"));
    }
}

void ProjectExporter::RunExportPremake5(wxCommandEvent & event)
{
    Manager::Get()->GetLogManager()->Log(_("Premake4 export started"));
    pm5ExportClass->OnFileExport(event);
    Manager::Get()->GetLogManager()->Log(_("Premake4 export completed"));
}

void ProjectExporter::RunExportCMake(wxCommandEvent & event)
{
    Manager::Get()->GetLogManager()->Log(_("RunExportCMake started"));
    CMakeListsExporter * exportObjectTmp = new CMakeListsExporter();
    exportObjectTmp->RunExport();
    Manager::Get()->GetLogManager()->Log(_("RunExportCMake completed"));
}

bool ProjectExporter::IsProjectOpen() const
{
    const cbProject * project = Manager::Get()->GetProjectManager()->GetActiveProject();

    if (!project)
    {
        return false;
    }

    return true;
}

void ProjectExporter::OnProjectClose(CodeBlocksEvent & event)
{
    wxMenu * submenu = Manager::Get()->GetAppFrame()->GetMenuBar()->GetMenu(0);

    if (submenu->FindItem(_("E&xport project")) == wxNOT_FOUND)
    {
        OnStartupDone(event);
    }

    submenu->FindItem(ID_Menu_AutotoolsExport)->Enable(IsProjectOpen());
    submenu->FindItem(ID_Menu_BakefileExport)->Enable(IsProjectOpen());
    submenu->FindItem(ID_Menu_Premake4Export)->Enable(IsProjectOpen());
    submenu->FindItem(ID_Menu_Premake5Export)->Enable(IsProjectOpen());
    submenu->FindItem(ID_Menu_CMakeExport)->Enable(IsProjectOpen());
}

void ProjectExporter::OnProjectOpen(CodeBlocksEvent & event)
{
    wxMenu * submenu = Manager::Get()->GetAppFrame()->GetMenuBar()->GetMenu(0);

    if (submenu->FindItem(_("E&xport project")) == wxNOT_FOUND)
    {
        OnStartupDone(event);
    }

    submenu->FindItem(ID_Menu_AutotoolsExport)->Enable(IsProjectOpen());
    submenu->FindItem(ID_Menu_BakefileExport)->Enable(IsProjectOpen());
    submenu->FindItem(ID_Menu_Premake4Export)->Enable(IsProjectOpen());
    submenu->FindItem(ID_Menu_Premake5Export)->Enable(IsProjectOpen());
    submenu->FindItem(ID_Menu_CMakeExport)->Enable(IsProjectOpen());
}

void ProjectExporter::OnProjectActivate(CodeBlocksEvent & event)
{
    wxMenu * submenu = Manager::Get()->GetAppFrame()->GetMenuBar()->GetMenu(0);

    if (submenu->FindItem(_("E&xport project")) == wxNOT_FOUND)
    {
        OnStartupDone(event);
    }

    submenu->FindItem(ID_Menu_AutotoolsExport)->Enable(IsProjectOpen());
    submenu->FindItem(ID_Menu_BakefileExport)->Enable(IsProjectOpen());
    submenu->FindItem(ID_Menu_Premake4Export)->Enable(IsProjectOpen());
    submenu->FindItem(ID_Menu_Premake5Export)->Enable(IsProjectOpen());
    submenu->FindItem(ID_Menu_CMakeExport)->Enable(IsProjectOpen());
}

void ProjectExporter::OnMenuCreateEnd(CodeBlocksEvent & event)
{
    wxMenu * submenu = Manager::Get()->GetAppFrame()->GetMenuBar()->GetMenu(0);

    if (submenu->FindItem(_("E&xport project")) == wxNOT_FOUND)
    {
        OnStartupDone(event);
    }
}
