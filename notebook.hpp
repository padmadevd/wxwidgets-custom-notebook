#ifndef NOTEBOOK_HPP
#define NOTEBOOK_HPP

#include <wx/window.h>
#include <wx/sizer.h>
#include <wx/string.h>
#include "windowstack.hpp"

class tabinfo{
public:
	wxString m_label;
	wxWindow *m_link = nullptr;
	bool m_mod = false;
};

class tabsPanel : public wxWindow{
public:
	std::vector<tabinfo*> tabs;
	tabinfo *active = nullptr;
	tabinfo *onHover = nullptr;
	tabinfo *onDrag = nullptr;
	bool closeHover = false;
	bool closeDown = false;
	tabsPanel(wxWindow *parent, wxWindowID id, wxPoint position = wxDefaultPosition, wxSize size = wxDefaultSize);
	~tabsPanel();

	int find(tabinfo *tab);
	int findByLink(wxWindow *window);

	tabinfo* insert(int index, wxString label, bool select = false);
	tabinfo* insert(tabinfo *tab, wxString label, bool after, bool select = false);
	tabinfo* append(wxString label, bool select = false);
	bool remove(tabinfo *tab);
	bool removeByLink(wxWindow *window);
	void removeAll();

	bool swap(tabinfo *tab1, tabinfo *tab2);
	bool select(tabinfo *tab);

	tabinfo* hitTest(wxPoint pos);
	void doSizeCalculation();

protected:
	wxColour colours[5];

	enum{
		fg,
		bg,
		bghover,
		bgselect,
		bgbtn
	};
	
	virtual wxSize DoGetBestClientSize();
	virtual int DoGetBestClientHeight();
	virtual int DoGetBestClientWidth();
	
	int scrollOffset = 0;
	int actualHeight;
	int actualWidth;
	int virtualWidth;

	wxPoint mousePos;
	int tabSelPoint = 0;
	int tabMousePoint = 0;
	int tabIndex = 0;

	int padding = 10;
	int tabWidth;
	
private:
	
	void onPaint(wxPaintEvent &event);
	void onLeftDown(wxMouseEvent &event);
	void onLeftUp(wxMouseEvent &event);
	void onMouseWheel(wxMouseEvent &event);
	void onMouseMove(wxMouseEvent &event);
	void onMouseLeave(wxMouseEvent &event);
	void bindInputEvents();
	void unbindInputEvents();
	void emitCloseEvent();
	void emitSelectEvent();
	void emitEmptyEvent();
};

wxDECLARE_EVENT(EVT_NBTAB_CLOSE_BTN_CLICK, wxCommandEvent);
wxDECLARE_EVENT(EVT_NBTAB_SELECT, wxCommandEvent);
wxDECLARE_EVENT(EVT_NBTAB_EMPTY, wxCommandEvent);

class Notebook : public wxWindow{
	
public:
	tabsPanel *panel;
	windowStack *container;

	Notebook(wxWindow *parent, wxWindowID id, wxPoint position = wxDefaultPosition, wxSize size = wxDefaultSize);

	void addPage(wxWindow *window, wxString label, bool select = false);
	bool selectPage(wxWindow *window);
	bool removePage(wxWindow *window);
	void onTabSelect(wxCommandEvent &event);
	void onCloseBtn(wxCommandEvent &event);
};

#endif