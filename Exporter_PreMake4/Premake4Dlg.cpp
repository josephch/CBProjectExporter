// System include files

// CB include files
#include <sdk.h> // Code::Blocks SDK
#include "manager.h"
#include <logmanager.h>

// ProjectExporter include files
#include "Premake4Dlg.h"
#include "Premake4Exporter.h"

//(*InternalHeaders(Premake4Dlg)
#include <wx/xrc/xmlres.h>
//*)

//(*IdInit(Premake4Dlg)
//*)

BEGIN_EVENT_TABLE(Premake4Dlg, wxScrollingDialog)
    //(*EventTable(Premake4Dlg)
    //*)
END_EVENT_TABLE()

Premake4Dlg::Premake4Dlg(wxWindow * parent)
{
    //(*Initialize(Premake4Dlg)
    wxXmlResource::Get()->LoadObject(this, parent, "Premake4Dlg", "wxScrollingDialog");
    CheckBoxReplVars = (wxCheckBox *)FindWindow(XRCID("ID_CHECKBOX1"));
    CheckBoxUpgr = (wxCheckBox *)FindWindow(XRCID("ID_CHECKBOX2"));
    ButtonExport = (wxButton *)FindWindow(XRCID("ID_BUTTON1"));
    ButtonCancel = (wxButton *)FindWindow(XRCID("ID_BUTTON2"));
    Connect(XRCID("ID_BUTTON1"), wxEVT_COMMAND_BUTTON_CLICKED, (wxObjectEventFunction)&Premake4Dlg::OnButtonExportClick);
    Connect(XRCID("ID_BUTTON2"), wxEVT_COMMAND_BUTTON_CLICKED, (wxObjectEventFunction)&Premake4Dlg::OnButtonCancelClick);
    //*)
}

Premake4Dlg::~Premake4Dlg()
{
    //(*Destroy(Premake4Dlg)
    //*)
}

void Premake4Dlg::OnButtonExportClick(wxCommandEvent & event)
{
    Premake4Exporter * ExportObject = new Premake4Exporter();
    ExportObject->RunExport(CheckBoxReplVars->IsChecked(), CheckBoxUpgr->IsChecked());
    EndModal(wxID_OK);
}

void Premake4Dlg::OnButtonCancelClick(wxCommandEvent & event)
{
    EndModal(wxID_CANCEL);
}
