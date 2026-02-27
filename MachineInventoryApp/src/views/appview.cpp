#include "views/appview.h"

wxDEFINE_EVENT(EVT_APPVIEW_UI, wxCommandEvent);
wxDEFINE_EVENT(EVT_APPVIEW_LOGOUT, wxCommandEvent);

inventory::AppView::AppView()
    : wxEvtHandler()
    , model(AppModel::GetInstance())
    , currentUI(NONE)
    , currentWidget(nullptr)
    , form(nullptr)
    , authForm(nullptr)
    , ID_MENU_LOGOUT(wxID_HIGHEST + 1)
{
    setupUI();

    window->Bind(wxEVT_MENU, [this](wxCommandEvent&) {
        window->Close(true);
    }, wxID_EXIT);

    window->Bind(wxEVT_MENU, [this](wxCommandEvent&) {
        wxCommandEvent evt(EVT_APPVIEW_LOGOUT);
        ProcessEvent(evt);
    }, ID_MENU_LOGOUT);
}

inventory::AppView& inventory::AppView::GetInstance()
{
    static AppView instance;
    return instance;
}

void inventory::AppView::Initialize()
{
    SetUI(LOADING);
    window->Show();
}

void* inventory::AppView::SetUI(UI ui)
{
    if (currentUI == ui)
        return currentWidget;
    
    // removendo outros ponteiros
    form = nullptr;
    loadingForm = nullptr;
    syncForm = nullptr;
    authForm = nullptr;
    machineForm = nullptr;
    homeForm = nullptr;

    if (ui & SYNC) {
        currentWidget = syncForm = new SyncForm(window);
        form = syncForm;
    }

    if (ui & MACHINE) {
        currentWidget = machineForm = new MachineForm(window);
        form = machineForm;
    }

    switch (ui)
    {
    case LOADING:
        currentWidget = loadingForm = new LoadingForm(window);
        form = loadingForm;
        break;

    case AUTH:
        currentWidget = authForm = new AuthForm(window);
        form = authForm;
        break;

    case HOME:
        currentWidget = homeForm = new HomeForm(window, syncForm, machineForm);
        form = homeForm;
        break;
    }

    sizer->Clear(true);
    sizer->Add((wxWindow*)currentWidget, 1, wxEXPAND | wxALL, 10);
    
    window->SetMenuBar(syncForm || homeForm? menuBar : NULL);
    window->Layout();

    wxCommandEvent event(EVT_APPVIEW_UI);
    event.SetInt(ui);
    ProcessEvent(event);

    return currentWidget;
}

void inventory::AppView::setupUI()
{
    wxMenu* menuFile;
    const wxSize& size = wxSize(900, 600);

    window = new wxFrame(NULL, wxID_ANY, "Machine Inventory", wxDefaultPosition, size);
    window->SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_FRAMEBK));
    window->SetMinSize(size);
    window->Center();

    window->SetMenuBar(menuBar = new wxMenuBar());
    menuBar->Append(menuFile = new wxMenu(), "&Arquivo");
    
    menuFile->Append(ID_MENU_LOGOUT, "&Logout\tCtrl+L", "Retorna a tela de login");
    menuFile->AppendSeparator();
    menuFile->Append(wxID_EXIT, "&Sair\tCtrl+Q", "Fecha a aplicação");

    wxFont font = window->GetFont();
    font.SetPointSize(11);
    window->SetFont(font);

    sizer = new wxBoxSizer(wxVERTICAL);
    window->SetSizer(sizer);
}