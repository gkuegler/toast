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

std::string convert(wxString text) {
    return std::string(text.c_str());
}

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
  wxStaticText* text_title;
  wxStaticText* body;
  wxTimer* timer;

  Frame(wxPoint p, wxSize s, wxString message, wxString title)
    //: wxFrame(nullptr, wxID_ANY, "Toast - Hello World!", p, s, wxDEFAULT_FRAME_STYLE| wxSTAY_ON_TOP)
    : wxFrame(nullptr, wxID_ANY, "Toast - Hello World!", p, s, wxCLIP_CHILDREN|wxSTAY_ON_TOP| wxFRAME_NO_TASKBAR)
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

    timer = new wxTimer(this);
#ifndef _DEBUG
    timer->StartOnce(1500);
#else
    timer->StartOnce(5000);
#endif

    
    //title = new wxStaticText(panel, wxALIGN_LEFT);
    text_title = new wxStaticText(panel, wxID_ANY, title, wxDefaultPosition, wxDefaultSize, wxALIGN_LEFT);
    text_title-> SetBackgroundColour(COLOR_BLACK);
    text_title-> SetForegroundColour(COLOR_YELLOW);
    //Setting the font of the title.
    auto font_title = text_title->GetFont();
    font_title.SetPointSize(20);
    font_title.SetFaceName("Arial");
    text_title->SetFont(font_title);

    body = new wxStaticText(panel, wxID_ANY, message, wxDefaultPosition, wxDefaultSize, wxALIGN_LEFT);
    body -> SetBackgroundColour(COLOR_BLACK);
    body -> SetForegroundColour(COLOR_WHITE);
    //Setting the font of the body.
    auto font_body = body->GetFont();
    font_body.SetPointSize(18);
    font_body.SetFaceName("Arial");
    body->SetFont(font_body);


    // May not be necessary to bind this to the panel
    // if both the title and body controls take up the entire
    // client area of the panel.
    // panel.
    text_title->Bind(wxEVT_LEFT_UP, &Frame::OnMouse, this);
    body->Bind(wxEVT_LEFT_UP, &Frame::OnMouse, this);
    Bind(wxEVT_TIMER, &Frame::OnTimer, this);

    auto sz_main = new wxBoxSizer(wxVERTICAL);
    sz_main -> Add(text_title, 0, wxEXPAND | wxBOTTOM, 5);
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

  void OnMouse(wxMouseEvent& event) { Close(true); }
  void OnTimer(wxTimerEvent& event) { Close(true); }
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


class App : public wxApp
{
public:
  Frame* frame_ = nullptr;
  App(){};
  ~App(){};
 
  virtual bool OnInit()
  {
      std::vector<spdlog::sink_ptr> sinks = {
        std::make_shared<spdlog::sinks::basic_file_sink_mt>("app.log", true),
      };

      auto logger_ = std::make_shared<spdlog::logger>("main", begin(sinks), end(sinks));
      logger_->set_level(spdlog::level::debug);
      logger_->set_pattern("[%T][%n][%l] %v"); // "[HH:MM:SS][logger_name][level] msg"

      spdlog::set_default_logger(std::move(logger_));
      spdlog::flush_every(std::chrono::seconds(2));
      spdlog::debug("Logging to file!");


      auto argument_count = wxTheApp->argc;
      spdlog::debug("argument count: {}", argument_count);

#ifndef _DEBUG
      if (argument_count < 3)
      {
          wxLogError("Too few arguments.");
          return false;
      }

      auto message = wxTheApp->argv[1];
      auto title = wxTheApp->argv[2];      
#else
      auto message = wxString("A very long message that hopefully wraps around successfully.");
      auto title = wxString("A Title!");
#endif

      auto size = wxSize(336, 192);
      frame_ = new Frame(CalcOrigin(size), size, message, title);
      frame_->Show();

      return true;
  }
  // virtual int OnExit();
};


wxIMPLEMENT_APP(App);
