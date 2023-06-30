#include "notebook.hpp"

#include <wx/dcbuffer.h>
#include <wx/dcgraph.h>

#include <wx/settings.h> //for system colours
#include <wx/wxcrtvararg.h> //for wxprintf

static float minOf(float x, float y, float z)
{
    float m = x < y ? x : y;
    m = m < z ? m : z;
    return m;
}

static float maxOf(float x, float y, float z)
{
    float m = x > y ? x : y;
    m = m > z ? m : z;
    return m;
}

static void RGBtoHSL(float r, float g, float b, float* h, float* s, float* l)
{
    float var_R = (r / 255.0); // RGB from 0 to 255
    float var_G = (g / 255.0);
    float var_B = (b / 255.0);

    float var_Min = minOf(var_R, var_G, var_B); // Min. value of RGB
    float var_Max = maxOf(var_R, var_G, var_B); // Max. value of RGB
    float del_Max = var_Max - var_Min;           // Delta RGB value

    *l = (var_Max + var_Min) / 2.0;

    if(del_Max == 0) { // This is a gray, no chroma...
        *h = 0;        // HSL results from 0 to 1
        *s = 0;
    } else { // Chromatic data...
        if(*l < 0.5)
            *s = del_Max / (var_Max + var_Min);
        else
            *s = del_Max / (2.0 - var_Max - var_Min);

        float del_R = (((var_Max - var_R) / 6.0) + (del_Max / 2.0)) / del_Max;
        float del_G = (((var_Max - var_G) / 6.0) + (del_Max / 2.0)) / del_Max;
        float del_B = (((var_Max - var_B) / 6.0) + (del_Max / 2.0)) / del_Max;

        if(var_R == var_Max)
            *h = del_B - del_G;
        else if(var_G == var_Max)
            *h = (1.0 / 3.0) + del_R - del_B;
        else if(var_B == var_Max)
            *h = (2.0 / 3.0) + del_G - del_R;

        if(*h < 0) *h += 1;
        if(*h > 1) *h -= 1;
    }
}

bool isLightColour(wxColour colour){
	float h, s, l;
	RGBtoHSL(colour.Red(), colour.Green(), colour.Blue(), &h, &s, &l);
//	wxPrintf("%f\n", l);
	if(l > 0.5f)
		return true;
	else
		return false;
}

void drawClippedText(wxString text, wxGCDC *dc, wxPoint origin, wxSize size, bool center = false){
	wxSize textExt = dc->GetTextExtent(text);
	if(textExt.x > size.x){
		int chWidth = dc->GetTextExtent("_").x;
		int strlen = size.x/chWidth;
		while(dc->GetTextExtent(text.substr(0, strlen+1)).x < size.x){
			strlen += 1;
		}
		strlen -= 2;
		if(strlen < text.length() && strlen > -1)
			text.replace(strlen, text.length()-strlen, "..");
	}
	textExt = dc->GetTextExtent(text);
	wxPoint pos;
	if(center)
		pos.x = origin.x + size.x/2 - textExt.x/2;
	else
		pos.x = origin.x;
	pos.y = origin.y + size.y/2 - textExt.y/2;
	dc->DrawText(text, pos);
}

void drawCross(wxColour fore, wxColour back, wxGCDC *dc, wxPoint pos,wxSize size){
	dc->SetPen(wxPen(back, 0));
	dc->SetBrush(back);
	dc->DrawEllipse(pos.x, pos.y, size.x, size.y);

	dc->SetPen(wxPen(fore, 1));
	dc->SetBrush(fore);
	int widthUnit = size.x/6;
	int heightUnit = size.y/6;
	wxPoint center(pos.x+size.x/2, pos.y+size.y/2);
	dc->DrawLine(center.x-widthUnit, center.y-heightUnit, center.x+widthUnit, center.y+heightUnit);
	dc->DrawLine(center.x+widthUnit, center.y-heightUnit, center.x-widthUnit, center.y+heightUnit);
}

void drawArrowUp(wxColour fore, wxColour back, wxGCDC *dc, wxPoint pos,wxSize size){
	dc->SetPen(wxPen(back, 0));
	dc->SetBrush(back);
	dc->DrawEllipse(pos.x, pos.y, size.x, size.y);

	dc->SetPen(wxPen(fore, 1));
	dc->SetBrush(fore);
	int widthUnit = size.x/4;
	int heightUnit = size.y/8;
	wxPoint center(pos.x+size.x/2, pos.y+size.y/2-heightUnit/2);
	dc->DrawLine(center.x-widthUnit, center.y+heightUnit, center.x, center.y-heightUnit);
	dc->DrawLine(center.x, center.y-heightUnit, center.x+widthUnit, center.y+heightUnit);
}

void drawArrowDown(wxColour fore, wxColour back, wxGCDC *dc, wxPoint pos,wxSize size){
	dc->SetPen(wxPen(back, 0));
	dc->SetBrush(back);
	dc->DrawEllipse(pos.x, pos.y, size.x, size.y);

	dc->SetPen(wxPen(fore, 1));
	dc->SetBrush(fore);
	int widthUnit = size.x/4;
	int heightUnit = size.y/8;
	wxPoint center(pos.x+size.x/2, pos.y+size.y/2+heightUnit/2);
	dc->DrawLine(center.x-widthUnit, center.y-heightUnit, center.x, center.y+heightUnit);
	dc->DrawLine(center.x, center.y+heightUnit, center.x+widthUnit, center.y-heightUnit);
}

wxDEFINE_EVENT(EVT_NBTAB_CLOSE_BTN_CLICK, wxCommandEvent);
wxDEFINE_EVENT(EVT_NBTAB_SELECT, wxCommandEvent);
wxDEFINE_EVENT(EVT_NBTAB_EMPTY, wxCommandEvent);

tabsPanel::tabsPanel(wxWindow *parent, wxWindowID id, wxPoint position, wxSize size)
	:wxWindow(parent, id, position, size)
{

	if(!wxSystemSettings::GetAppearance().IsDark()){
		colours[fg] = wxColour("#353535");
		colours[bg] = wxColour("#8A8A8A");
		colours[bghover] = wxColour("#9A9996");
		colours[bgselect] = wxColour("#DEDDDA");
		colours[bgbtn] = wxColour("#FFFFFF");
	}else{
		colours[fg] = wxColour("#D0CFCC");
		colours[bg] = wxColour("#131313");
		colours[bghover] = wxColour("#252525");
		colours[bgselect] = wxColour("#353535");
		colours[bgbtn] = wxColour("#505050");
	}

	Bind(wxEVT_PAINT, &tabsPanel::onPaint, this, GetId());
	SetInitialSize(wxSize(-1, DoGetBestClientHeight()));
}

tabsPanel::~tabsPanel(){
	for(int i = 0;i < tabs.size(); i++){
		delete tabs[i];
	}
}

wxSize tabsPanel::DoGetBestClientSize(){
	wxSize textExt = GetTextExtent("_______________|");
	wxSize size;
	size.x = (textExt.x + 4*padding + 8 + 20)*tabs.size();
	if(textExt.y < 20)
		textExt.y = 20;
	size.y = textExt.y + 2*padding;
	return size;
}

int tabsPanel::DoGetBestClientHeight(){
	return DoGetBestClientSize().y;
}

int tabsPanel::DoGetBestClientWidth(){
	return DoGetBestClientSize().x;
}

int tabsPanel::find(tabinfo *tab){
	int index = -1;
	for(int i = 0; i < tabs.size(); i++){
		if(tabs[i] == tab){
			index = i;
			break;
		}
	}
	
//	wxPrintf("find \n");
	
	return index;
}

int tabsPanel::findByLink(wxWindow *window){
	int index = -1;
	for(int i = 0; i < tabs.size(); i++){
		if(tabs[i]->m_link == window){
			index = i;
			break;
		}
	}

//	wxPrintf("find by link \n");

	return index;
}

tabinfo* tabsPanel::insert(int index, wxString label, bool select){
	tabinfo *tab = new tabinfo;
	tab->m_label = label;
	if(tabs.size() == 0){
		bindInputEvents();
	}
	tabs.insert(tabs.begin()+index, tab);
	if(select)
		active = tab;

	return tab;
}

tabinfo* tabsPanel::insert(tabinfo *tab, wxString label, bool after, bool select){
	tabinfo *ntab = new tabinfo;
	ntab->m_label = label;
	if(tabs.size() == 0){
		bindInputEvents();
	}
	int index = find(tab);
	if(after)
		index += 1;
	tabs.insert(tabs.begin()+index, ntab);

	if(select)
		active = tab;

	return ntab;
}

tabinfo* tabsPanel::append(wxString label, bool select){
	tabinfo *tab = new tabinfo;
	tab->m_label = label;
	if(tabs.size() == 0){
		bindInputEvents();
	}
	tabs.push_back(tab);

	if(select)
		active = tab;

//	wxPrintf("appen tab \n");

	return tab;
}

bool tabsPanel::remove(tabinfo* tab){

	int index = find(tab);
	
	if(index == -1)
		return false;

	delete tab;
	tabs.erase(tabs.begin()+index);

	if(tabs.size() == 0){
		unbindInputEvents();
		emitEmptyEvent();
	}
	
//	wxPrintf("remove tab \n");
	
	return true;
}

bool tabsPanel::removeByLink(wxWindow *window){

	int index = findByLink(window);
	if(index == -1)
		return false;

	tabinfo *tab = tabs[index];
	delete tab;
	tabs.erase(tabs.begin()+index);

	if(tabs.size() == 0){
		unbindInputEvents();
		emitEmptyEvent();
	}

//	wxPrintf("remove by link\n");

	return true;
}

void tabsPanel::removeAll(){
	for(int i = 0;i < tabs.size(); i++){
		delete tabs[i];
	}
	unbindInputEvents();
	emitEmptyEvent();
	tabs.clear();
//	wxPrintf("remove all tab \n");
}

bool tabsPanel::swap(tabinfo *tab1, tabinfo *tab2){
	if(!tab1 || !tab2)
		return false;
	int i1 = find(tab1);
	int i2 = find(tab2);
	tabs[i1] = tab2;
	tabs[i2] = tab1;
	
//	wxPrintf("swap \n");
	
	return true;
}

bool tabsPanel::select(tabinfo *tab){
	if(find(tab) == -1)
		return false;

	active = tab;
	Refresh();
	
//	wxPrintf("select \n");
	
	return true;
}

void tabsPanel::doSizeCalculation(){
	virtualWidth = actualWidth = GetClientSize().x;
	if(actualWidth < DoGetBestClientWidth()){
		virtualWidth = DoGetBestClientWidth();
	}
	actualHeight = GetClientSize().y;
	if(tabs.size()>0)
		tabWidth = virtualWidth/tabs.size();
	else
		tabWidth = 0;
//	wxPrintf("size calc \n");
}

tabinfo* tabsPanel::hitTest(wxPoint pos){
	if(scrollOffset > virtualWidth - actualWidth){
		scrollOffset = virtualWidth - actualWidth;
	}
	if(scrollOffset < 0){
		scrollOffset = 0;
	}
	if(tabWidth > 0){
		tabIndex = (pos.x+scrollOffset)/tabWidth;
		tabMousePoint = (pos.x+scrollOffset)%tabWidth;
	}else{
		tabIndex = -1;
		tabMousePoint = -1;
		closeHover = false;
		closeDown = false;
		return nullptr;
	}
	if(tabMousePoint >= tabWidth-20-padding && tabMousePoint <= tabWidth-padding && pos.y <= GetClientSize().y/2+10 && pos.y >= GetClientSize().y/2-10){
		closeHover = true;
	}else{
		closeHover = false;
	}
	if(tabIndex < 0)
		tabIndex = 0;
	else if(tabIndex > tabs.size()-1)
		tabIndex = tabs.size()-1;
	
//	wxPrintf("hit test \n");
	
	return tabs[tabIndex];
}

void tabsPanel::onMouseMove(wxMouseEvent &event){
	bool doRefresh = false;
	bool _closeHover = closeHover;

	mousePos = event.GetPosition();
	tabinfo *tab = hitTest(mousePos);

	if(_closeHover != closeHover)
		doRefresh = true;

	if(closeDown)
		closeDown = false;

	if(event.LeftIsDown() && !onDrag && !closeDown){
		onDrag = tab;
		active = tab;
		emitSelectEvent();
		tabSelPoint = tabMousePoint;
	}

	if(onHover != tab){
		onHover = tab;
		doRefresh = true;
	}

	if(onDrag)
		doRefresh = true;
	
	if(doRefresh)
		Refresh();
		
//	wxPrintf("on mouse move \n");
}

void tabsPanel::onMouseLeave(wxMouseEvent &event){
	onHover = NULL;
	closeHover = false;
	Refresh();
//	wxPrintf("on mouse leave \n");
}

void tabsPanel::onLeftDown(wxMouseEvent &event){
	if(closeHover)
		closeDown = true;
	else{
		if(active != onHover){
			active = onHover;
			emitSelectEvent();
		}
	}
	CaptureMouse();
	Refresh(true);
//	wxPrintf("on left down \n");
}

void tabsPanel::onLeftUp(wxMouseEvent &event){
	if(closeDown){
		closeDown = false;
		emitCloseEvent();
	}else{
		if(onDrag){
			if(onHover != onDrag){
				swap(onDrag, onHover);
			}
			onHover = onDrag;
			onDrag = NULL;
		}
	}
	if(HasCapture())
		ReleaseMouse();
	Refresh();
//	wxPrintf("on left up \n");
}

void tabsPanel::onMouseWheel(wxMouseEvent &event){

	scrollOffset += event.GetWheelRotation()*0.1;

	onHover = hitTest(mousePos);
	Refresh();
//	wxPrintf("on mouse wheel \n");
}

void tabsPanel::onPaint(wxPaintEvent &event){

//	wxPrintf("on paint\n");
	SetBackgroundStyle(wxBG_STYLE_PAINT);
	wxBufferedPaintDC dc(this);
	wxGCDC gc(dc);
	gc.SetBackground(colours[bg]);
	gc.Clear();

	doSizeCalculation();
	onHover = hitTest(mousePos);
//	wxPrintf("on paint\n");

	for(int i = 0; i < tabs.size(); i++){

		if(tabs[i] == onDrag)
			continue;

		wxPoint tabStart = wxPoint(i*(tabWidth)-scrollOffset, 0);

		if(tabs[i] == active){

			gc.SetPen(wxPen(colours[bgselect], 0));
			gc.SetBrush(colours[bgselect]);
			gc.DrawRectangle(tabStart.x, tabStart.y, tabWidth, actualHeight);

		}else if(tabs[i] == onHover){

			gc.SetPen(wxPen(colours[bghover], 0));
			gc.SetBrush(colours[bghover]);
			gc.DrawRectangle(tabStart.x, tabStart.y, tabWidth, actualHeight);
		}

		if(tabs[i]->m_mod){
			gc.SetPen(wxPen(colours[fg], 0));
			gc.SetBrush(colours[fg]);
			gc.DrawEllipse(tabStart.x+padding, actualHeight/2-4, 8, 8);
		}

		gc.SetTextForeground(colours[fg]);
		if(tabs[i] == onHover){
			if(closeHover){
				if(closeDown)
					drawCross(colours[fg], colours[bg], &gc, tabStart+wxPoint(tabWidth-padding-20, padding), wxSize(20, 20));
				else
					if(tabs[i] == active)
						drawCross(colours[fg], colours[bgbtn], &gc, tabStart+wxPoint(tabWidth-padding-20, padding), wxSize(20, 20));
					else
						drawCross(colours[fg], colours[bgselect], &gc, tabStart+wxPoint(tabWidth-padding-20, padding), wxSize(20, 20));
			}else{
				if(tabs[i] == active)
					drawCross(colours[fg], colours[bgselect], &gc, tabStart+wxPoint(tabWidth-padding-20, padding), wxSize(20, 20));
				else
					drawCross(colours[fg], colours[bghover], &gc, tabStart+wxPoint(tabWidth-padding-20, padding), wxSize(20, 20));
			}
			drawClippedText(tabs[i]->m_label, &gc, wxPoint(2*padding+8, 0)+tabStart, wxSize(tabWidth-4*padding-28, actualHeight));
		}else{
			drawClippedText(tabs[i]->m_label, &gc, wxPoint(2*padding+8, 0)+tabStart, wxSize(tabWidth-8-4*padding, actualHeight));
		}
	}

	if(onDrag){
		gc.SetPen(wxPen(colours[bgselect], 0));
		gc.SetBrush(colours[bgselect]);
		wxPoint tabStart = wxPoint(mousePos.x - tabSelPoint, 0);
		gc.SetTextForeground(colours[fg]);
		gc.DrawRectangle(tabStart.x, 0, tabWidth, actualHeight);
		drawClippedText(onDrag->m_label, &gc, tabStart, wxSize(tabWidth, actualHeight), true);
	}
//	wxPrintf("on paint \n");

}

void tabsPanel::bindInputEvents(){
	Bind(wxEVT_MOTION, &tabsPanel::onMouseMove, this, GetId());
	Bind(wxEVT_LEAVE_WINDOW, &tabsPanel::onMouseLeave, this, GetId());
	Bind(wxEVT_LEFT_DOWN, &tabsPanel::onLeftDown, this, GetId());
	Bind(wxEVT_LEFT_UP, &tabsPanel::onLeftUp, this, GetId());
	Bind(wxEVT_MOUSEWHEEL, &tabsPanel::onMouseWheel, this, GetId());
//	wxPrintf("bind \n");
}

void tabsPanel::unbindInputEvents(){
	Unbind(wxEVT_MOTION, &tabsPanel::onMouseMove, this, GetId());
	Unbind(wxEVT_LEAVE_WINDOW, &tabsPanel::onMouseLeave, this, GetId());
	Unbind(wxEVT_LEFT_DOWN, &tabsPanel::onLeftDown, this, GetId());
	Unbind(wxEVT_LEFT_UP, &tabsPanel::onLeftUp, this, GetId());
	Unbind(wxEVT_MOUSEWHEEL, &tabsPanel::onMouseWheel, this, GetId());
//	wxPrintf("unbind \n");
}

void tabsPanel::emitCloseEvent(){
	wxCommandEvent event(EVT_NBTAB_CLOSE_BTN_CLICK);
	event.SetString(onHover->m_label);
	event.SetId(GetId());
	GetEventHandler()->ProcessEvent(event);
//	wxPrintf("emit close \n");
}

void tabsPanel::emitSelectEvent(){
	wxCommandEvent event(EVT_NBTAB_SELECT);
	event.SetString(active->m_label);
	event.SetId(GetId());
	GetEventHandler()->ProcessEvent(event);
//	wxPrintf("emit select \n");
}

void tabsPanel::emitEmptyEvent(){
	wxCommandEvent event(EVT_NBTAB_EMPTY);
	event.SetId(GetId());
	GetEventHandler()->ProcessEvent(event);
//	wxPrintf("emit empty \n");
}

Notebook::Notebook(wxWindow *parent, wxWindowID id, wxPoint position, wxSize size)
	: wxWindow(parent, id, position, size)
{
	panel = new tabsPanel(this, wxID_ANY);
	container = new windowStack(this, wxID_ANY);
	wxBoxSizer *sizer = new wxBoxSizer(wxVERTICAL);
	sizer->Add(panel, wxSizerFlags(0).Expand());
	sizer->Add(container, wxSizerFlags(1).Expand());
	SetSizerAndFit(sizer);

	Bind(EVT_NBTAB_SELECT, &Notebook::onTabSelect, this, panel->GetId());
	Bind(EVT_NBTAB_CLOSE_BTN_CLICK, &Notebook::onCloseBtn, this, panel->GetId());
}

void Notebook::addPage(wxWindow *window, wxString label, bool select){
	container->addWindow(window, select);
	tabinfo *tab = panel->append(label, select);
	tab->m_link = window;
//	wxPrintf("add page\n");
}

bool Notebook::selectPage(wxWindow *window){
	return container->selectWindow(window) && panel->select(panel->tabs[panel->findByLink(window)]);
//	wxPrintf("select page\n");
}

bool Notebook::removePage(wxWindow *window){
	return panel->removeByLink(window) && container->removeWindow(window);
}

void Notebook::onTabSelect(wxCommandEvent &event){
	selectPage(panel->active->m_link);
}

void Notebook::onCloseBtn(wxCommandEvent &event){
	int index = panel->find(panel->onHover);
	if(panel->tabs.size() > 1){
		if(index == panel->tabs.size()-1){
			selectPage(panel->tabs[index-1]->m_link);
		}else{
			selectPage(panel->tabs[index+1]->m_link);
		}
	}
	container->removeWindow(panel->onHover->m_link);
	panel->remove(panel->onHover);
}