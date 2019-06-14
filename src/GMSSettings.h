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
#ifndef GMSSETTINGS_H
#define GMSSETTINGS_H
#include <RQ_OBJECT.h>

#include <TGFrame.h>

class TGPictureButton;
class TGCheckButton;
class TGListBox;
class TGComboBox;
class TGTab;
class TRootEmbeddedCanvas;
class TGTextEntry;
class TGLayoutHints;
class TList;


/**
@author Raphael Tieulent
*/

//!  Class which describs the GUI of the Settings part
/*!
*/
class GMSSettings : public TGTransientFrame{

private:
   TGCompositeFrame    *fFrame1, *fF1, *fF2, *fF3, *fF4, *fF5;
   TGCompositeFrame   *fRefPlatFrame;
   TGGroupFrame        *fF6, *fF7, *fF8, *fF9;
   TGButton            *fOkButton, *fCancelButton, *fStartB, *fStopB;
	TGButton 			*fTMSModeButton;
	
   TGButton            *fBrowseButton1, *fBrowseButton2 , *fBrowseButton3, *fBrowseButton4, *fBrowseButton5, *fBrowseButton6;
   TGButton            *fBtn1, *fBtn2, *fChk1, *fChk2, *fRad1, *fRad2, *fRad3, *fRad4 ,*fRad11, *fRad12, *fRad13,*fRad21, *fRad22;
   TGPictureButton     *fPicBut1;
   TGCheckButton       *fCheck1;
   TGCheckButton       *fCheckMulti;
   TGListBox           *fListBox;
   TGComboBox          *fCombo;
   TGTab               *fTab;
   TGTextEntry         *fTxt1, *fTxt2, *fTxt3, *fTxt4, *fTxt5, *fTxt6;
   TGLayoutHints       *fL1, *fL2, *fL3, *fL4;
   TRootEmbeddedCanvas *fEc1, *fEc2;
   Int_t                fFirstEntry;
   Int_t                fLastEntry;
   Bool_t               fFillHistos;

   TList               *fCleanup;
   TGNumberEntry      *fRefPlatNumber;
	TGLabel 			*fRefPlatLabel;


public:
   GMSSettings(const TGWindow *p, const TGWindow *main, UInt_t w, UInt_t h,
               UInt_t options = kVerticalFrame);
   virtual ~GMSSettings();

   //virtual void CloseWindow();
   virtual Bool_t ProcessMessage(Long_t msg, Long_t parm1, Long_t parm2);
	
	enum CommandIdentifiers {
		B_OK,
		B_TMS_REF_PLATFORM,
		B_RAW_DATA_DIR,
		B_OUTPUT_DIR,
		B_CHAMBER_DB_FILE,
		B_PLATFORM_DB_FILE,
		B_LINE_DB_FILE,
		B_SIGMA_FILE,
		B_MONITORING,
		B_ALIGNMENT,
		B_SIMULATION,
		B_SIGMA_CALC,
		B_PRINT_OFF,
		B_PRINT_LITE,
		B_PRINT_FULL,
		B_ONE_PLAT_FIXED,
		B_Z_FIXED,
		B_MINIMIZER_TYPE
	};
	
// 	ClassDef(GMSSettings,1) 

};



#endif
