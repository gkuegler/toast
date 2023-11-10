#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#pragma comment(lib, "comctl32")
#pragma comment(lib, "Rpcrt4")

#pragma warning(push)
#pragma warning(disable : 4996)
#include <wx/wx.h>
#include <wx/cmdline.h>
#pragma warning(pop)

#include <spdlog/spdlog.h>
#include "spdlog/sinks/basic_file_sink.h"

void
DrawText(wxPaintDC& dc, wxString text, wxPoint pt)
{
  //wxFont font(12, wxFONTFAMILY_SWISS, wxNORMAL, wxBOLD);
  wxFont font = wxFontInfo(12).FaceName("Helvetica");
  dc.SetFont(font);
   //dc.SetBackgroundMode(wxTRANSPARENT);
  dc.SetTextForeground(*wxBLACK);
  dc.SetTextBackground(*wxWHITE);
  dc.DrawText(text, pt);
}

class Panel : public wxPanel {
public:
    Panel(wxWindow* parent) : wxPanel(parent) {
        // nothing to construct
        Bind(wxEVT_PAINT, &Panel::PaintEvent, this);
    }
    void PaintEvent(wxPaintEvent& event)
    {
        // Update();
        const constexpr int border = 0;
        wxPaintDC dc(this);
        auto s = GetClientRect().GetSize();

        DrawText(dc, wxString("Hello World!"), wxPoint(0, 0));
        event.Skip();
    }

    // this is needed to update the whole image on a reize
    void OnSize(wxSizeEvent& event)
    {
        Refresh();
        // skip the event.
        event.Skip();
    }
};

class Frame : public wxFrame
{
public:
  wxStaticText* title;
  wxStaticText* body;

  Frame(wxPoint p, wxSize s)
    : wxFrame(nullptr, wxID_ANY, "Toast - Hello World!", p, s, wxDEFAULT_FRAME_STYLE)
  {
    auto panel = new wxPanel(this);
    //// auto panel = new wxPanel(this, size=GetClientRect().GetSize());
    auto COLOR_WHITE = wxColour(255, 255, 255);
    // COLOR_YELLOW = wxColour(250, 247, 190)
    // COLOR_YELLOW = wxColour(245, 240, 140)
    auto COLOR_YELLOW = wxColour(245, 239, 118);
    auto COLOR_BLACK = wxColour(0, 0, 0);
    
    panel -> SetSize(GetClientSize());
    panel->SetBackgroundColour(COLOR_BLACK);

    // timer = wxTimer(self)
    // timer.StartOnce(milliseconds=2000)
    // timer.StartOnce(milliseconds=5000)

    // auto font = wxFont(18, wxFONTFAMILY_DEFAULT, 0, 90, false, "Consolas", wxFONTENCODING_DEFAULT);

    
    //title = new wxStaticText(panel, wxALIGN_LEFT);
    title = new wxStaticText(panel, wxID_ANY, "xxx", wxDefaultPosition, wxDefaultSize, wxALIGN_LEFT);
    // title -> SetFont(font);
    title -> SetBackgroundColour(COLOR_BLACK);
    title -> SetForegroundColour(COLOR_YELLOW);
    title -> SetLabel("hello");

    body = new wxStaticText(panel, wxID_ANY, "xxx", wxDefaultPosition, wxDefaultSize, wxALIGN_LEFT);
    // body -> SetFont(font);
    body -> SetBackgroundColour(COLOR_BLACK);
    body -> SetForegroundColour(COLOR_WHITE);
    body -> SetLabel("world");


    // May not be necessary to bind this to the panel
    // if both the title and body controls take up the entire
    // client area of the panel.
    // panel.Bind(wxEVT_LEFT_UP, close)

    // Bind( wxEVT_COMMAND_MENU_SELECTED, &MyFrameHandler::OnFrameExit,
        // &myFrameHandler, wxID_EXIT );
    // title -> Bind(wxEVT_LEFT_UP, OnClose);
    // body -> Bind(wxEVT_LEFT_UP, OnClose);
    // Bind(wxEVT_TIMER, OnClose);

    auto sz_main = new wxBoxSizer(wxVERTICAL);
    sz_main -> Add(title, 0, wxEXPAND | wxBOTTOM, 5);
    sz_main -> Add(body, 1, wxEXPAND, 0);

    auto sub_sizer = new wxBoxSizer(wxVERTICAL);
    sub_sizer -> Add(sz_main, 1, wxEXPAND | wxALL, 5);
    panel -> SetSizer(sub_sizer);
    
    // Also a weird problem where the panel doesn't
    // fll to the size of the frame.
    panel -> Layout();

    // Fit not required for panel to expand to frame
    // panel->GetSizer()->Fit(this);
    // panel->Fit();
    // RedrawWindow();
    Refresh();
  };

  void OnClose(wxCommandEvent& event) { Close(true); }
};

//////////////////////////////////////////////////////////////////////
//                         Main Application                         //
//////////////////////////////////////////////////////////////////////

// center app on main display
// returns top left corner required to center app on main display
wxPoint
CalcOrigin(wxSize size)
{
  int desktop_width = GetSystemMetrics(SM_CXMAXIMIZED);
  int desktop_height = GetSystemMetrics(SM_CYMAXIMIZED);
  int padding = 30;

  int x = desktop_width - size.x - padding;
  int y = desktop_height - size.y - padding;

  return wxPoint(x, y);
}


//static const wxCmdLineEntryDesc g_cmdLineDesc[] =
//{
//     { wxCMD_LINE_SWITCH, wxT("h"), wxT("help"), wxT("displays help on the command line parameters"),
//          wxCMD_LINE_VAL_NONE, wxCMD_LINE_OPTION_HELP },
//     { wxCMD_LINE_SWITCH, wxT("t"), wxT("test"), wxT("test switch"),
//          wxCMD_LINE_VAL_NONE, wxCMD_LINE_OPTION_MANDATORY  },
//     { wxCMD_LINE_SWITCH, wxT("s"), wxT("silent"), wxT("disables the GUI") },
//
//     { wxCMD_LINE_NONE }
//};

//static const wxCmdLineEntryDesc g_cmdLineDesc[] =
//{
//     { wxCMD_LINE_SWITCH, "h", "help", "displays help on the command line parameters",
//          wxCMD_LINE_VAL_NONE, wxCMD_LINE_OPTION_HELP },
//     { wxCMD_LINE_SWITCH, "t", "test", "test switch",
//          wxCMD_LINE_VAL_NONE, wxCMD_LINE_OPTION_MANDATORY  },
//     { wxCMD_LINE_SWITCH, "s", "silent", "disables the GUI" },
//
//     { wxCMD_LINE_NONE }
//};

class App : public wxApp
{
public:
  Frame* frame_ = nullptr;
  App(){};
  ~App(){};

  //void OnInitCmdLine(wxCmdLineParser& parser)
  //{
  //    parser.SetDesc(g_cmdLineDesc);
  //    // must refuse '/' as parameter starter or cannot use "/path" style paths
  //    parser.SetSwitchChars(wxT("-"));
  //}

  //bool OnCmdLineParsed(wxCmdLineParser& parser)
  //{

  //    spdlog::debug("parsing args...");
  //    bool silent_mode = parser.Found(wxT("s"));
  //    spdlog::debug("silent mode found: {}", silent_mode);


  //    // to get at your unnamed parameters use
  //    wxArrayString args;
  //    for (int i = 0; i < parser.GetParamCount(); i++)
  //    {
  //        args.Add(parser.GetParam(i));
  //    }

  //    // and other command line parameters
  //    for (auto& arg : args) {
  //        std::cout << arg << std::endl;
  //        //spdlog::debug("argument: {}", arg.c_str());
  //    }

  //    // then do what you need with them.


  //    return true;
  //}
  virtual bool OnInit()
  {
      auto logger = spdlog::basic_logger_mt("basic_logger", "app.log");
      logger->set_level(spdlog::level::debug);
      logger->debug("Logging to file!");
      spdlog::set_default_logger(std::move(logger));
      spdlog::flush_every(std::chrono::seconds(2));
      auto size = wxSize(300, 150);
      frame_ = new Frame(CalcOrigin(size), size);
      frame_->Show();
      return true;
  }
  // virtual int OnExit();
};


wxIMPLEMENT_APP(App);
