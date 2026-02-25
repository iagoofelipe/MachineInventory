#include "appview.h"

inventory::AppView& inventory::AppView::getInstance()
{
    static AppView instance;
    return instance;
}

inventory::AppView::AppView()
    : model(AppModel::getInstance())
    , window(new wxFrame(NULL, wxID_ANY, "Machine Inventory"))
{
    window->SetMinSize(wxSize(900, 600));
    window->SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_FRAMEBK));
	window->Center();
}

bool inventory::AppView::initialize()
{
    return window->Show();
}