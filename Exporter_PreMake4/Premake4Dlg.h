#ifndef PREMAKEDLG_H
#define PREMAKEDLG_H

//(*Headers(Premake4Dlg)
#include <wx/checkbox.h>
#include <wx/button.h>
#include "scrollingdialog.h"
//*)

class Premake4Dlg: public wxScrollingDialog
{
    public:

        Premake4Dlg(wxWindow * parent);
        virtual ~Premake4Dlg();

        //(*Declarations(Premake4Dlg)
        wxButton * ButtonExport;
        wxCheckBox * CheckBoxReplVars;
        wxCheckBox * CheckBoxUpgr;
        wxButton * ButtonCancel;
        //*)

    protected:

        //(*Identifiers(Premake4Dlg)
        //*)

    private:

        //(*Handlers(Premake4Dlg)
        void OnButtonExportClick(wxCommandEvent & event);
        void OnButtonCancelClick(wxCommandEvent & event);
        //*)

        DECLARE_EVENT_TABLE()
};

#endif
