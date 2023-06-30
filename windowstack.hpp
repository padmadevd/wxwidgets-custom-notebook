#ifndef WINDOWSTACK_HPP
#define WINDOWSTACK_HPP

#include <wx/window.h>
#include <vector>

class windowStack : public wxWindow{
public:
	windowStack(wxWindow *parent, wxWindowID id, wxPoint position = wxDefaultPosition, wxSize size = wxDefaultSize);
	void addWindow(wxWindow *window, bool select = false);
	bool selectWindow(wxWindow *window);
	bool removeWindow(wxWindow *window);
	int contains(wxWindow *window);
	
protected:
	std::vector<wxWindow*> items;
	wxWindow *active = NULL;

	void onResize(wxSizeEvent &event);
};

#endif