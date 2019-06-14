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
#ifndef GMSGUI_H
#define GMSGUI_H

#include <TGFrame.h>

class TGDockableFrame;
class TGCanvas;
class TGTextEntry;
class TGButton;
class TGColorSelect;
class TGMenu;
class TGMenuBar;
class TGPopupMenu;
class TGToolBar;
class TGTab;
class TRootEmbeddedCanvas;
class TGTextView;
class TCanvas;
class TGNumberEntry;
class TGLabel;

/**
Class describing the GUI Main frame of the GMS Analyser. It derives from TGMainFrame, a Root class.

@author Raphael Tieulent
*/
class GMSGui : public TGMainFrame
{

	
private:

	TGDockableFrame    *fMenuDock;
   TGCompositeFrame   *fStatusFrame;
   TGCanvas           *fCanvasWindow;
   TGTextEntry        *fTestText;
   TGButton           *fTestButton, *fStopButton, *fStartButton;
   TGColorSelect      *fColorSel;

    
   TGCompositeFrame   *fMainFrame, *fButtonFrame, *fSelectionFrame ,*fRunFrame,*fEventFrame;
   TGMenuBar          *fMenuBar;
	TGToolBar			 *fToolBar;
   TGPopupMenu        *fMenuFile, *fMenuView, *fMenuHelp, *fMenuConf;
   TGPopupMenu        *fMenuNew1, *fMenuNew2;
   TGLayoutHints      *fMenuBarLayout, *fMenuBarItemLayout, *fMenuBarHelpLayout;
	TGLayoutHints      *fL5, *fButtonLayout;
 
 	TCanvas            *cA;
   TCanvas            *cB;
   TCanvas            *cC;
	 // Display frame
    TGTab              *fDisplayFrame; // TGTab for graphical and text display
    TRootEmbeddedCanvas *fEmbeddedCanvas; // the actual frame which displays event
    TRootEmbeddedCanvas *fEmbeddedCanvas2; // the actual frame which displays event
    TRootEmbeddedCanvas *fEmbeddedCanvas3; // the actual frame which displays histo
    TGTextView         *fTextView;

    // Zooming stuff...
    TGHorizontalFrame  *fHFrame,*fHFrame2;
    TGLayoutHints      *fZoomButtonsLayout;
    TGLayoutHints      *fClearButtonLayout;
    TGButton           *fZoomPlusButton,*fZoomMoinsButton;
    TGButton           *fClearButton;
    TGButton           *fZoomPlusButton2,*fZoomMoinsButton2;
    TGNumberEntry      *fRunNumber;
	TGLabel 			*fRunLabel;
    TGNumberEntry      *fEventNumber;
	TGLabel 			*fEventLabel;
	
	void MakeMenuBarFrame();
	void MakeToolBar();
	void ShowToolBar(Bool_t show = kTRUE);

public:
   GMSGui(const TGWindow *p, UInt_t w, UInt_t h);
   virtual ~GMSGui();
	
	inline TCanvas * GetGeomCanvas() const {return cA;};
	inline TGTextView * GetStdOut() const {return fTextView;};
	
	void PrintOut( const char *fmt, ...);
		   
	virtual Bool_t     HandleKey(Event_t *event);
   virtual void CloseWindow();
   virtual Bool_t ProcessMessage(Long_t msg, Long_t parm1, Long_t);
   Int_t GetRunNumber();
   Int_t GetNEvent();
   TGNumberEntry* GetEventNumberFrame(){return fEventNumber;};
	//ClassDef(GMSGui,1) 

};

extern GMSGui * gGMSGui;
#endif
