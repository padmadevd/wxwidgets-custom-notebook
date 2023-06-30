#include "notebook.hpp"
#include <wx/wx.h>

class testApp : public wxApp{
public:
	virtual bool OnInit();
};

wxDECLARE_APP(testApp);
wxIMPLEMENT_APP(testApp);

class testFrame : public wxFrame{
public:
	testFrame();
};

testFrame::testFrame()
	: wxFrame(nullptr, wxID_ANY, "notebook")
{
	Notebook *nb = new Notebook(this, wxID_ANY);
	wxButton *btn = new wxButton(this, wxID_ANY, "first");
	nb->addPage(btn, "first", true);
	for(int i = 1; i < 11; i++){
//		wxPrintf("%d\n", i);
		wxString label;
		label.Printf("button %d", i);
		btn = new wxButton(this, wxID_ANY, label);
		label.Printf("tab %d", i);
		nb->addPage(btn, label);
	}
//	wxPrintf("hello \n");
}

bool testApp::OnInit(){
	testFrame *frame = new testFrame;
	frame->Show();
//	wxPrintf("show frame\n");
	return true;
}