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
#include <TGFrame.h>
#include <TGButton.h>
#include <TGListBox.h>
#include <TGComboBox.h>
#include <TGTab.h>
#include <TRootEmbeddedCanvas.h>
#include <TGTextEntry.h>
#include <TGLayout.h>
#include <TGLabel.h>
#include <TCanvas.h>
#include <TTimer.h>
#include <TH1.h>
#include <TH2.h>
#include <TGXYLayout.h>
#include <TGFileDialog.h>
#include <TGNumberEntry.h>
#include "iostream"
using namespace std;


#include "GMSSettings.h"
#include "GMSGui.h"
#include "GMSAnalyser.h"

//ClassImp(GMSSettings)

GMSSettings::GMSSettings(const TGWindow *p, const TGWindow *main, UInt_t w,
                       UInt_t h, UInt_t options)
    : TGTransientFrame(p, main, w, h, options)
{
	/// @todo Ajouter la possibilite de sauver les settings dans un fichier.
	
//	    SetLayoutManager(new TGXYLayout(this));                           
    // Create a dialog window. A dialog window pops up with respect to its
   // "main" window.

   // Used to store GUI elements that need to be deleted in the ctor.
   fCleanup = new TList;

   fFrame1 = new TGHorizontalFrame(this, 60, 20, kFixedWidth);

   fOkButton = new TGTextButton(fFrame1, "&Ok", B_OK);
   fOkButton->Associate(this);

   fL1 = new TGLayoutHints(kLHintsTop | kLHintsLeft | kLHintsExpandX,
                           2, 2, 2, 2);
   fL2 = new TGLayoutHints(kLHintsBottom | kLHintsRight, 2, 2, 5, 1);

   fFrame1->AddFrame(fOkButton, fL1);
   //fFrame1->AddFrame(fCancelButton, fL1);

   fFrame1->Resize(150, fOkButton->GetDefaultHeight());
   AddFrame(fFrame1, fL2);

   //--------- create Tab widget and some composite frames for Tab testing

   fTab = new TGTab(this, 300, 300);
   fL3 = new TGLayoutHints(kLHintsTop | kLHintsLeft, 5, 5, 5, 5);

   TGCompositeFrame *tf = fTab->AddTab("Input / Output");
   fF1 = new TGCompositeFrame(tf, 50, 20, kVerticalFrame);
   fF1->SetLayoutManager(new TGMatrixLayout(fF1, 4, 3));   
   char buff[25]; 
	sprintf(buff, "Raw Data Directory");
   fF1->AddFrame(new TGLabel(fF1, new TGHotString(buff)));
   fF1->AddFrame(fTxt1 = new TGTextEntry(fF1, new TGTextBuffer(200)));	   
	fF1->AddFrame(fBrowseButton1 = new TGTextButton(fF1, "Browse", B_RAW_DATA_DIR));
	fTxt1->SetText(gGMSAnalyser->GetRawDataDir());
		
	sprintf(buff, "Output Directory");
   fF1->AddFrame(new TGLabel(fF1, new TGHotString(buff)));
   fF1->AddFrame(fTxt2 = new TGTextEntry(fF1, new TGTextBuffer(200)));	   
	fF1->AddFrame(fBrowseButton2 = new TGTextButton(fF1, "Browse", B_OUTPUT_DIR));
	fTxt2->SetText(gGMSAnalyser->GetOutputDir());
		
		
   tf->AddFrame(fF1, fL3);
   fTxt1->Resize(250, fTxt1->GetDefaultHeight());
   fTxt2->Resize(250, fTxt2->GetDefaultHeight());
	fBrowseButton1->Associate(this);
 	fBrowseButton2->Associate(this);  
   
   //--------- Database Tab --------
   	tf = fTab->AddTab("Databases");
   	tf->SetLayoutManager(new TGHorizontalLayout(tf));
   	fF3 = new TGCompositeFrame(tf, 50, 20, kVerticalFrame);
   	fF3->SetLayoutManager(new TGMatrixLayout(fF3, 4, 3));   
	   

	sprintf(buff, "Chamber Database File");
   	fF3->AddFrame(new TGLabel(fF3, new TGHotString(buff)));
   	fF3->AddFrame(fTxt3 = new TGTextEntry(fF3, new TGTextBuffer(200)));	   
	fF3->AddFrame(fBrowseButton3 = new TGTextButton(fF3, "Browse", B_CHAMBER_DB_FILE));
	fTxt3->SetText(gGMSAnalyser->GetChamberDBFileName());
   	fTxt3->Resize(300, fTxt3->GetDefaultHeight());
	
	sprintf(buff, "Platform Database File");
   	fF3->AddFrame(new TGLabel(fF3, new TGHotString(buff)));
   	fF3->AddFrame(fTxt4 = new TGTextEntry(fF3, new TGTextBuffer(200)));	   
	fF3->AddFrame(fBrowseButton4 = new TGTextButton(fF3, "Browse", B_PLATFORM_DB_FILE));
	fTxt4->SetText(gGMSAnalyser->GetPlatformDBFileName());
   	fTxt4->Resize(300, fTxt4->GetDefaultHeight());
	
	sprintf(buff, "Lignes Database File");
   	fF3->AddFrame(new TGLabel(fF3, new TGHotString(buff)));
   	fF3->AddFrame(fTxt5 = new TGTextEntry(fF3, new TGTextBuffer(200)));	   
	fF3->AddFrame(fBrowseButton5 = new TGTextButton(fF3, "Browse", B_LINE_DB_FILE));
	fTxt5->SetText(gGMSAnalyser->GetBlocDBFileName());
   	fTxt5->Resize(300, fTxt5->GetDefaultHeight());
	
	sprintf(buff, "Sigmas File");
   	fF3->AddFrame(new TGLabel(fF3, new TGHotString(buff)));
   	fF3->AddFrame(fTxt6 = new TGTextEntry(fF3, new TGTextBuffer(200)));	   
	fF3->AddFrame(fBrowseButton6 = new TGTextButton(fF3, "Browse", B_SIGMA_FILE));
	fTxt6->SetText(gGMSAnalyser->GetSigmasFileName());
   	fTxt6->Resize(300, fTxt6->GetDefaultHeight());
	
	tf->AddFrame(fF3, fL3);
	
	
 	fBrowseButton3->Associate(this);  
 	fBrowseButton4->Associate(this);  
 	fBrowseButton5->Associate(this);  
 	fBrowseButton6->Associate(this);  

   //--------- Options Tab --------
   tf = fTab->AddTab("Options");
   tf->SetLayoutManager(new TGHorizontalLayout(tf));
   fL1 = new TGLayoutHints(kLHintsTop | kLHintsLeft | kLHintsExpandX,
                           200, 2, 2, 2);
   
	fF6 = new TGGroupFrame(tf, "Running Mode", kVerticalFrame);
   fF6->SetTitlePos(TGGroupFrame::kRight); // right aligned
   tf->AddFrame(fF6, fL3);
   
	// 1 column, 4 rows
   fF6->SetLayoutManager(new TGMatrixLayout(fF6, 4, 1,10));   
	
	fF6->AddFrame(fRad1 = new TGRadioButton(fF6, "Monitoring", B_MONITORING));
   fF6->AddFrame(fRad2 = new TGRadioButton(fF6, "Alignment ", B_ALIGNMENT));
   fF6->AddFrame(fRad3 = new TGRadioButton(fF6, "Simulation", B_SIMULATION));
   fF6->AddFrame(fRad4 = new TGRadioButton(fF6, "Sigma Calculation", B_SIGMA_CALC));
	switch(gGMSAnalyser->GetMode()){
		case GMSAnalyser::kMonitMode:
			fRad1->SetState(kButtonDown);
			break;
		case GMSAnalyser::kAlignMode:
			fRad2->SetState(kButtonDown);
			break;
		case GMSAnalyser::kSimulMode:
			fRad3->SetState(kButtonDown);
			break;
		case GMSAnalyser::kSigmaMode:
			fRad4->SetState(kButtonDown);
			break;
	}
   fF6->Resize(); // resize to default size

//----------------------------------------------------------------------------
	fF7 = new TGGroupFrame(tf, "Print Level", kVerticalFrame);
   fF7->SetTitlePos(TGGroupFrame::kRight); // right aligned
   tf->AddFrame(fF7, fL3);
   
	// 1 column, 3 rows
   fF7->SetLayoutManager(new TGMatrixLayout(fF7, 3, 1, 10));   
	
	fF7->AddFrame(fRad11 = new TGRadioButton(fF7, "Off ", B_PRINT_OFF));
	fF7->AddFrame(fRad12 = new TGRadioButton(fF7, "Lite", B_PRINT_LITE));
   fF7->AddFrame(fRad13 = new TGRadioButton(fF7, "Full", B_PRINT_FULL));
	switch(gGMSAnalyser->GetPrintLevel()){
		case 0:
			fRad11->SetState(kButtonDown);
			break;
		case 1:
			fRad12->SetState(kButtonDown);
			break;
		case 2:
			fRad13->SetState(kButtonDown);
			break;
	}
   fF7->Resize(); // resize to default size
//----------------------------------------------------------------------------

   
	fF8 = new TGGroupFrame(tf, "TMS Options");
	fF8->SetLayoutBroken(kTRUE);
   
   fF8->SetTitlePos(TGGroupFrame::kRight); // right aligned
   tf->AddFrame(fF8, fL3);
   
   fF8->AddFrame(fRad21 = new TGRadioButton(fF8, "One Platform Fixed ", B_ONE_PLAT_FIXED), new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
   fRad21->MoveResize(32,40,152,20);
   fF8->AddFrame(fRad22 = new TGRadioButton(fF8, "Z of 3 Platforms Fixed", B_Z_FIXED), new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
   fRad22->MoveResize(32,64,152,20);
	
	switch(gGMSAnalyser->GetTMSMode()){
		case 0:
			fRad21->SetState(kButtonDown);
			break;
		case 1:
			fRad22->SetState(kButtonDown);
			break;
	}

   fRefPlatNumber = new TGNumberEntry(fF8, (Double_t) gGMSAnalyser->GetTMSRefPlatform(),6,B_TMS_REF_PLATFORM,(TGNumberFormat::EStyle) 0,(TGNumberFormat::EAttribute) 1,(TGNumberFormat::ELimit) 2,-1,3);
   fF8->AddFrame(fRefPlatNumber, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
   fRefPlatNumber->MoveResize(144,88,59,22);
   fRefPlatLabel = new TGLabel(fF8,"Reference Platform");
   fRefPlatLabel->SetTextJustify(36);
   fF8->AddFrame(fRefPlatLabel, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
   fRefPlatLabel->MoveResize(32,88,112,20);

   fF8->SetLayoutManager(new TGVerticalLayout(fF8));
   fF8->Resize(224,128);
	//----------------------------------------------------------------------------
	
	
	fF9 = new TGGroupFrame(tf, "Minimizer");
	fF9->SetLayoutBroken(kTRUE);
	
	fF9->SetTitlePos(TGGroupFrame::kRight); // right aligned
	tf->AddFrame(fF9, fL3);
	
	fF9->AddFrame(fCombo = new TGComboBox(fF9, B_MINIMIZER_TYPE), new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
	fCombo->AddEntry("Minuit",0);
	fCombo->AddEntry("Minuit2",1);
	fCombo->AddEntry("Fumili2",2);
	
	fCombo->MoveResize(32,40,152,20);
	
	if(strcmp(gGMSAnalyser->GetMinimizerType(),"Minuit")==0) fCombo->Select(0);
	else if(strcmp(gGMSAnalyser->GetMinimizerType(),"Minuit2")==0) fCombo->Select(1);
	else if(strcmp(gGMSAnalyser->GetMinimizerType(),"Fumili2")==0) fCombo->Select(2);
	
	fF9->SetLayoutManager(new TGVerticalLayout(fF9));
	fF9->Resize(224,128);
	
//----------------------------------------------------------------------------

   fRad1->Associate(this);
   fRad2->Associate(this);
   fRad3->Associate(this);
   fRad4->Associate(this);
   fRad11->Associate(this);
   fRad12->Associate(this);
   fRad13->Associate(this);
   fRad21->Associate(this);
   fRad22->Associate(this);
	fCombo->Associate(this);
	fRefPlatNumber->Associate(this);


   TGLayoutHints *fL4 = new TGLayoutHints(kLHintsBottom | kLHintsExpandX |
                                          kLHintsExpandY, 2, 2, 5, 1);
   AddFrame(fTab, fL4);

   MapSubwindows();
   Resize();   // resize to default size

   // position relative to the parent's window
   CenterOnParent();

   SetWindowName("Settings");

   MapWindow();
   //fClient->WaitFor(this);    // otherwise canvas contextmenu does not work
}

GMSSettings::~GMSSettings()
{
   // Delete test dialog widgets.
   fCleanup->Delete();
   delete fCleanup;
   delete fOkButton;
   //delete fCancelButton;
  delete fFrame1;
   delete fL1; delete fL2;
   delete fTab;
   delete fL3;
   delete fF1;
   delete fTxt1; delete fTxt2;delete fTxt3;delete fTxt4;delete fTxt5;delete fTxt6;
   delete fBrowseButton1;	delete fBrowseButton2;	delete fBrowseButton3;delete fBrowseButton4;
   delete fBrowseButton5;delete fBrowseButton6;
   delete fF6;
   delete fRad1; delete fRad2; delete fRad3; delete fRad4;
   delete fRad11; delete fRad12; delete fRad13;
    delete fF7, fF3;
    //delete fL4;		
}


// void GMSSettings::CloseWindow()
// {
//    // Called when window is closed via the window manager.
// 
//    //DeleteWindow();
//    delete this;
// }

Bool_t GMSSettings::ProcessMessage(Long_t msg, Long_t parm1, Long_t)
{
   // Process messages coming from widgets associated with the dialog.

   char tmp[20];
   static int newtab = 0;

	switch (GET_MSG(msg)) {
	   case kC_TEXTENTRY:
			switch (GET_SUBMSG(msg)) {
				case kTE_TEXTCHANGED:
					switch (parm1) {
						case B_TMS_REF_PLATFORM:
							gGMSAnalyser->SetTMSRefPlatform((Int_t)fRefPlatNumber->GetNumber());
							break;
					}
					break;
			}
			break;
		
		case kC_COMMAND:
			switch (GET_SUBMSG(msg)) {
				case kCM_COMBOBOX:
					switch (parm1) {
						case B_MINIMIZER_TYPE:
							switch (fCombo->GetSelected()) {
								case 0:
									gGMSAnalyser->SetMinimizerType("Minuit");
									break;
								case 1:
									gGMSAnalyser->SetMinimizerType("Minuit2");
									break;
								case 2:
									gGMSAnalyser->SetMinimizerType("Fumili2");
									break;
							}
							break;
					}
					break;

				case kCM_BUTTON:
					switch(parm1) {
						case B_OK:
							printf("\nTerminating dialog: OK pressed\n");
							CloseWindow();
							break;
						case B_RAW_DATA_DIR:
						case B_OUTPUT_DIR:
							printf("\n Browse button: pressed\n");                    
							{
								static TString dir(".");
								TGFileInfo fi;
								const char *filetypes[] = { "All files",     "*",
									"ROOT files",    "*.root",
									"ROOT macros",   "*.C",
									0,               0 };
								fi.fFileTypes = filetypes;
								fi.fIniDir    = StrDup(dir);
								new TGFileDialog(fClient->GetRoot(), this, kFDOpen, &fi);
								printf("Open file: %s (dir: %s)\n", fi.fFilename,fi.fIniDir);
								dir = fi.fIniDir;
								if(parm1==B_RAW_DATA_DIR){
									fTxt1->SetText(fi.fIniDir);
									gGMSAnalyser->SetRawDataDir(fi.fIniDir);
								} else {
									fTxt2->SetText(fi.fIniDir);
									gGMSAnalyser->SetOutputDir(fi.fIniDir);
								}
							}
							break;
						case B_CHAMBER_DB_FILE:
						case B_PLATFORM_DB_FILE:
						case B_LINE_DB_FILE:
						case B_SIGMA_FILE:
							printf("\n Browse button: pressed\n");                    
						{
							static TString dir(".");
							TGFileInfo fi;
							const char *filetypes[] = { "Database files",     "*.db","All files",     "*",
								"ROOT files",    "*.root",
								"ROOT macros",   "*.C",
								0,               0 };
							fi.fFileTypes = filetypes;
							fi.fIniDir    = StrDup(dir);
							new TGFileDialog(fClient->GetRoot(), this, kFDOpen, &fi);
							printf("Open file: %s (dir: %s)\n", fi.fFilename, fi.fIniDir);
							dir = fi.fIniDir;
							if(!fi.fFilename) break;
								if(parm1==B_CHAMBER_DB_FILE){
									fTxt3->SetText(fi.fFilename);
									gGMSAnalyser->SetChamberDBFileName(fi.fFilename);
								} else if(parm1==B_PLATFORM_DB_FILE){
									fTxt4->SetText(fi.fFilename);
									gGMSAnalyser->SetPlatformDBFileName(fi.fFilename);
								}else if(parm1 ==B_LINE_DB_FILE){
									fTxt5->SetText(fi.fFilename);
									gGMSAnalyser->SetBlocDBFileName(fi.fFilename);
								}else{
									fTxt6->SetText(fi.fFilename);
									gGMSAnalyser->SetSigmasFileName(fi.fFilename);
								}
							}
							break;

						default:
							break;
					}
					break;
				case kCM_RADIOBUTTON:
					switch (parm1) {
						case B_MONITORING:
							fRad1->SetState(kButtonDown);
							fRad2->SetState(kButtonUp);
							fRad3->SetState(kButtonUp);
							fRad4->SetState(kButtonUp);
							gGMSAnalyser->SetMode(GMSAnalyser::kMonitMode);
							gGMSGui->GetEventNumberFrame()->SetState(kFALSE);
							break;
						case B_ALIGNMENT:
							fRad1->SetState(kButtonUp);
							fRad2->SetState(kButtonDown);
							fRad3->SetState(kButtonUp);
							fRad4->SetState(kButtonUp);
							gGMSAnalyser->SetMode(GMSAnalyser::kAlignMode);
							gGMSGui->GetEventNumberFrame()->SetState(kFALSE);
							break;
						case B_SIMULATION:
							fRad1->SetState(kButtonUp);
							fRad2->SetState(kButtonUp);
							fRad3->SetState(kButtonDown);
							fRad4->SetState(kButtonUp);
							gGMSAnalyser->SetMode(GMSAnalyser::kSimulMode);	
							gGMSGui->GetEventNumberFrame()->SetState(kTRUE);
							break;
						case B_SIGMA_CALC:
							fRad1->SetState(kButtonUp);
							fRad2->SetState(kButtonUp);
							fRad3->SetState(kButtonUp);
							fRad4->SetState(kButtonDown);
							gGMSAnalyser->SetMode(GMSAnalyser::kSigmaMode);	
							gGMSGui->GetEventNumberFrame()->SetState(kFALSE);
							break;
						case B_PRINT_OFF:
							fRad11->SetState(kButtonDown);
							fRad12->SetState(kButtonUp);
							fRad13->SetState(kButtonUp);
							gGMSAnalyser->SetPrintLevel(0);
							break;
						case B_PRINT_LITE:
							fRad11->SetState(kButtonUp);
							fRad12->SetState(kButtonDown);
							fRad13->SetState(kButtonUp);
							gGMSAnalyser->SetPrintLevel(1);
							break;
						case B_PRINT_FULL:
							fRad11->SetState(kButtonUp);
							fRad12->SetState(kButtonUp);
 							fRad13->SetState(kButtonDown);
							gGMSAnalyser->SetPrintLevel(2);
							break;
						case B_ONE_PLAT_FIXED:
							fRad21->SetState(kButtonDown);
							fRad22->SetState(kButtonUp);
							gGMSAnalyser->SetTMSMode(GMSAnalyser::kOnePlatformFixed);
							break;
						case B_Z_FIXED:
							fRad21->SetState(kButtonUp);
							fRad22->SetState(kButtonDown);
							gGMSAnalyser->SetTMSMode(GMSAnalyser::kThreeZFixed);
							break;
					}
					break;
				case kCM_CHECKBUTTON:
					switch (parm1) {
						default:
							break;
					}
					break;
				case kCM_TAB:
					break;
				default:
					break;
			}
			break;

		default:
			break;
		}
	return kTRUE;
}
