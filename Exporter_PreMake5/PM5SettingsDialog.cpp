#include "PM5SettingsDialog.h"

//(*InternalHeaders(PM5SettingsDialog)
#include <wx/intl.h>
#include <wx/string.h>
//*)

//(*IdInit(PM5SettingsDialog)
const long PM5SettingsDialog::ID_TEXTCTRL1 = wxNewId();
const long PM5SettingsDialog::ID_CHECKBOX1 = wxNewId();
const long PM5SettingsDialog::ID_CHECKBOX2 = wxNewId();
const long PM5SettingsDialog::ID_CHECKBOX3 = wxNewId();
const long PM5SettingsDialog::ID_CHECKBOX4 = wxNewId();
const long PM5SettingsDialog::ID_CHECKBOX5 = wxNewId();
const long PM5SettingsDialog::ID_CHECKBOX6 = wxNewId();
const long PM5SettingsDialog::ID_CHECKBOX7 = wxNewId();
const long PM5SettingsDialog::ID_CHECKBOX8 = wxNewId();
const long PM5SettingsDialog::ID_CHECKBOX11 = wxNewId();
const long PM5SettingsDialog::ID_CHECKBOX10 = wxNewId();
const long PM5SettingsDialog::ID_CHECKBOX9 = wxNewId();
const long PM5SettingsDialog::ID_STATICTEXT3 = wxNewId();
const long PM5SettingsDialog::ID_BUTTON1 = wxNewId();
const long PM5SettingsDialog::ID_PANEL1 = wxNewId();
const long PM5SettingsDialog::ID_TEXTCTRL5 = wxNewId();
const long PM5SettingsDialog::ID_PANEL4 = wxNewId();
const long PM5SettingsDialog::ID_TEXTCTRL4 = wxNewId();
const long PM5SettingsDialog::ID_PANEL3 = wxNewId();
const long PM5SettingsDialog::ID_STATICTEXT1 = wxNewId();
const long PM5SettingsDialog::ID_TEXTCTRL3 = wxNewId();
const long PM5SettingsDialog::ID_TEXTCTRL2 = wxNewId();
const long PM5SettingsDialog::ID_PANEL2 = wxNewId();
const long PM5SettingsDialog::ID_NOTEBOOK1 = wxNewId();
//*)

BEGIN_EVENT_TABLE(PM5SettingsDialog, wxScrollingDialog)
    //(*EventTable(PM5SettingsDialog)
    //*)
END_EVENT_TABLE()

PM5SettingsDialog::PM5SettingsDialog(wxWindow * parent, wxWindowID id, const wxPoint & pos, const wxSize & size)
{
    //(*Initialize(PM5SettingsDialog)
    wxBoxSizer * BoxSizer10;
    wxBoxSizer * BoxSizer1;
    wxBoxSizer * BoxSizer2;
    wxBoxSizer * BoxSizer3;
    wxBoxSizer * BoxSizer4;
    wxBoxSizer * BoxSizer5;
    wxBoxSizer * BoxSizer6;
    wxBoxSizer * BoxSizer7;
    wxBoxSizer * BoxSizer8;
    wxBoxSizer * BoxSizer9;
    wxButton * FactorySettingsButton;
    wxNotebook * Notebook1;
    wxPanel * PanelConfigDefaults;
    wxPanel * PanelGeneral;
    wxPanel * PanelWorkspaceDefaults;
    wxPanel * Panelproject_defaults;
    wxStaticBoxSizer * StaticBoxSizer1;
    wxStaticBoxSizer * StaticBoxSizer2;
    wxStaticBoxSizer * StaticBoxSizer3;
    wxStaticBoxSizer * StaticBoxSizer4;
    wxStaticBoxSizer * StaticBoxSizer5;
    wxStaticBoxSizer * StaticBoxSizer6;
    wxStaticText * StaticText2;
    wxStaticText * StaticText4;
    wxStdDialogButtonSizer * StdDialogButtonSizer1;
    Create(parent, id, _("Premake5 exporter settings"), wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE, _T("id"));
    SetClientSize(wxDefaultSize);
    Move(wxDefaultPosition);
    BoxSizer1 = new wxBoxSizer(wxVERTICAL);
    BoxSizer2 = new wxBoxSizer(wxHORIZONTAL);
    StaticBoxSizer2 = new wxStaticBoxSizer(wxHORIZONTAL, this, _("File masks"));
    FileMasks = new wxTextCtrl(this, ID_TEXTCTRL1, wxEmptyString, wxDefaultPosition, wxSize(-1, 200), wxTE_MULTILINE, wxDefaultValidator, _T("ID_TEXTCTRL1"));
    StaticBoxSizer2->Add(FileMasks, 1, wxALL | wxEXPAND, 5);
    StaticBoxSizer2->Add(0, 400, 0, wxALL | wxALIGN_CENTER_HORIZONTAL | wxALIGN_CENTER_VERTICAL, 5);
    BoxSizer2->Add(StaticBoxSizer2, 2, wxALL | wxEXPAND, 5);
    Notebook1 = new wxNotebook(this, ID_NOTEBOOK1, wxDefaultPosition, wxSize(200, 0), 0, _T("ID_NOTEBOOK1"));
    PanelGeneral = new wxPanel(Notebook1, ID_PANEL1, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL, _T("ID_PANEL1"));
    BoxSizer3 = new wxBoxSizer(wxVERTICAL);
    export_on_build = new wxCheckBox(PanelGeneral, ID_CHECKBOX1, _("Automatic export to premake5 on build (change requires C::B restart)"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_CHECKBOX1"));
    export_on_build->SetValue(false);
    BoxSizer3->Add(export_on_build, 0, wxALL | wxALIGN_LEFT, 5);
    use_workspace_prefix = new wxCheckBox(PanelGeneral, ID_CHECKBOX2, _("Use workspace prefix in premake5 filename"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_CHECKBOX2"));
    use_workspace_prefix->SetValue(false);
    BoxSizer3->Add(use_workspace_prefix, 0, wxALL | wxALIGN_LEFT, 5);
    StaticBoxSizer6 = new wxStaticBoxSizer(wxHORIZONTAL, PanelGeneral, _("Use defaults for"));
    use_workspace_defaults = new wxCheckBox(PanelGeneral, ID_CHECKBOX3, _("Workspace"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_CHECKBOX3"));
    use_workspace_defaults->SetValue(false);
    StaticBoxSizer6->Add(use_workspace_defaults, 0, wxALL | wxEXPAND, 5);
    use_project_defaults = new wxCheckBox(PanelGeneral, ID_CHECKBOX4, _("Project"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_CHECKBOX4"));
    use_project_defaults->SetValue(false);
    StaticBoxSizer6->Add(use_project_defaults, 0, wxALL | wxEXPAND, 5);
    BoxSizer3->Add(StaticBoxSizer6, 0, wxALL | wxALIGN_LEFT, 5);
    StaticBoxSizer5 = new wxStaticBoxSizer(wxHORIZONTAL, PanelGeneral, _("Export project types"));
    ExportWindowedApp = new wxCheckBox(PanelGeneral, ID_CHECKBOX5, _("WindowedApp"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_CHECKBOX5"));
    ExportWindowedApp->SetValue(false);
    StaticBoxSizer5->Add(ExportWindowedApp, 0, wxALL | wxALIGN_CENTER_HORIZONTAL | wxALIGN_CENTER_VERTICAL, 5);
    ExportConsoleApp = new wxCheckBox(PanelGeneral, ID_CHECKBOX6, _("ConsoleApp"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_CHECKBOX6"));
    ExportConsoleApp->SetValue(false);
    StaticBoxSizer5->Add(ExportConsoleApp, 0, wxALL | wxALIGN_CENTER_HORIZONTAL | wxALIGN_CENTER_VERTICAL, 5);
    ExportStaticLib = new wxCheckBox(PanelGeneral, ID_CHECKBOX7, _("StaticLib"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_CHECKBOX7"));
    ExportStaticLib->SetValue(false);
    StaticBoxSizer5->Add(ExportStaticLib, 0, wxALL | wxALIGN_CENTER_HORIZONTAL | wxALIGN_CENTER_VERTICAL, 5);
    ExportSharedLib = new wxCheckBox(PanelGeneral, ID_CHECKBOX8, _("SharedLib"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_CHECKBOX8"));
    ExportSharedLib->SetValue(false);
    StaticBoxSizer5->Add(ExportSharedLib, 0, wxALL | wxALIGN_CENTER_HORIZONTAL | wxALIGN_CENTER_VERTICAL, 5);
    BoxSizer3->Add(StaticBoxSizer5, 0, wxALL | wxALIGN_LEFT, 5);
    export_cb_targetname = new wxCheckBox(PanelGeneral, ID_CHECKBOX11, _("Export C::B build target output file names"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_CHECKBOX11"));
    export_cb_targetname->SetValue(false);
    BoxSizer3->Add(export_cb_targetname, 1, wxALL | wxALIGN_LEFT, 5);
    post_build_copy = new wxCheckBox(PanelGeneral, ID_CHECKBOX10, _("Post build: Copy binaries to common folders under workspace"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_CHECKBOX10"));
    post_build_copy->SetValue(false);
    BoxSizer3->Add(post_build_copy, 0, wxALL | wxALIGN_LEFT, 5);
    save_all_on_export = new wxCheckBox(PanelGeneral, ID_CHECKBOX9, _("Save C::B workspace and projects on export"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_CHECKBOX9"));
    save_all_on_export->SetValue(false);
    BoxSizer3->Add(save_all_on_export, 0, wxALL | wxALIGN_LEFT, 5);
    BoxSizer3->Add(-1, -1, 1, wxALL | wxALIGN_CENTER_HORIZONTAL | wxALIGN_CENTER_VERTICAL, 5);
    StaticBoxSizer4 = new wxStaticBoxSizer(wxHORIZONTAL, PanelGeneral, _("Factory settings"));
    StaticText3 = new wxStaticText(PanelGeneral, ID_STATICTEXT3, _("Warning: This cannot be undone!"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT3"));
    StaticBoxSizer4->Add(StaticText3, 1, wxALL | wxALIGN_CENTER_HORIZONTAL | wxALIGN_CENTER_VERTICAL, 5);
    FactorySettingsButton = new wxButton(PanelGeneral, ID_BUTTON1, _("Restore factory settings"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_BUTTON1"));
    StaticBoxSizer4->Add(FactorySettingsButton, 0, wxALL | wxALIGN_CENTER_HORIZONTAL | wxALIGN_CENTER_VERTICAL, 5);
    BoxSizer3->Add(StaticBoxSizer4, 0, wxALL | wxEXPAND, 5);
    PanelGeneral->SetSizer(BoxSizer3);
    PanelWorkspaceDefaults = new wxPanel(Notebook1, ID_PANEL4, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL, _T("ID_PANEL4"));
    BoxSizer9 = new wxBoxSizer(wxVERTICAL);
    BoxSizer10 = new wxBoxSizer(wxHORIZONTAL);
    StaticText4 = new wxStaticText(PanelWorkspaceDefaults, wxID_ANY, _("Settings apply to premake workspace level"), wxDefaultPosition, wxDefaultSize, 0, _T("wxID_ANY"));
    BoxSizer10->Add(StaticText4, 1, wxALL | wxALIGN_CENTER_HORIZONTAL | wxALIGN_CENTER_VERTICAL, 5);
    BoxSizer9->Add(BoxSizer10, 0, wxEXPAND, 5);
    workspace_defaults = new wxTextCtrl(PanelWorkspaceDefaults, ID_TEXTCTRL5, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE, wxDefaultValidator, _T("ID_TEXTCTRL5"));
    BoxSizer9->Add(workspace_defaults, 1, wxALL | wxEXPAND, 5);
    PanelWorkspaceDefaults->SetSizer(BoxSizer9);
    Panelproject_defaults = new wxPanel(Notebook1, ID_PANEL3, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL, _T("ID_PANEL3"));
    BoxSizer5 = new wxBoxSizer(wxVERTICAL);
    BoxSizer8 = new wxBoxSizer(wxHORIZONTAL);
    StaticText2 = new wxStaticText(Panelproject_defaults, wxID_ANY, _("Settings apply to premake project level"), wxDefaultPosition, wxDefaultSize, 0, _T("wxID_ANY"));
    BoxSizer8->Add(StaticText2, 1, wxALL | wxALIGN_CENTER_HORIZONTAL | wxALIGN_CENTER_VERTICAL, 5);
    BoxSizer5->Add(BoxSizer8, 0, wxEXPAND, 5);
    project_defaults = new wxTextCtrl(Panelproject_defaults, ID_TEXTCTRL4, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE, wxDefaultValidator, _T("ID_TEXTCTRL4"));
    BoxSizer5->Add(project_defaults, 1, wxALL | wxEXPAND, 5);
    Panelproject_defaults->SetSizer(BoxSizer5);
    PanelConfigDefaults = new wxPanel(Notebook1, ID_PANEL2, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL, _T("ID_PANEL2"));
    BoxSizer4 = new wxBoxSizer(wxVERTICAL);
    BoxSizer7 = new wxBoxSizer(wxHORIZONTAL);
    StaticText1 = new wxStaticText(PanelConfigDefaults, ID_STATICTEXT1, _("Settings apply to premake configurations filters"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT1"));
    BoxSizer7->Add(StaticText1, 1, wxALL | wxALIGN_CENTER_HORIZONTAL | wxALIGN_CENTER_VERTICAL, 5);
    BoxSizer4->Add(BoxSizer7, 0, wxEXPAND, 5);
    BoxSizer6 = new wxBoxSizer(wxHORIZONTAL);
    StaticBoxSizer3 = new wxStaticBoxSizer(wxHORIZONTAL, PanelConfigDefaults, _("Debug"));
    configurations_debug = new wxTextCtrl(PanelConfigDefaults, ID_TEXTCTRL3, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE, wxDefaultValidator, _T("ID_TEXTCTRL3"));
    StaticBoxSizer3->Add(configurations_debug, 1, wxALL | wxEXPAND, 5);
    BoxSizer6->Add(StaticBoxSizer3, 1, wxALL | wxEXPAND, 5);
    StaticBoxSizer1 = new wxStaticBoxSizer(wxHORIZONTAL, PanelConfigDefaults, _("Release"));
    configurations_release = new wxTextCtrl(PanelConfigDefaults, ID_TEXTCTRL2, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE, wxDefaultValidator, _T("ID_TEXTCTRL2"));
    StaticBoxSizer1->Add(configurations_release, 1, wxALL | wxEXPAND, 5);
    BoxSizer6->Add(StaticBoxSizer1, 1, wxALL | wxEXPAND, 5);
    BoxSizer4->Add(BoxSizer6, 1, wxALL | wxEXPAND, 5);
    PanelConfigDefaults->SetSizer(BoxSizer4);
    Notebook1->AddPage(PanelGeneral, _("General"), false);
    Notebook1->AddPage(PanelWorkspaceDefaults, _("Workspace defaults"), false);
    Notebook1->AddPage(Panelproject_defaults, _("Project defaults"), false);
    Notebook1->AddPage(PanelConfigDefaults, _("Config defaults"), false);
    BoxSizer2->Add(Notebook1, 6, wxALL | wxEXPAND, 5);
    BoxSizer1->Add(BoxSizer2, 1, wxEXPAND, 5);
    BoxSizer1->Add(900, 0, 0, wxALL | wxALIGN_CENTER_HORIZONTAL | wxALIGN_CENTER_VERTICAL, 5);
    StdDialogButtonSizer1 = new wxStdDialogButtonSizer();
    StdDialogButtonSizer1->AddButton(new wxButton(this, wxID_OK, wxEmptyString));
    StdDialogButtonSizer1->AddButton(new wxButton(this, wxID_CANCEL, wxEmptyString));
    StdDialogButtonSizer1->Realize();
    BoxSizer1->Add(StdDialogButtonSizer1, 0, wxALL | wxALIGN_CENTER_HORIZONTAL | wxALIGN_CENTER_VERTICAL, 5);
    SetSizer(BoxSizer1);
    BoxSizer1->SetSizeHints(this);
    Connect(ID_BUTTON1, wxEVT_COMMAND_BUTTON_CLICKED, (wxObjectEventFunction)&PM5SettingsDialog::OnFactorySettingsButtonClick);
    //*)
}

PM5SettingsDialog::~PM5SettingsDialog()
{
    //(*Destroy(PM5SettingsDialog)
    //*)
}

std::vector<wxString> PM5SettingsDialog::GetLines(wxTextCtrl * ctrl)
{
    std::vector<wxString> strings;

    for (int i = 0; i < ctrl->GetNumberOfLines(); i++)
    {
        strings.push_back(ctrl->GetLineText(i));
    }

    return strings;
}

void PM5SettingsDialog::PutLines(wxTextCtrl * ctrl, const std::vector<wxString> & lines)
{
    ctrl->Clear();

    for (auto & l : lines)
    {
        (*ctrl) << l << '\n';
    }
}


void PM5SettingsDialog::ToDialog(std::shared_ptr<pm_defaults> defaults)
{
    m_defaults = defaults;
    PutLines(FileMasks, defaults->get("FileMasks"));
    PutLines(workspace_defaults, defaults->get("workspace_defaults"));
    PutLines(project_defaults, defaults->get("project_defaults"));
    PutLines(configurations_release, defaults->get("configurations_release"));
    PutLines(configurations_debug, defaults->get("configurations_debug"));
    export_on_build->SetValue(defaults->get_bool_flag("export_on_build"));
    use_workspace_prefix->SetValue(defaults->get_bool_flag("use_workspace_prefix"));
    use_workspace_defaults->SetValue(defaults->get_bool_flag("use_workspace_defaults"));
    use_project_defaults->SetValue(defaults->get_bool_flag("use_project_defaults"));
    ExportConsoleApp->SetValue(defaults->get_bool_flag("ExportConsoleApp"));
    ExportSharedLib->SetValue(defaults->get_bool_flag("ExportSharedLib"));
    ExportStaticLib->SetValue(defaults->get_bool_flag("ExportStaticLib"));
    ExportWindowedApp->SetValue(defaults->get_bool_flag("ExportWindowedApp"));
    save_all_on_export->SetValue(defaults->get_bool_flag("save_all_on_export"));
    post_build_copy->SetValue(defaults->get_bool_flag("post_build_copy"));
    export_cb_targetname->SetValue(defaults->get_bool_flag("export_cb_targetname"));
}

void PM5SettingsDialog::FromDialog(std::shared_ptr<pm_defaults> defaults)
{
    defaults->put("FileMasks", GetLines(FileMasks));
    defaults->put("workspace_defaults", GetLines(workspace_defaults));
    defaults->put("project_defaults", GetLines(project_defaults));
    defaults->put("configurations_release", GetLines(configurations_release));
    defaults->put("configurations_debug", GetLines(configurations_debug));
    defaults->put_bool_flag("export_on_build", export_on_build->GetValue());
    defaults->put_bool_flag("use_workspace_prefix", use_workspace_prefix->GetValue());
    defaults->put_bool_flag("use_workspace_defaults", use_workspace_defaults->GetValue());
    defaults->put_bool_flag("use_project_defaults", use_project_defaults->GetValue());
    defaults->put_bool_flag("ExportConsoleApp", ExportConsoleApp->GetValue());
    defaults->put_bool_flag("ExportSharedLib", ExportSharedLib->GetValue());
    defaults->put_bool_flag("ExportStaticLib", ExportStaticLib->GetValue());
    defaults->put_bool_flag("ExportWindowedApp", ExportWindowedApp->GetValue());
    defaults->put_bool_flag("save_all_on_export", save_all_on_export->GetValue());
    defaults->put_bool_flag("post_build_copy", post_build_copy->GetValue());
    defaults->put_bool_flag("export_cb_targetname", export_cb_targetname->GetValue());
    m_defaults = defaults;
}

void PM5SettingsDialog::OnFactorySettingsButtonClick(wxCommandEvent & event)
{
    if (m_defaults.get())
    {
        m_defaults->factory_settings();
        ToDialog(m_defaults);
    }
}
