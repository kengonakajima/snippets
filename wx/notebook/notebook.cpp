/////////////////////////////////////////////////////////////////////////////
// Name:        samples/notebook/notebook.cpp
// Purpose:     a sample demonstrating notebook usage
// Author:      Julian Smart
// Modified by: Dimitri Schoolwerth
// Created:     26/10/98
// RCS-ID:      $Id: notebook.cpp,v 1.38 2005/06/17 14:36:23 VZ Exp $
// Copyright:   (c) 1998-2002 wxWidgets team
// License:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".


//IMPLEMENT_APP(MyApp)

// Define a new application

#include "wx/wx.h"
#include "wx/button.h"
#include "wx/listctrl.h"

//てきとうだ。うつくしくせよ。
#define FRAME_WIDTH 150
#define FRAME_HEIGHT 350
#define BUTTON_NUM 10


class MyApp : public wxApp
{
public:
    bool OnInit();
};


class AddPanel : public wxPanel
{
public:
	AddPanel( wxFrame *frame, int x, int y, int w, int h );
	
private:
	wxListCtrl *m_flist; // 友達リスト、複数選択可能
	wxTextCtrl *m_text;  // 友達が知っていることのリスト。	
};

class AddFrame : public wxFrame
{
public:
	AddFrame( void );
private:
	AddPanel *m_panel;
};

class MainPanel : public wxPanel
{
public:
	MainPanel( wxFrame *frame, int x, int y, int w, int h );
	
private:
	wxTextCtrl *m_sb;
	wxButton *m_listb[BUTTON_NUM];
	wxStaticText *m_sinfo; 
	wxButton *m_addf;

	
};




class MainFrame : public wxFrame
{
public:

	MainFrame( const wxString & title,  const wxPoint& pos, const wxSize &size );

	void OnQuit( wxCommandEvent& event );
	void OnAbout( wxCommandEvent& event );
	void MainFrame::OnAddButton( wxCommandEvent& event );
	DECLARE_EVENT_TABLE()

private:
	MainPanel *m_panel;
	
};




enum 
{
ID_Quit = 1,
ID_About ,

ID_AddButton = 100,
ID_SearchBox,
ID_SearchInfo,

ID_Word0 = 1000,


};

BEGIN_EVENT_TABLE( MainFrame, wxFrame )
EVT_MENU( ID_Quit, MainFrame::OnQuit )
EVT_MENU( ID_About, MainFrame::OnAbout )

EVT_BUTTON( ID_AddButton, MainFrame::OnAddButton )

END_EVENT_TABLE()


IMPLEMENT_APP( MyApp )

bool MyApp::OnInit()
{
		MainFrame  *frame = new MainFrame(  
			"YKWK", wxPoint( 50,50 ), wxSize(FRAME_WIDTH,FRAME_HEIGHT) );
		frame->Show(TRUE);
		SetTopWindow(frame);
		return TRUE;

}

MainFrame::MainFrame( const wxString & title,  const wxPoint& pos, const wxSize &size ) 
: wxFrame( (wxFrame*) NULL, -1,  title, pos, size )
{
	wxMenu * menuFile = new wxMenu;
	menuFile->Append( ID_About, "&About..." );
	menuFile->AppendSeparator();
	menuFile->Append( ID_Quit, "E&xit" );

	wxMenuBar *menuBar = new wxMenuBar;
	menuBar->Append( menuFile, "&File" );
	SetMenuBar( menuBar );
	CreateStatusBar();
	SetStatusText( "YKWK : 1234pt " );


	//init panel
	m_panel = new MainPanel( this, 0,0, FRAME_WIDTH, FRAME_HEIGHT );
}

MainPanel::MainPanel( wxFrame *frame, int x, int y, int w, int h )
       : wxPanel( frame, wxID_ANY, wxPoint(x, y), wxSize(w, h) )
{	


	// 検索窓
	m_sb = new wxTextCtrl( this, ID_SearchBox, _T(""),
                             wxDefaultPosition, wxDefaultSize,
                             wxTE_PROCESS_ENTER);
    m_sb->SetForegroundColour(*wxBLUE);
    m_sb->SetBackgroundColour(*wxLIGHT_GREY);
    m_sb->SetInsertionPoint(0);
	
	// 検索の情報
	m_sinfo = new wxStaticText( 
		this, ID_SearchInfo, 
		"85 Knows 59 bytes"  ,
		wxDefaultPosition, wxDefaultSize, 
		0, "sinfo" 
	);
	
	// 友達（の情報)を追加する
	m_addf = new wxButton( this, ID_AddButton, "Add", wxDefaultPosition, wxDefaultSize,0 );
		
	
	// 答えるボタン
	int i;
	for(i=0;i<BUTTON_NUM;i++){
		wxString s = "word";
		s += i;
		m_listb[i] = new wxButton( this, ID_Word0 + i, s,
			wxDefaultPosition, wxDefaultSize, 0 );
	}

	// 
	wxBoxSizer *topSizer = new wxBoxSizer( wxVERTICAL );
	for(i=0;i<BUTTON_NUM;i++){	
		topSizer->Add( m_listb[i], 0, wxALL | wxBOTTOM | wxEXPAND, 1 );
	}
	topSizer->Add( m_sb, 0, wxALL | wxEXPAND, 1 );	
	topSizer->Add( m_sinfo, 0, wxALL | wxEXPAND, 1 );		
	topSizer->Add( m_addf, 0, wxALL | wxEXPAND, 1 );			
	
	SetAutoLayout(TRUE);
	SetSizer(topSizer);

}

void MainFrame::OnAddButton( wxCommandEvent& WXUNUSED(event))
{
	
	wxMessageBox( "Hoge", "Message Box", wxOK, this );
}

void MainFrame::OnQuit( wxCommandEvent& WXUNUSED(event))
{
	Close(TRUE);
}

void MainFrame::OnAbout( wxCommandEvent& WXUNUSED(event))
{
	wxMessageBox( "You Know Who Knows",
	"About YKWH", wxOK | wxICON_INFORMATION, this );

}

