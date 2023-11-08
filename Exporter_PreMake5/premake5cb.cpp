#include <sdk.h> // Code::Blocks SDK
#include <logmanager.h>
#include <projectmanager.h>
#include <cbworkspace.h>
#include "cbproject.h"

#include <wx/msgdlg.h>
#include <fstream>
#include <configurationpanel.h>

#include "premake5cb.h"
#include "pm_workspace_cb.h"
#include "PM5SettingsDialog.h"
#include "pm_defaults.h"

const long premake5cb::ID_EXPORT = wxNewId();

//BEGIN_EVENT_TABLE(premake5cb, cbToolPlugin)
//    EVT_MENU(ID_EXPORT, premake5cb::OnFileExport)
//END_EVENT_TABLE()

// Register the plugin with Code::Blocks.
// We are using an anonymous namespace so we don't litter the global one.
//namespace
//{
//PluginRegistrant<premake5cb> reg(_T("premake5cb"));
//}


// constructor
premake5cb::premake5cb()
{
    // Make sure our resources are available.
    // In the generated boilerplate code we have no resources but when
    // we add some, it will be nice that this code is in place already ;)
#if 0
    if (!Manager::LoadResource(_T("premake5cb.zip")))
    {
        NotifyMissingFile(_T("premake5cb.zip"));
    }

#else
    m_defaults = std::make_shared<pm_defaults>(Manager::Get()->GetConfigManager("premake5cb"));
#endif
}

// destructor
premake5cb::~premake5cb()
{
}

void premake5cb::OnAttach()
{
    // do whatever initialization you need for your plugin
    // NOTE: after this function, the inherited member variable
    // m_IsAttached will be TRUE...
    // You should check for it in other functions, because if it
    // is FALSE, it means that the application did *not* "load"
    // (see: does not need) this plugin...
    // Restore persistent settings
    m_defaults = std::make_shared<pm_defaults>(Manager::Get()->GetConfigManager("premake5cb"));

    if (m_defaults->get_bool_flag("export_on_build", false))
    {
        // we save the premake5 file after compile completed
        Manager::Get()->RegisterEventSink(cbEVT_COMPILER_FINISHED, new cbEventFunctor<premake5cb, CodeBlocksEvent>(this, &premake5cb::OnSave));
    }

    // Because this is a cbToolPlugin, Code::Blocks will never call BuildMenu
    // so we do it ourselves...
    BuildMenu(Manager::Get()->GetAppFrame()->GetMenuBar());
}

void premake5cb::OnRelease(bool appShutDown)
{
    // do de-initialization for your plugin
    // if appShutDown is true, the plugin is unloaded because Code::Blocks is being shut down,
    // which means you must not use any of the SDK Managers
    // NOTE: after this function, the inherited member variable
    // m_IsAttached will be FALSE...
}

int premake5cb::Execute()
{
    PM5SettingsDialog dlg(Manager::Get()->GetAppWindow());

    if (!m_defaults)
    {
        m_defaults = std::make_shared<pm_defaults>(Manager::Get()->GetConfigManager("premake5cb"));
    }

    // apply settings in GUI
    dlg.ToDialog(m_defaults);

    if (dlg.ShowModal() == wxID_OK)
    {
        // OK was pressed
        dlg.FromDialog(m_defaults);
        m_defaults->ToConfigManager();
    }

    return 0;
}

void premake5cb::BuildMenu(wxMenuBar * menuBar)
{
    int pos          = menuBar->FindMenu("File");
    wxMenu * fileMenu = menuBar->GetMenu(pos);
    int index = 0;
    wxMenuItemList & items = fileMenu->GetMenuItems();

    for (wxMenuItem * item : items)
    {
        wxString label = item->GetItemLabelText();
        index++;

        if (label == "Export")
        {
            wxMenu * ExportMenu = item->GetSubMenu();

            if (ExportMenu)
            {
                ExportMenu->AppendSeparator();
                ExportMenu->Append(ID_EXPORT, "Premake5 workspace...");
            }

            break;
        }
    }
}

void premake5cb::OnSave(CodeBlocksEvent & event)
{
    bool use_workspace_prefix = m_defaults->get_bool_flag("use_workspace_prefix", true);
    wxFileName fname_lua = Manager::Get()->GetProjectManager()->GetWorkspace()->GetFilename();

    if (use_workspace_prefix)
    {
        fname_lua.SetName(fname_lua.GetName() + "_premake5");
    }
    else
    {
        fname_lua.SetName("premake5");
    }

    fname_lua.SetExt("lua");
    DoExport(fname_lua);
}

void premake5cb::OnFileExport(wxCommandEvent & event)
{
    cbWorkspace * pWorkspace = Manager::Get()->GetProjectManager()->GetWorkspace();

    if (pWorkspace)
    {
        bool use_workspace_prefix = m_defaults->get_bool_flag("use_workspace_prefix", true);
        wxFileName fname_lua(pWorkspace->GetFilename());

        if (!pWorkspace->IsOK())
        {
            cbProject * project = Manager::Get()->GetProjectManager()->GetProjects()->Item(0);

            if (project)
            {
                wxFileName wxfProjectFileName(project->GetFilename());
                fname_lua.Assign(wxfProjectFileName);
            }
        }

        if (use_workspace_prefix)
        {
            fname_lua.SetName(fname_lua.GetName() + "_premake5");
        }
        else
        {
            fname_lua.SetName("premake5");
        }

        fname_lua.SetExt("lua");
        wxWindow * parent = Manager::Get()->GetAppWindow();
        wxFileDialog dlg(parent, "Save workspace as premake5 script", fname_lua.GetPath(), fname_lua.GetName(), wxT("Premake5 script (*.lua)|*.lua"), wxFD_SAVE | wxFD_OVERWRITE_PROMPT);

        if (dlg.ShowModal() == wxID_OK)
        {
            fname_lua = dlg.GetPath();
            DoExport(fname_lua);
        }
    }
}

void premake5cb::DoExport(const wxFileName & fname_lua)
{
    //    if (m_IsAttached)
    //    {
    if (m_defaults->get_bool_flag("save_all_on_export", false))
    {
        Manager::Get()->GetProjectManager()->SaveAllProjects();
        Manager::Get()->GetProjectManager()->SaveWorkspace();
    }

    auto ws = std::make_shared<pm_workspace_cb>(m_defaults);

    if (ws->size() == 0)
    {
        Manager::Get()->GetLogManager()->LogError("premake5cb: workspace is empty " + ws->filename().GetFullPath());
        return;
    }

    if (ws->is_local_workspace())
    {
        std::string fname = fname_lua.GetFullPath().ToStdString();
        std::ofstream out(fname);

        if (out.is_open())
        {
            // write the premake5 file
            ws->premake_export(out);
            Manager::Get()->GetLogManager()->Log("premake5cb: created " + fname_lua.GetFullPath());
            // produce error/warning message if it was saved in the wrong folder
            wxString lua_path = fname_lua.GetPath();
            wxString ws_path  = ws->filename().GetPath();

            if (lua_path != ws_path)
            {
                Manager::Get()->GetLogManager()->LogError(wxString("premake5cb: C::B workspace and premake5 folders are not the same!")
                                                          + "\nC::B workspace folder = " + ws_path
                                                          + "\nPremake5 lua folder = " + lua_path
                                                         );
            }
        }
        else
        {
            Manager::Get()->GetLogManager()->LogError("premake5cb: could not write to " + fname_lua.GetFullPath());
        }
    }
    else
    {
        Manager::Get()->GetLogManager()->LogError("premake5cb: projects are not subdirs of " + ws->filename().GetFullPath());
    }

    //    }
}
