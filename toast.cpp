#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#pragma warnging(push)
#pragma warning(disable : 4996)
#include <wx/wx.h>
#include <wx/cmdline.h>
#pragma warnging(pop)

void
DrawText(wxPaintDC& dc, wxString text, wxPoint pt)
{
  wxFont font(12, wxFONTFAMILY_SWISS, wxNORMAL, wxBOLD);
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
  Frame(wxPoint p, wxSize s)
    : wxFrame(nullptr, wxID_ANY, "Example Title", p, s, wxDEFAULT_FRAME_STYLE)
  {
     auto panel = new Panel(this);
    //// auto panel = new wxPanel(this, size=GetClientRect().GetSize());

     auto* top = new wxBoxSizer(wxVERTICAL);
     top->Add(panel, 1, wxEXPAND, 0);
     this->SetSizer(top);

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

static const wxCmdLineEntryDesc g_cmdLineDesc[] =
{
     { wxCMD_LINE_SWITCH, "h", "help", "displays help on the command line parameters",
          wxCMD_LINE_VAL_NONE, wxCMD_LINE_OPTION_HELP },
     { wxCMD_LINE_SWITCH, "t", "test", "test switch",
          wxCMD_LINE_VAL_NONE, wxCMD_LINE_OPTION_MANDATORY  },
     { wxCMD_LINE_SWITCH, "s", "silent", "disables the GUI" },

     { wxCMD_LINE_NONE }
};

class App : public wxApp
{
public:
  Frame* frame_ = nullptr;
  App(){};
  ~App(){};

  void OnInitCmdLine(wxCmdLineParser& parser)
  {
      parser.SetDesc(g_cmdLineDesc);
      // must refuse '/' as parameter starter or cannot use "/path" style paths
      parser.SetSwitchChars(wxT("-"));
  }

  bool OnCmdLineParsed(wxCmdLineParser& parser)
  {
      //silent_mode = parser.Found(wxT("s"));
      wxMessageBox("parsing args...");

      // to get at your unnamed parameters use
      wxArrayString args;
      for (int i = 0; i < parser.GetParamCount(); i++)
      {
          args.Add(parser.GetParam(i));
      }

      // and other command line parameters
      for (auto& arg : args) {
          std::cout << arg << std::endl;
          wxMessageBox(arg);
      }

      // then do what you need with them.


      return true;
  }
  virtual bool OnInit()
  {
      auto size = wxSize(300, 150);
      frame_ = new Frame(CalcOrigin(size), size);
      frame_->Show();
      return true;
  }
  // virtual int OnExit();
};


wxIMPLEMENT_APP(App);
