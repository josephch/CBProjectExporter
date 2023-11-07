#include <sdk.h> // Code::Blocks SDK
#include <configurationpanel.h>
#include "ProjectExporter.h"
#include "AutotoolsExporter.h"
#include "BakefileExporter.h"
#include "PremakeDlg.h"

// Register the plugin with Code::Blocks.
// We are using an anonymous namespace so we don't litter the global one.
namespace
{
    PluginRegistrant<ProjectExporter> reg(_T("ProjectExporter"));
}

int ID_Menu_ExportProject=wxNewId();
int ID_Menu_AutotoolsExport=wxNewId();
int ID_Menu_BakefileExport=wxNewId();
int ID_Menu_PremakeExport=wxNewId();


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
    if(!Manager::LoadResource(_T("ProjectExporter.zip")))
    {
        NotifyMissingFile(_T("ProjectExporter.zip"));
    }
}

// destructor
ProjectExporter::~ProjectExporter()
{
}

void ProjectExporter::OnAttach()
{
    // do whatever initialization you need for your plugin
    // NOTE: after this function, the inherited member variable
    // m_IsAttached will be TRUE...
    // You should check for it in other functions, because if it
    // is FALSE, it means that the application did *not* "load"
    // (see: does not need) this plugin...
    Manager::Get()->RegisterEventSink(cbEVT_PROJECT_CLOSE, new cbEventFunctor<ProjectExporter, CodeBlocksEvent>(this, &ProjectExporter::OnProjectClose));
    Manager::Get()->RegisterEventSink(cbEVT_PROJECT_OPEN, new cbEventFunctor<ProjectExporter, CodeBlocksEvent>(this, &ProjectExporter::OnProjectOpen));
    Manager::Get()->RegisterEventSink(cbEVT_PROJECT_ACTIVATE, new cbEventFunctor<ProjectExporter, CodeBlocksEvent>(this, &ProjectExporter::OnProjectActivate));
    Manager::Get()->RegisterEventSink(cbEVT_APP_STARTUP_DONE, new cbEventFunctor<ProjectExporter, CodeBlocksEvent>(this, &ProjectExporter::OnStartupDone));
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

void ProjectExporter::BuildMenu(wxMenuBar* menuBar)
{
    //The application is offering its menubar for your plugin,
    //to add any menu items you want...
    //Append any items you need in the menu...
    //NOTE: Be careful in here... The application's menubar is at your disposal.
}

//Delayed menu build (change this if projects importer recognizes this menu item)
void ProjectExporter::OnStartupDone(CodeBlocksEvent& event)
{
    wxMenu *submenu = Manager::Get()->GetAppFrame()->GetMenuBar()->GetMenu(0);
    if(submenu->FindItem(_T("E&xport project")) != wxNOT_FOUND)
    {
        return;
    }
    wxMenu *SubmenuExportProject = new wxMenu;//wxMenuItem(submenu, ID_Menu_ExportProject, _("E&xport project..."), _("Export active project as Premake script."));
    wxMenuItem *MenuItemExportAutotools = new wxMenuItem(SubmenuExportProject, ID_Menu_AutotoolsExport, wxT("&Autotools build system"), wxT("Set up an Autotools build system for the active project"));
    wxMenuItem *MenuItemExportBakefile = new wxMenuItem(SubmenuExportProject, ID_Menu_BakefileExport, wxT("&Bakefile"), wxT("Export active project as a Bakefile"));
    wxMenuItem *MenuItemExportPremake = new wxMenuItem(SubmenuExportProject, ID_Menu_PremakeExport, wxT("&Premake script..."), wxT("Export active project as a Premake4 script"));
    SubmenuExportProject->Append(MenuItemExportAutotools);
    SubmenuExportProject->Append(MenuItemExportBakefile);
    SubmenuExportProject->Append(MenuItemExportPremake);
    int idx = submenu->GetMenuItems().IndexOf(submenu->FindItem(submenu->FindItem(_T("&Import project"))));
    if(idx == wxNOT_FOUND)
    {
        idx = submenu->GetMenuItems().IndexOf(submenu->FindItem(submenu->FindItem(_T("R&ecent files"))));
        if(idx++ == wxNOT_FOUND)
        {
            idx = 7;
        }
        submenu->Insert(++idx, ID_Menu_ExportProject, wxT("E&xport project"), SubmenuExportProject);
        //submenu->(++idx, SubmenuExportProject);
        submenu->InsertSeparator(++idx);
    }
    else
    {
        submenu->Insert(++idx, ID_Menu_ExportProject, wxT("E&xport project"), SubmenuExportProject);
        //submenu->Insert(++idx, SubmenuExportProject);
    }
    MenuItemExportAutotools->Enable(IsProjectOpen());
    MenuItemExportBakefile->Enable(IsProjectOpen());
    MenuItemExportPremake->Enable(IsProjectOpen());
    Connect(ID_Menu_AutotoolsExport, wxEVT_COMMAND_TOOL_CLICKED, (wxObjectEventFunction)&ProjectExporter::RunExportAutotools);
    Connect(ID_Menu_BakefileExport, wxEVT_COMMAND_TOOL_CLICKED, (wxObjectEventFunction)&ProjectExporter::RunExportBakefile);
    Connect(ID_Menu_PremakeExport, wxEVT_COMMAND_TOOL_CLICKED, (wxObjectEventFunction)&ProjectExporter::RunExportPremake);
}

void ProjectExporter::RunExportAutotools()
{
    AutotoolsExporter *exportObjectTmp = new AutotoolsExporter();
    exportObjectTmp->RunExport();
    Manager::Get()->GetLogManager()->Log(wxT("Autotools build system created"));
}

void ProjectExporter::RunExportBakefile()
{
    BakefileExporter *exportObjectTmp = new BakefileExporter();
    exportObjectTmp->RunExport();
    Manager::Get()->GetLogManager()->Log(wxT("Bakefile exported"));
}

void ProjectExporter::RunExportPremake()
{
    //Manager::Get()->GetLogManager()->Log(wxT("Premake script exported");
    PremakeDlg SettingsDialog(Manager::Get()->GetAppWindow());
    SettingsDialog.ShowModal();
}

bool ProjectExporter::IsProjectOpen() const
{
    const cbProject* project = Manager::Get()->GetProjectManager()->GetActiveProject();
    if(!project)
    {
        return false;
    }
    return true;
}

void ProjectExporter::OnProjectClose(CodeBlocksEvent& event)
{
    wxMenu *submenu = Manager::Get()->GetAppFrame()->GetMenuBar()->GetMenu(0);
    if(submenu->FindItem(_T("E&xport project")) == wxNOT_FOUND)
    {
        OnStartupDone(event);
    }
    submenu->FindItem(ID_Menu_AutotoolsExport)->Enable(IsProjectOpen());
    submenu->FindItem(ID_Menu_BakefileExport)->Enable(IsProjectOpen());
    submenu->FindItem(ID_Menu_PremakeExport)->Enable(IsProjectOpen());
}

void ProjectExporter::OnProjectOpen(CodeBlocksEvent& event)
{
    wxMenu *submenu = Manager::Get()->GetAppFrame()->GetMenuBar()->GetMenu(0);
    if(submenu->FindItem(_T("E&xport project")) == wxNOT_FOUND)
    {
        OnStartupDone(event);
    }
    submenu->FindItem(ID_Menu_AutotoolsExport)->Enable(IsProjectOpen());
    submenu->FindItem(ID_Menu_BakefileExport)->Enable(IsProjectOpen());
    submenu->FindItem(ID_Menu_PremakeExport)->Enable(IsProjectOpen());
    //submenu->FindItem(_T("E&xport project")->Enable(ID_Menu_BakefileExport, IsProjectOpen());
    //submenu->FindItem(_T("E&xport project")->Enable(ID_Menu_PremakeExport, IsProjectOpen());
}

void ProjectExporter::OnProjectActivate(CodeBlocksEvent& event)
{
    wxMenu *submenu = Manager::Get()->GetAppFrame()->GetMenuBar()->GetMenu(0);
    if(submenu->FindItem(_T("E&xport project")) == wxNOT_FOUND)
    {
        OnStartupDone(event);
    }
    submenu->FindItem(ID_Menu_AutotoolsExport)->Enable(IsProjectOpen());
    submenu->FindItem(ID_Menu_BakefileExport)->Enable(IsProjectOpen());
    submenu->FindItem(ID_Menu_PremakeExport)->Enable(IsProjectOpen());
}

void ProjectExporter::OnMenuCreateEnd(CodeBlocksEvent& event)
{
    wxMenu *submenu = Manager::Get()->GetAppFrame()->GetMenuBar()->GetMenu(0);
    if(submenu->FindItem(_T("E&xport project")) == wxNOT_FOUND)
    {
        OnStartupDone(event);
    }
}
