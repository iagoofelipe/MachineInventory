#include "appview.h"
#include "id.h"

wxDEFINE_EVENT(EVT_APPVIEW_LOGOUT, wxCommandEvent);

inventory::AppView::AppView()
    :  wxFrame(NULL, wxID_ANY, "Machine Inventory", wxDefaultPosition, wxSize(900, 600))
    , currentUI(-1)
    , currentWidget(nullptr)
    , timerStatusbar(this, ID_TIMER_STATUSBAR)
{
    setupUI();

    Bind(wxEVT_MENU, [this](wxCommandEvent&) { Close(true); }, wxID_EXIT);
    Bind(wxEVT_MENU, [this](wxCommandEvent&) { wxCommandEvent evt(EVT_APPVIEW_LOGOUT); ProcessEvent(evt); }, ID_MENU_LOGOUT);
    Bind(wxEVT_TIMER, [this](wxTimerEvent&) { this->GetStatusBar()->SetLabel(""); }, ID_TIMER_STATUSBAR);
}

void inventory::AppView::Initialize()
{
    Show();
}

void* inventory::AppView::SetUI(int id)
{
    if (currentUI == id)
        return currentWidget;
    
    // removendo outros ponteiros
    form = nullptr;
    loadingForm = nullptr;
    syncForm = nullptr;
    authForm = nullptr;
    machineForm = nullptr;
    homeForm = nullptr;

    // if (ui & UI_SYNC) {
    //     currentWidget = syncForm = new SyncForm(this, ID_SYNC_FORM);
    //     form = syncForm;
    // }

    // if (ui & UI_MACHINE) {
    //     currentWidget = machineForm = new MachineForm(this, ID_MACHINE_FORM);
    //     form = machineForm;
    // }

    switch (id)
    {
    case ID_LOADING_FORM:
        currentWidget = loadingForm = new LoadingForm(this, ID_LOADING_FORM);
        form = loadingForm;
        break;

    case ID_AUTH_FORM:
        currentWidget = authForm = new AuthForm(this, ID_AUTH_FORM);
        form = authForm;
        break;

    case ID_HOME_FORM:
        currentWidget = homeForm = new HomeForm(this, ID_HOME_FORM);
        syncForm = homeForm->GetSyncForm();
        machineForm = homeForm->GetMachineForm();
        form = homeForm;
        break;

    case ID_SYNC_FORM:
        currentWidget = syncForm = new SyncForm(this, ID_SYNC_FORM);
        form = syncForm;
        break;
    
    case ID_MACHINE_FORM:
        currentWidget = machineForm = new MachineForm(this, ID_MACHINE_FORM);
        form = machineForm;
        break;
    }

    sizer->Clear(true);
    sizer->Add((wxWindow*)currentWidget, 1, wxEXPAND | wxALL, 10);
    
    SetMenuBar(syncForm || machineForm? menuBar : NULL);
    Layout();

    return currentWidget;
}

void inventory::AppView::setupUI()
{
    SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_FRAMEBK));
    SetIcon(wxICON(MAINICON));
    SetStatusBar(new wxStatusBar(this));

    wxFont font = GetFont();
    font.SetPointSize(11);
    SetFont(font);
    
    wxMenu* menuFile;
    SetMenuBar(menuBar = new wxMenuBar());
    menuBar->Append(menuFile = new wxMenu(), "&Arquivo");
    
    menuFile->Append(ID_MENU_LOGOUT, "&Logout\tCtrl+L", "Retorna a tela de login");
    menuFile->AppendSeparator();
    menuFile->Append(wxID_EXIT, "&Sair\tCtrl+Q", wxString::FromUTF8("Fecha a aplicação"));

    sizer = new wxBoxSizer(wxVERTICAL);
    SetSizer(sizer);
    Center();

    SetUI(ID_LOADING_FORM);
}

void inventory::AppView::ShowMessage(const wxString& msg, int timeout)
{
    if (authForm || loadingForm)
        form->ShowMessage(msg);
    else {
        GetStatusBar()->SetLabel(msg);
        if (timeout >= 1000) {
            timerStatusbar.Start(timeout, wxTIMER_ONE_SHOT);
        }
    }
}
