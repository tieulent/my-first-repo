/***************************************************************************
 *   Copyright (C) 2004 by Raphael Tieulent                                *
 *   tieulent@in2p3.fr                                                     *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#include <TROOT.h>
#include <TStyle.h>
#include <TRint.h>
#include <TVirtualX.h>
#include <TEnv.h>
#include <KeySymbols.h>
#include <TError.h>

#include <TFile.h>
#include <TTree.h>
#include <TFrame.h>
#include <TH1.h>
#include <TF1.h>

#include <TGMenu.h>
#include <TGFileDialog.h>
#include <TGTextView.h>
#include <TRootEmbeddedCanvas.h>
#include <TCanvas.h>
#include <TRandom.h>
#include <TSystem.h>
#include <TRootHelpDialog.h>
#include <TGStatusBar.h>
#include <TBrowser.h>

#include <TApplication.h>
#include <TGDockableFrame.h>
#include <TGCanvas.h>
#include <TGTextEntry.h>
#include <TGButton.h>
#include <TGColorSelect.h>
#include <TGMenu.h>
#include <TGFileDialog.h>
#include <TGFontDialog.h>
#include <TGMsgBox.h>
#include <TGToolBar.h>
#include <KeySymbols.h>
#include <TGSplitter.h>
#include <TGTab.h>
#include <TView.h>
#include <TGLabel.h>
#include <TGNumberEntry.h>

#include "GMSGui.h"
#include "GMSSettings.h"
#include "GMSAnalyser.h"

#include <iostream>
using namespace std;

//ClassImp(GMSGui)

GMSGui * gGMSGui;

enum CommandIdentifiers {
   M_FILE_OPEN,
	M_FILE_CLOSE,
   M_FILE_SAVE,
   M_FILE_SAVEAS,
   M_FILE_PRINT,
   M_FILE_PRINTSETUP,
   M_FILE_EXIT,
	
	M_ZOOM_PLUS,
	M_ZOOM_MOINS,
	M_ZOOM_PLUS2,
	M_ZOOM_MOINS2,
	M_CLEAR,
   
	M_CONF_SETTINGS,

	M_VIEW_TOOLBAR,
   M_VIEW_ENBL_DOCK,
   M_VIEW_ENBL_HIDE,
   M_VIEW_DOCK,
   M_VIEW_UNDOCK,

   M_HELP_CONTENTS,
   M_HELP_SEARCH,
   M_HELP_ABOUT,

	M_INSPECT_BROWSER,
	M_SHOW_3D,

   M_NEW_REMOVEMENU,

	B_START,
	B_STOP,
	
	B_RUNNUMBER,
	B_EVENTNUMBER,
	
   VId1,
   HId1,
   VId2,
   HId2,

   VSId1,
   HSId1,
   VSId2,
   HSId2,

   ColorSel
};

const char *filetypes[] = { "All files",     "*",
                            "ROOT files",    "*.root",
                            "ROOT macros",   "*.C",
                            0,               0 };
									 

const char *xpm_names[] = {
    "open.xpm",
    "save.xpm",
    "",
    "settings.xpm",
    "",
    "infos.xpm",
    "view3d.xpm",
    "",
    "browser.xpm",
    "",
    "help.xpm",
    "license.xpm",
    "about.xpm",
    "",
    "quit.xpm",
    0
};			 

ToolBarData_t tb_data[] = {
  { "", "Open Root  file",     kFALSE, M_FILE_OPEN,        NULL },
  { "", "Save in Root file",  kFALSE, M_FILE_SAVEAS,      NULL },
  { "",              0,             0,      -1,                 NULL },
  { "", "Analysis settings",           kFALSE, M_CONF_SETTINGS,     NULL },
  { "",              0,             0,      -1,                 NULL },
  { "", "Infos on current event",   kFALSE, -1,       NULL },
  { "", "Open 3D viewer",           kFALSE, M_SHOW_3D,          NULL },
  { "",              0,             0,      -1,                 NULL },
  { "", "Start Root browser",       kFALSE, M_INSPECT_BROWSER,  NULL },
  { "",              0,             0,      -1,                 NULL },
  { "", "GMS Analyser help",          kFALSE, -1,  NULL },
  { "", "Display license",          kFALSE, -1,     NULL },
  { "", "About GMS Analyser",         kFALSE, M_HELP_ABOUT,       NULL },
  { "",              0,             0,      -1,                 NULL },
  { "", "Exit Application",         kFALSE, M_FILE_EXIT,        NULL },
  { NULL,            NULL,          0,      0,                  NULL }
};

									 
/// Create GMS Gui main frame. A TGMainFrame is a top level window.
GMSGui::GMSGui(const TGWindow *p, UInt_t w, UInt_t h)
      : TGMainFrame(p, w, h)
{
	//---- Menu Bar
		  MakeMenuBarFrame();
	
	//---- toolbar
	
	MakeToolBar();
     fL5 = new TGLayoutHints(kLHintsTop | kLHintsLeft | kLHintsExpandX | kLHintsExpandY,
                            2, 2, 2, 2);
    
	  // CREATE MAIN FRAME
    fMainFrame = new TGCompositeFrame(this, 100, 100, kHorizontalFrame | kRaisedFrame);

    TGVerticalFrame *fV1 = new TGVerticalFrame(fMainFrame, 200, 10, kSunkenFrame | kFixedWidth);
    TGVerticalFrame *fV2 = new TGVerticalFrame(fMainFrame, 10, 10, kSunkenFrame);

    TGLayoutHints *lo;

    lo = new TGLayoutHints(kLHintsLeft | kLHintsExpandY,2,0,2,2);
    fMainFrame->AddFrame(fV1, lo);

    TGVSplitter *splitter = new TGVSplitter(fMainFrame, 5);
    splitter->SetFrame(fV1, kTRUE);
    lo = new TGLayoutHints(kLHintsLeft | kLHintsExpandY, 0, 0 ,0, 0);
    fMainFrame->AddFrame(splitter, lo);

    lo = new TGLayoutHints(kLHintsRight | kLHintsExpandX | kLHintsExpandY,0,2,2,2);
    fMainFrame->AddFrame(fV2, lo);  
	 
	 
	 
    // Create Selection frame (i.e. with buttons and geometry selection widgets)
    fSelectionFrame = new TGCompositeFrame(fV1, 100,100, kVerticalFrame);
    // create button frame
    fButtonFrame = new TGCompositeFrame (fSelectionFrame, 100,100,kVerticalFrame);
	     // Create Layout hints
    fButtonLayout = new TGLayoutHints(kLHintsTop | kLHintsLeft | kLHintsExpandX, 5, 2, 2, 2);

    // Create Event Buttons
    fStartButton = new TGTextButton(fButtonFrame, "Start Analysis", B_START);
    fStartButton->Associate(this);
    fStartButton->SetToolTipText("Start new analysis");
    fButtonFrame->AddFrame(fStartButton, fButtonLayout);
//     fStopButton = new TGTextButton(fButtonFrame, "&Interrupt Analysis", B_STOP);
//     fStopButton->Associate(this);
//     fStopButton->SetToolTipText("Interrupts the current analysis");
//     fButtonFrame->AddFrame(fStopButton, fButtonLayout);
    
    // Create Runnumber frame 
    fRunFrame = new TGCompositeFrame(fV1, 100, 100, kHorizontalFrame);
    fRunLabel = new TGLabel(fRunFrame, "Run Number");
    fRunFrame->AddFrame(fRunLabel, new TGLayoutHints(kLHintsLeft| kLHintsExpandX,5,2,5,0));
	fRunNumber = new TGNumberEntry(fRunFrame, gGMSAnalyser->GetRunNumber(), 5, B_RUNNUMBER,
                                             (TGNumberFormat::EStyle) 0);
	fRunNumber->SetLimits(TGNumberEntry::kNELLimitMin,1);
    fRunNumber->Associate(this);
   	fRunFrame->AddFrame(fRunNumber, new TGLayoutHints(kLHintsRight,0,2,2,2));

    // Create Number of event frame 
    fEventFrame = new TGCompositeFrame(fV1, 100, 100, kHorizontalFrame);
    fEventLabel = new TGLabel(fEventFrame, "Number of Event");
    fEventFrame->AddFrame(fEventLabel, new TGLayoutHints(kLHintsLeft| kLHintsExpandX,5,2,5,0));
	fEventNumber = new TGNumberEntry(fEventFrame, gGMSAnalyser->GetNEvent(), 8, B_EVENTNUMBER,
                                             (TGNumberFormat::EStyle) 0);
	fEventNumber->SetLimits(TGNumberEntry::kNELLimitMin,1);
    fEventNumber->Associate(this);
   	fEventFrame->AddFrame(fEventNumber, new TGLayoutHints(kLHintsRight,0,2,2,2));
	if(gGMSAnalyser->GetMode() != GMSAnalyser::kSimulMode) fEventNumber->SetState(kFALSE);


    fStartButton->Resize(150,GetDefaultHeight());
   // fStopButton->Resize(150,GetDefaultHeight());

    //fStopButton->SetState(kButtonDisabled);
	 
	 lo = new TGLayoutHints(kLHintsTop | kLHintsCenterX | kLHintsExpandX, 2, 5, 1, 2);
    fSelectionFrame->AddFrame(fButtonFrame, lo);
    fSelectionFrame->AddFrame(fRunFrame, lo);
    fSelectionFrame->AddFrame(fEventFrame, lo);

    lo = new TGLayoutHints(kLHintsExpandX | kLHintsExpandY);
    fV1->AddFrame(fSelectionFrame, lo);
	 
	  
	// ----------------------------------------- 
	 // Create Display frame
	// ----------------------------------------- 
    fDisplayFrame = new TGTab(fV2, 580, 360);

	   // Create text display Tab
	   TGCompositeFrame *tFrame = fDisplayFrame->AddTab("Standard Output");
	   fTextView = new TGTextView(tFrame, 300, 100, kSunkenFrame | kDoubleBorder);
	   tFrame->AddFrame(fTextView, fL5);

    // Create Layout hints
    fClearButtonLayout = new TGLayoutHints(kLHintsBottom | kLHintsLeft | kLHintsExpandX, 5, 2, 2, 2);

    fHFrame = new TGHorizontalFrame(tFrame,0,0,0);
    tFrame->AddFrame(fHFrame, new TGLayoutHints(kLHintsBottom | kLHintsLeft | kLHintsExpandX, 5, 5, 5, 5));	
    
	fClearButton = new TGTextButton(fHFrame, "&Clear", M_CLEAR);
    fClearButton->Associate(this);
    fClearButton->SetToolTipText("CLear Text output window");
    fHFrame->AddFrame(fClearButton, fClearButtonLayout);


	    lo = new TGLayoutHints(kLHintsExpandX | kLHintsExpandY);
	    fV2->AddFrame(fDisplayFrame, lo);

	// ----------------------------------------- 
    // Create Display Canvas Tab (where the actual main event is displayed)
	// ----------------------------------------- 
    tFrame = fDisplayFrame->AddTab("Geometry Viewer");

    // Create Layout hints
    fZoomButtonsLayout = new TGLayoutHints(kLHintsBottom | kLHintsLeft | kLHintsExpandX, 5, 2, 2, 2);

    fHFrame = new TGHorizontalFrame(tFrame,0,0,0);
    tFrame->AddFrame(fHFrame, new TGLayoutHints(kLHintsBottom | kLHintsLeft | kLHintsExpandX, 5, 5, 5, 5));
    // Create Zoom Buttons
    fZoomPlusButton = new TGTextButton(fHFrame, "&Zoom Forward", M_ZOOM_PLUS);
    fZoomPlusButton->Associate(this);
    fZoomPlusButton->SetToolTipText("Zoom forward event view");
    fHFrame->AddFrame(fZoomPlusButton, fZoomButtonsLayout);
    fZoomMoinsButton = new TGTextButton(fHFrame, "Zoom &Backward", M_ZOOM_MOINS);
    fZoomMoinsButton->Associate(this);
    fZoomMoinsButton->SetToolTipText("Zoom backward event view");
    fHFrame->AddFrame(fZoomMoinsButton, fZoomButtonsLayout);

    fEmbeddedCanvas = new TRootEmbeddedCanvas("fEmbeddedCanvas", tFrame, 580, 360);
    tFrame->AddFrame(fEmbeddedCanvas, fL5);
    fEmbeddedCanvas->GetCanvas()->SetBorderMode(0);
    cA = fEmbeddedCanvas->GetCanvas();


    // Create Display Canvas Tab (where the histogram is displayed)
    TGCompositeFrame *tFrame3 = fDisplayFrame->AddTab("Histograms");

    fEmbeddedCanvas3 = new TRootEmbeddedCanvas("fEmbeddedCanvas3", tFrame3, 580, 360);
    tFrame3->AddFrame(fEmbeddedCanvas3, fL5);
    fEmbeddedCanvas3->GetCanvas()->SetBorderMode(0);
//     cC = fEmbeddedCanvas3->GetCanvas();
//     cC->SetFillColor(10);
//     cC->cd();
//     padC = new TPad("padC","Histogram",0.0,0.0,1.0,1.0,10,3,1);
//     padC->SetFillColor(10);
//     padC->SetBorderMode(0);
//     padC->SetBorderSize(0);
//     padC->Draw();
//     // Creation of histogram for particle's energy loss
//     fHisto_dEdX = new TH1F("Statistics","Energy loss for each particle",100,0,0.025); // Max at 25 MeV
//     fHisto_dEdX->SetFillColor(38);
//     fHisto_dEdX->SetStats(kTRUE);
//     fHisto_dEdX->SetXTitle("Energy Loss [GeV]");
//     fHisto_dEdX->SetLabelFont(42,"X");
//     fHisto_dEdX->SetLabelSize(0.03f, "X");
//     fHisto_dEdX->GetXaxis()->SetTitleFont(42);
//     fHisto_dEdX->SetYTitle("Number");
//     fHisto_dEdX->SetLabelFont(42,"Y");
//     fHisto_dEdX->SetLabelSize(0.03f, "Y");
//     fHisto_dEdX->GetYaxis()->SetTitleFont(42);
// 
//     cC->Update();

    AddFrame(fMainFrame, lo);


//    // Create status frame containing a button and a text entry widget
//    fStatusFrame = new TGCompositeFrame(this, 60, 20, kHorizontalFrame |
//                                                      kSunkenFrame);
// 
//    fTestButton = new TGTextButton(fStatusFrame, "&Open editor...", 150);
//    fTestButton->Associate(this);
//    fTestButton->SetToolTipText("Pops up editor");
//    fStatusFrame->AddFrame(fTestButton, new TGLayoutHints(kLHintsTop |
//                           kLHintsLeft, 2, 0, 2, 2));
//    fTestText = new TGTextEntry(fStatusFrame, new TGTextBuffer(100));
//    fTestText->SetToolTipText("This is a text entry widget");
//    fTestText->Resize(300, fTestText->GetDefaultHeight());
//    fStatusFrame->AddFrame(fTestText, new TGLayoutHints(kLHintsTop | kLHintsLeft,
//                                                        10, 2, 2, 2));
//    Pixel_t yellow;
//    fClient->GetColorByName("yellow", yellow);
//    fColorSel = new TGColorSelect(fStatusFrame, yellow, ColorSel);
//    fStatusFrame->AddFrame(fColorSel, new TGLayoutHints(kLHintsTop |
//                           kLHintsLeft, 2, 0, 2, 2));
// 
//    AddFrame(fStatusFrame, new TGLayoutHints(kLHintsBottom | kLHintsExpandX,
//             0, 0, 1, 0));

   SetWindowName("GMS Analyser");
   MapSubwindows();

   // we need to use GetDefault...() to initialize the layout algorithm...
   Resize();   // resize to default size
   MapWindow();
}

GMSGui::~GMSGui()
{
   // Delete all created widgets.
	delete fRunLabel;
	delete fRunFrame;
	delete fRunNumber;
	delete fEventLabel;
	delete fEventFrame;
	delete fEventNumber;
	delete fStartButton;
	delete fButtonFrame;
	//delete fStopButton ;
	delete fTextView;
	delete fEmbeddedCanvas3;
	delete fEmbeddedCanvas;
	delete fZoomMoinsButton;
	delete fZoomPlusButton;
	delete fHFrame;
	delete fZoomButtonsLayout;
	delete fDisplayFrame;
	delete fMainFrame;
	delete fL5;
	delete fToolBar;
	delete fMenuBar;
	delete fMenuNew2;
	delete fMenuNew1;
	delete fMenuHelp;
	delete fMenuView;
	delete fMenuConf;
	delete fMenuFile;
	delete fMenuBarLayout	  ;
	delete fMenuBarItemLayout ;
	delete fMenuBarHelpLayout ;
	delete fMenuDock;
}

void GMSGui::CloseWindow()
{
   // Got close message for this MainFrame. Terminate the application
   // or returns from the TApplication event loop (depending on the
   // argument specified in TApplication::Run()).
	gGMSAnalyser->SaveSettings();
   	gGMSApplication->Terminate(0);
	//delete gGMSApplication;
	delete gGMSAnalyser;
}

void GMSGui::MakeMenuBarFrame(){
   // Create menubar and popup menus. The hint objects are used to place
   // and group the different menu widgets with respect to eachother.
   fMenuDock = new TGDockableFrame(this);
   AddFrame(fMenuDock, new TGLayoutHints(kLHintsExpandX, 0, 0, 1, 0));
   fMenuDock->SetWindowName("GMSAnalyser Menu");

   fMenuBarLayout 		= new TGLayoutHints(kLHintsTop | kLHintsExpandX);
   fMenuBarItemLayout 	= new TGLayoutHints(kLHintsTop | kLHintsLeft, 0, 4, 0, 0);
   fMenuBarHelpLayout 	= new TGLayoutHints(kLHintsTop | kLHintsRight);

   fMenuFile = new TGPopupMenu(fClient->GetRoot());
   fMenuFile->AddEntry("&Open...", M_FILE_OPEN);
   fMenuFile->AddEntry("&Save", M_FILE_SAVE);
   fMenuFile->AddEntry("S&ave as...", M_FILE_SAVEAS);
   fMenuFile->AddEntry("&Close", M_FILE_CLOSE);
   fMenuFile->AddSeparator();
   fMenuFile->AddEntry("&Print", M_FILE_PRINT);
   fMenuFile->AddEntry("P&rint setup...", M_FILE_PRINTSETUP);
   fMenuFile->AddSeparator();
   fMenuFile->AddEntry("E&xit", M_FILE_EXIT);

   fMenuFile->DisableEntry(M_FILE_SAVEAS);
   fMenuFile->HideEntry(M_FILE_PRINT);


	fMenuConf = new TGPopupMenu(fClient->GetRoot());
   fMenuConf->AddEntry("Analysis Settings", M_CONF_SETTINGS);
	
	
   fMenuView = new TGPopupMenu(gClient->GetRoot());
	fMenuView->AddEntry("&Toolbar", M_VIEW_TOOLBAR);
	fMenuView->CheckEntry(M_VIEW_TOOLBAR);
   fMenuView->AddEntry("&Dock", M_VIEW_DOCK);
   fMenuView->AddEntry("&Undock", M_VIEW_UNDOCK);
   fMenuView->AddSeparator();
   fMenuView->AddEntry("Enable U&ndock", M_VIEW_ENBL_DOCK);
   fMenuView->AddEntry("Enable &Hide", M_VIEW_ENBL_HIDE);
   fMenuView->DisableEntry(M_VIEW_DOCK);

   fMenuDock->EnableUndock(kTRUE);
   fMenuDock->EnableHide(kTRUE);
   fMenuView->CheckEntry(M_VIEW_ENBL_DOCK);
   fMenuView->CheckEntry(M_VIEW_ENBL_HIDE);

   fMenuHelp = new TGPopupMenu(fClient->GetRoot());
   fMenuHelp->AddEntry("&Contents", M_HELP_CONTENTS);
   fMenuHelp->AddEntry("&Search...", M_HELP_SEARCH);
   fMenuHelp->AddSeparator();
   fMenuHelp->AddEntry("&About", M_HELP_ABOUT);

   fMenuNew1 = new TGPopupMenu();
   fMenuNew1->AddEntry("Remove New Menus", M_NEW_REMOVEMENU);

   fMenuNew2 = new TGPopupMenu();
   fMenuNew2->AddEntry("Remove New Menus", M_NEW_REMOVEMENU);

   // Menu button messages are handled by the main frame (i.e. "this")
   // ProcessMessage() method.
   fMenuFile->Associate(this);
   fMenuConf->Associate(this);
   fMenuView->Associate(this);
   fMenuHelp->Associate(this);
   fMenuNew1->Associate(this);
   fMenuNew2->Associate(this);

   fMenuBar = new TGMenuBar(fMenuDock, 1, 1, kHorizontalFrame);
   fMenuBar->AddPopup("&File", fMenuFile, fMenuBarItemLayout);
   fMenuBar->AddPopup("&Configuration", fMenuConf, fMenuBarItemLayout);
   fMenuBar->AddPopup("&View", fMenuView, fMenuBarItemLayout);
   fMenuBar->AddPopup("&Help", fMenuHelp, fMenuBarHelpLayout);

   fMenuDock->AddFrame(fMenuBar, fMenuBarLayout);

	gGMSGui = this;

}


//______________________________________________________________________________
Bool_t GMSGui::HandleKey(Event_t *event)
{
   char   input[10];
   Int_t  n;
   UInt_t keysym;

   if (event->fType == kGKeyPress) {
      gVirtualX->LookupString(event, input, sizeof(input), keysym);
      n = strlen(input);

      switch ((EKeySym)keysym) {   // ignore these keys
         case kKey_Shift:
         case kKey_Control:
         case kKey_Meta:
         case kKey_Alt:
         case kKey_CapsLock:
         case kKey_NumLock:
         case kKey_ScrollLock:
            return kTRUE;
         case kKey_F1:
//            SendMessage(this, MK_MSG(kC_COMMAND, kCM_MENU),
//                       M_HELP_SIMULATION, 0);
            return kTRUE;
         default:
            break;
      }
      if (event->fState & kKeyControlMask) {   // Cntrl key modifier pressed
         switch((EKeySym)keysym & ~0x20) {   // treat upper and lower the same
            case kKey_A:
               SendMessage(this, MK_MSG(kC_COMMAND, kCM_MENU),
                           M_FILE_SAVEAS, 0);
               return kTRUE;
            case kKey_B:
//               SendMessage(this, MK_MSG(kC_COMMAND, kCM_MENU),
//                           M_INSPECT_BROWSER, 0);
               return kTRUE;
            case kKey_I:
//               SendMessage(this, MK_MSG(kC_COMMAND, kCM_MENU),
//                           M_SHOW_INFOS, 0);
               return kTRUE;
            case kKey_O:
               SendMessage(this, MK_MSG(kC_COMMAND, kCM_MENU),
                           M_FILE_OPEN, 0);
               return kTRUE;
            case kKey_Q:
               SendMessage(this, MK_MSG(kC_COMMAND, kCM_MENU),
                           M_FILE_EXIT, 0);
               return kTRUE;
            default:
               break;
         }
      }
   }
   return TGMainFrame::HandleKey(event);
}

/// Create the Tool Bar
void GMSGui::MakeToolBar(){	
	int spacing = 8;
	fToolBar = new TGToolBar(this, 60, 20, kHorizontalFrame | kRaisedFrame);
	for (int i = 0; xpm_names[i]; i++) {
		TString iconname("icons/");
		iconname += xpm_names[i];
		tb_data[i].fPixmap = iconname.Data();
		if (strlen(xpm_names[i]) == 0) {
				spacing = 8;
				continue;
		}
		fToolBar->AddButton(this, &tb_data[i], spacing);
		spacing = 0;
	}

   AddFrame(fToolBar, new TGLayoutHints(kLHintsTop | kLHintsExpandX, 0, 0, 0, 0));
}

/// Show (show == kTRUE) or Hide (show == kFALSE) the Tool Bar
void GMSGui::ShowToolBar(Bool_t show){
   // Show or hide toolbar.

   if (show) {
      ShowFrame(fToolBar);
      fMenuView->CheckEntry(M_VIEW_TOOLBAR);
   } else {
      HideFrame(fToolBar);
      fMenuView->UnCheckEntry(M_VIEW_TOOLBAR);
   }
}

/// Handle messages send to the GMSGui object
/// E.g. all menu button messages.
Bool_t GMSGui::ProcessMessage(Long_t msg, Long_t parm1, Long_t parm2){

   switch (GET_MSG(msg)) {

	  case kC_TEXTENTRY:
         switch (GET_SUBMSG(msg)) {
			case kTE_TEXTCHANGED:
               switch (parm1) {
					case B_RUNNUMBER:
					gGMSAnalyser->SetRunNumber((Int_t)fRunNumber->GetNumber());
					gGMSAnalyser->SetFileIn();
					gGMSAnalyser->SetFileOut();
					break;
					case B_EVENTNUMBER:
					gGMSAnalyser->SetNEvent((Int_t)fEventNumber->GetNumber());
					break;
				}
				break;
		}
		break;
		
      case kC_COMMAND:
         switch (GET_SUBMSG(msg)) {
				
            case kCM_BUTTON:
            case kCM_MENU:
               switch (parm1) {

                  case M_FILE_OPEN:
                     {
                        static TString dir(".");
			               TGFileInfo fi;
                        fi.fFileTypes = filetypes;
                        fi.fIniDir    = StrDup(dir);
                        new TGFileDialog(fClient->GetRoot(), this, kFDOpen, &fi);
                        if(gGMSAnalyser->GetPrintLevel()>1) Info("GMSGui::ProcessMessage","Open file: %s (dir: %s)\n", fi.fFilename,fi.fIniDir);
                        dir = fi.fIniDir;
                     }
                     break;

                  case M_FILE_PRINT:
                     fMenuFile->HideEntry(M_FILE_PRINT);
                     break;

                  case M_FILE_PRINTSETUP:
                     fMenuFile->EnableEntry(M_FILE_PRINT);
                     break;
						
 						case M_FILE_SAVE:
							int retval;
 							new TGMsgBox(fClient->GetRoot(), this,
                                  "File Save", 
											 "Sorry but \"File->Save\" is not yet implemented.",
											 kMBIconExclamation, kMBOk, &retval);
							break;
								
                  case M_FILE_EXIT:
					case M_FILE_CLOSE:
 							if(gGMSAnalyser->GetPrintLevel() >1) Info("GMSGui::ProcessMessage","Terminate Application");
                    		CloseWindow();   // this also terminates theApp
                     	break;
						
						case M_CONF_SETTINGS:
							new GMSSettings(fClient->GetRoot(), this, 400, 200);
							break;
							
						case M_INSPECT_BROWSER:
							new TBrowser;
							break;
						case M_SHOW_3D:
							cA->cd();
							cA->GetViewer3D("ogl");
							break;
						case M_ZOOM_PLUS:
							cA->cd();
							cA->GetView()->ZoomView(0, 1.25);
							cA->Modified();
							cA->Update();
							break;

						case M_ZOOM_MOINS:
							cA->cd();
							cA->GetView()->UnzoomView(0, 1.25);
							cA->Modified();
							cA->Update();
							break;
						case M_CLEAR:
							fTextView->Clear();
							break;

                  case B_START:
                     gGMSAnalyser->Minimize();
                     break;


                  case M_VIEW_ENBL_DOCK:
                     fMenuDock->EnableUndock(!fMenuDock->EnableUndock());
                     if (fMenuDock->EnableUndock()) {
                        fMenuView->CheckEntry(M_VIEW_ENBL_DOCK);
                        fMenuView->EnableEntry(M_VIEW_UNDOCK);
                     } else {
                        fMenuView->UnCheckEntry(M_VIEW_ENBL_DOCK);
                        fMenuView->DisableEntry(M_VIEW_UNDOCK);
                     }
                     break;

                  case M_VIEW_ENBL_HIDE:
                     fMenuDock->EnableHide(!fMenuDock->EnableHide());
                     if (fMenuDock->EnableHide()) {
                        fMenuView->CheckEntry(M_VIEW_ENBL_HIDE);
                     } else {
                        fMenuView->UnCheckEntry(M_VIEW_ENBL_HIDE);
                     }
                     break;

                  case M_VIEW_DOCK:
                     fMenuDock->DockContainer();
                     fMenuView->EnableEntry(M_VIEW_UNDOCK);
                     fMenuView->DisableEntry(M_VIEW_DOCK);
                     break;

                  case M_VIEW_UNDOCK:
                     fMenuDock->UndockContainer();
                     fMenuView->EnableEntry(M_VIEW_DOCK);
                     fMenuView->DisableEntry(M_VIEW_UNDOCK);
                     break;
						
						case M_VIEW_TOOLBAR:
								if (fMenuView->IsEntryChecked(M_VIEW_TOOLBAR))
									ShowToolBar(kFALSE);
								else
									ShowToolBar();
								break;

                  default:
                     break;
               }
 
            default:
               break;
         }
      default:
         break;
   }
   return kTRUE;
}

void GMSGui::PrintOut( const char *fmt, ...){	
	
   // Print debugging message to stderr and, on Windows, to the system debugger.

   static Int_t buf_size = 2048;
   static char *buf = 0;

   va_list arg_ptr;
   va_start(arg_ptr, fmt);

   // R__LOCKGUARD2(gErrorMutex);

again:
   if (!buf)
      buf = new char[buf_size];

   Int_t n = vsnprintf(buf, buf_size, fmt, arg_ptr);

   if (n == -1 || n >= buf_size) {
      buf_size *= 2;
      delete [] buf;
      buf = 0;
      goto again;
   }
   va_end(arg_ptr);

   fTextView->AddLine(buf);
   fTextView->ScrollCanvas(-1, TGTextView::kVertical);
if(fTextView->ReturnLineCount()%20 == 0) fTextView->Update();
		 //fprintf(stderr, "%s", buf);
}

Int_t GMSGui::GetRunNumber(){
	return (Int_t)fRunNumber->GetNumber();
}
Int_t GMSGui::GetNEvent(){
	return (Int_t)fEventNumber->GetNumber();
}
