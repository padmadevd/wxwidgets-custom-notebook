#include "windowstack.hpp"
#include <wx/wxcrtvararg.h>

windowStack::windowStack(wxWindow *parent, wxWindowID id, wxPoint position, wxSize size)
	:wxWindow(parent, id, position, size)
{
	SetInitialSize(wxSize(200, 200));
	Bind(wxEVT_SIZE, &windowStack::onResize, this, GetId());
}

void windowStack::addWindow(wxWindow *window, bool select){
	if(contains(window) != -1)
		return;
	items.push_back(window);
	window->Reparent(this);
	window->Hide();
	if(active == NULL)
		active = window;
	if(select)
		selectWindow(window);
//	wxPrintf("add window\n");
}

bool windowStack::selectWindow(wxWindow *window){
	if(contains(window) == -1)
		return false;
	if(active != NULL)
		active->Hide();
	active = window;
	window->SetSize(wxRect(0, 0, GetSize().x, GetSize().y));
	window->Show();
	Refresh();
//	wxPrintf("select window\n");
	return true;
}

bool windowStack::removeWindow(wxWindow *window){
	int index = contains(window);
	if(index == -1)
		return false;
	items.erase(items.begin()+index);
	if(active == window)
		active = NULL;
	window->Hide();
	delete window;
//	wxPrintf("remove window\n");
	return true;
}

int windowStack::contains(wxWindow *window){
	int index = -1;
	for(int i = 0; i < items.size(); i++){
		if(items[i] == window){
			index = i;
			break;
		}
	}
//	wxPrintf("contains\n");
	return index;
}

void windowStack::onResize(wxSizeEvent &event){
	if(active != NULL){
		active->SetSize(wxRect(0, 0, GetSize().x, GetSize().y));
	}
	event.Skip();
//	wxPrintf("on resize\n");
}