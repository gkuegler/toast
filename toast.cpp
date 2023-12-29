// Lean and mean prevents winsock version clash
// between wxWidgets and Windows.h.
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#include <string>
#include <vector>

// Fix wxWidgets and windows linking.
#pragma comment(lib, "comctl32")
#pragma comment(lib, "Rpcrt4")

// wxWidgets library uses unsecure 'strcpy' functions.
// Suppress this warning.
#pragma warning(push)
#pragma warning(disable : 4996)
#include <wx/cmdline.h>
#include <wx/wx.h>
#pragma warning(pop)

#include "spdlog/sinks/basic_file_sink.h"
#include <spdlog/spdlog.h>

class Frame : public wxFrame
{
public:
  wxStaticText* text_title;
  wxStaticText* body;
  wxTimer* timer;

  Frame(wxPoint p, wxSize s, wxString message, wxString title)
    : wxFrame(nullptr,
              wxID_ANY,
              "Toast - Hello World!",
              p,
              s,
              wxCLIP_CHILDREN | wxSTAY_ON_TOP | wxFRAME_NO_TASKBAR |
                wxFRAME_TOOL_WINDOW)
  {
    auto panel = new wxPanel(this);
    const auto COLOR_WHITE = wxColour(255, 255, 255);
    const auto COLOR_YELLOW = wxColour(245, 239, 118);
    const auto COLOR_BLACK = wxColour(0, 0, 0);

    panel->SetSize(GetClientSize());
    panel->SetBackgroundColour(COLOR_BLACK);

    timer = new wxTimer(this);

// Set visibility time.
// Increase the time spent visible for debug builds to evaluate appearance.
#ifndef _DEBUG
    timer->StartOnce(1500);
#else
    timer->StartOnce(8000);
#endif

    // Construct title text.
    text_title = new wxStaticText(
      panel, wxID_ANY, title, wxDefaultPosition, wxDefaultSize, wxALIGN_LEFT);
    text_title->SetBackgroundColour(COLOR_BLACK);
    text_title->SetForegroundColour(COLOR_YELLOW);

    // Set title font.
    auto font_title = text_title->GetFont();
    font_title.SetPointSize(20);
    font_title.SetFaceName("Arial");
    text_title->SetFont(font_title);

    // Construct body message.
    body = new wxStaticText(
      panel, wxID_ANY, message, wxDefaultPosition, wxDefaultSize, wxALIGN_LEFT);
    body->SetBackgroundColour(COLOR_BLACK);
    body->SetForegroundColour(COLOR_WHITE);

    // Set body Font.
    auto font_body = body->GetFont();
    font_body.SetPointSize(18);
    font_body.SetFaceName("Arial");
    body->SetFont(font_body);

    // Bind the mouse events to close the toast when clicked
    // and timer event to close the toast after a delay period.
    text_title->Bind(wxEVT_LEFT_UP, &Frame::OnClick, this);
    body->Bind(wxEVT_LEFT_UP, &Frame::OnClick, this);
    Bind(wxEVT_TIMER, &Frame::OnTimer, this);

    auto sz_contents = new wxBoxSizer(wxVERTICAL);
    sz_contents->Add(text_title, 0, wxEXPAND | wxBOTTOM, 5);
    sz_contents->Add(body, 1, wxEXPAND, 0);

    // Set margin for text contents.
    auto sz_margin = new wxBoxSizer(wxVERTICAL);
    sz_margin->Add(sz_contents, 1, wxEXPAND | wxALL, 5);
    panel->SetSizer(sz_margin);

    // Layout the panel elements.
    panel->Layout();
  };

  void OnClick(wxMouseEvent& event) { Close(true); }
  void OnTimer(wxTimerEvent& event) { Close(true); }
};

//////////////////////////////////////////////////////////////////////
//                         Main Application                         //
//////////////////////////////////////////////////////////////////////

// Calculate top left origin coordinates for the toast to appear in the bottom
// left corner of the main display.
wxPoint
CalcOrigin(wxSize size)
{
  const int desktop_width = GetSystemMetrics(SM_CXMAXIMIZED);
  const int desktop_height = GetSystemMetrics(SM_CYMAXIMIZED);
  const int padding = 30;

  const int x = desktop_width - size.x - padding;
  const int y = desktop_height - size.y - padding;

  return wxPoint(x, y);
}

class App : public wxApp
{
public:
  Frame* frame = nullptr;
  App(){};
  ~App(){};

  virtual bool OnInit()
  {
    // Set up logging.
    std::vector<spdlog::sink_ptr> sinks = {
      std::make_shared<spdlog::sinks::basic_file_sink_mt>("toast.log", true),
    };

    auto logger =
      std::make_shared<spdlog::logger>("main", begin(sinks), end(sinks));
    logger->set_level(spdlog::level::debug);

    // Pattern description.
    // "[HH:MM:SS][name][level] msg"
    logger->set_pattern("[%T][%n][%l] %v");

    spdlog::set_default_logger(std::move(logger));
    spdlog::flush_every(std::chrono::seconds(2));
    spdlog::debug("Logging to file!");

    // Parse commandline arguments.
    auto arg_count = wxTheApp->argc;
    spdlog::debug("argument count: {}", arg_count);

// Use default arguments for debug builds.
#ifndef _DEBUG
    if (arg_count < 3) {
      wxLogError("Too few arguments.");
      return false;
    }

    const auto message = wxTheApp->argv[1];
    const auto title = wxTheApp->argv[2];
#else
    const auto message =
      wxString("A very long message that hopefully wraps successfully.");
    const auto title = wxString("A Title!");
#endif

    // TODO: dynamically size frame based on message length.
    const auto size = wxSize(336, 192);
    frame = new Frame(CalcOrigin(size), size, message, title);
    frame->Show();

    return true;
  }

  // No cleanup code necessary on exit.
  // virtual int OnExit();
};

wxIMPLEMENT_APP(App);
