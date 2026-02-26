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
    wxFont font = window->GetFont();
    font.SetPointSize(11);
    window->SetFont(font);

    wxBoxSizer *sizer = new wxBoxSizer(wxVERTICAL);
    sizer->AddStretchSpacer();
    sizer->Add(new wxStaticText(window, wxID_ANY, "Test Compiler", wxDefaultPosition, wxDefaultSize, wxALIGN_CENTER_HORIZONTAL), 0, wxEXPAND);
    sizer->AddStretchSpacer();

    window->SetSizer(sizer);

    window->SetMinSize(wxSize(900, 600));
    window->SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_FRAMEBK));
	window->Center();
}

bool inventory::AppView::initialize()
{
    return window->Show();
}