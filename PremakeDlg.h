#ifndef PREMAKEDLG_H
#define PREMAKEDLG_H

//(*Headers(PremakeDlg)
#include <wx/checkbox.h>
#include <wx/button.h>
#include "scrollingdialog.h"
//*)

class PremakeDlg: public wxScrollingDialog
{
	public:

		PremakeDlg(wxWindow* parent);
		virtual ~PremakeDlg();

		//(*Declarations(PremakeDlg)
		wxButton* ButtonExport;
		wxCheckBox* CheckBoxReplVars;
		wxCheckBox* CheckBoxUpgr;
		wxButton* ButtonCancel;
		//*)

	protected:

		//(*Identifiers(PremakeDlg)
		//*)

	private:

		//(*Handlers(PremakeDlg)
		void OnButtonExportClick(wxCommandEvent& event);
		void OnButtonCancelClick(wxCommandEvent& event);
		//*)

		DECLARE_EVENT_TABLE()
};

#endif
