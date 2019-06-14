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

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <TRandom3.h>
#include <TROOT.h>
#include <TCanvas.h>
#include <TThread.h>
#include <TGTextView.h>
#include <TFitter.h>
#include <TStopwatch.h>
#include <TGeoManager.h>
#include <TGMsgBox.h>
#include <TGeoVolume.h>
#include <TGeoMatrix.h>
#include <TMath.h>
#include <TString.h>
#include <TFile.h>
#include <TTree.h>
#include <TStopwatch.h>
#include <TClonesArray.h>
#include <TMap.h>
#include <TApplication.h>
#include <Math/FitMethodFunction.h>
#include <Math/Factory.h>

#include <iostream>
using namespace std;
#include <cstdlib>

#include "AliLog.h"
#include "Spectro.h"
#include "Plane.h"
#include "GMSGui.h"
#include "BlocTMS.h"
#include "GMSDimClient.h"
#include "GMSAnalyser.h"
#include "LMSChi2Function.h"
#include "TMSChi2Function.h"

//ClassImp(GMSAnalyser)


TGeoMaterial *mat;
TGeoMaterial *mat1;

TGeoMedium *med0 ;
TGeoMedium *med1 ;
TGeoVolume *TOP ;

Spectro *gSimulatedSpectrometer ;
Spectro *gMeasuredSpectrometer;

GMSAnalyser * gGMSAnalyser;
TApplication * gGMSApplication;
TGeoManager * gGMSGeoManager;
TClonesArray * fOutputArray;

GMSAnalyser::GMSAnalyser(const TGWindow *p, UInt_t w, UInt_t h) : 
	TObject(), fFitterLMS(NULL)
{
// 	// By default the GMS analyser is in the Monitoring Mode.
// 	fMode = kSimulMode;
// 	//fMode = kMonitMode;
// 	fPrintLevel = 1;
// 	fRawDataDir = "/homelocal/tieulent/GMS/Data";
// 	fOutputDir  = "/homelocal/tieulent/GMS/Results";
// 	fChamberDBFileName  = "data/Chambers.db";
// 	fPlatformDBFileName = "data/Platforms.db";
// 	fBlocDBFileName     = "data/Lignes.db";
	fTimer = new TStopwatch();
	
	fGMSAnalyserRootDir = getenv("GMSANALYSER");
	AliInfo(Form("GMSAnalyser Root Directory : %s",fGMSAnalyserRootDir.Data()));
	fSettingsFileName = fGMSAnalyserRootDir.Data() ;
	fSettingsFileName += "/Settings.txt";
	
	fTMSRefPlatform = 0;
	ReadSettings();
	
	fNEvent = 1;
	
	fFileOut = NULL;
	fTreeOut = NULL;
    time_t t;
	fRandObj = new TRandom3((UInt_t)time(&t));
	
	fOutputArray = new TClonesArray("TGeoHMatrix",10);
	
	gGMSAnalyser = this;
	fMainGui = new GMSGui(p,w,h);
	
	//fRunNumber = gGMSGui->GetRunNumber();
	
	PrintMode();

}

//_________________________________________________________________________________________________________________________________


GMSAnalyser::~GMSAnalyser()
{	
	delete fMeasuredSpectrometer;
	delete fSimulatedSpectrometer;
	gSimulatedSpectrometer = gMeasuredSpectrometer = NULL;
	delete TOP;
	delete med1;
	delete med0;
	delete mat1;
	delete mat;
	delete fGeoManager;

	delete fMainGui;
	delete fOutputArray;
	delete fRandObj;
	delete fTimer;
	delete fTreeOut;	
}

//_________________________________________________________________________________________________________________________________

/// Create the spectrometers and the tree to save out the results
void GMSAnalyser::CreateSpectro(){

	fSimulatedSpectrometer = new Spectro(fChamberDBFileName.Data(),fBlocDBFileName.Data(),Spectro::kSimulated);
	fMeasuredSpectrometer  = new Spectro(fChamberDBFileName.Data(),fBlocDBFileName.Data(),Spectro::kMeasured);
	
	gSimulatedSpectrometer = this->fSimulatedSpectrometer;
	gMeasuredSpectrometer  = this->fMeasuredSpectrometer;
	
	fSigmasFile = fopen(fSigmasFileName.Data(),"r");
	gSimulatedSpectrometer->ReadSigmas(fSigmasFile);
	rewind(fSigmasFile);
	gMeasuredSpectrometer->ReadSigmas(fSigmasFile);
	fclose(fSigmasFile);


}
//_________________________________________________________________________________________________________________________________

void GMSAnalyser::Minimize(){
	SetFileIn();
	SetFileOut();
	OpenFileOut();

	// Double_t shift_reel[10][3];
	// Double_t shift_mes[10][3];
	// Double_t ang_glob_ch_reel[10][3];
	// Double_t ang_glob_ch_mes[10][3];
	//    
	// memset ( shift_reel, 0, sizeof(shift_reel) ); 
	// memset ( shift_mes, 0, sizeof(shift_mes) ); 
	// memset ( ang_glob_ch_reel, 0, sizeof(ang_glob_ch_reel) ); 
	// memset ( ang_glob_ch_mes, 0, sizeof(ang_glob_ch_mes) ); 
	//    
	// gSimulatedSpectrometer->InitChamberPosition(shift_mes,ang_glob_ch_mes);
	// gMeasuredSpectrometer->InitChamberPosition(shift_reel,ang_glob_ch_reel);

	gSimulatedSpectrometer->CalcRefImages();
	
	fTimer->Start(kTRUE);
	if(fMode == kSimulMode) {	
		for(int iCh=0;iCh<gMeasuredSpectrometer->GetNChamber();iCh++){
			Double_t shift[3];
			Double_t ang[3];
			for(int i=0;i<3;i++){
 				shift[i] = fRandObj->Gaus(0.,Plane::MAXDEP);
 				ang[i] =   fRandObj->Gaus(0.,Plane::MAXDEP);
// 				shift[i] = (-iCh-1)*(Plane::MAXDEP);
// 				ang[i] =   (-iCh-1)*(Plane::MAXDEP);
//				shift[i] = 0. ;
//				ang[i] = 0. ;
			}
			gMeasuredSpectrometer->GetChamber(iCh)->Translate(shift);
			gMeasuredSpectrometer->GetChamber(iCh)->CalculateMatrixChgtGlobPlane(ang);
		}
		for (int iEvent=0;iEvent<fNEvent;iEvent++) {
			if(iEvent%100 == 0 ) AliDebug(1,Form("Event : %d",iEvent));
			MinimizeSimul(iEvent);
			Clean();
			//MinimizeSimulGlobal(iEvent);
		}
		
	} else if(fMode == kSigmaMode){
		AliDebug(2,"Starting sigma calculation");
		CalculateSigmas();
	} else {
// 	 	MinimizeGlobal();
 		MinimizeMonitor();
//	 	MinimizeMixed();
	}
		
	AliInfo(Form(" ---->  %d EVENTS Analyzed",fNEvent));
	fTimer->Stop();
	fTimer->Print("m");


	// //if(gGMSAnalyser->GetPrintLevel() >1) 
	// Info("GMSAnalyser::Minimize","Writing out ROOT file");
	// fOutputArray->Print();
	//     fFileOut = new TFile(fOutputFileName.Data(),"RECREATE");
	// fFileOut->WriteObject(fOutputArray,"GMSarray");
	// //gGMSAnalyser->GetTree()->Print();
	// //if(gGMSAnalyser->GetPrintLevel() >1) 
	// Info("GMSAnalyser::Minimize","Closing ROOT file");
	// fFileOut->Close();
	// delete fFileOut;							
	
}
//_________________________________________________________________________________________________________________________________

void GMSAnalyser::MinimizeSimul(Int_t iEvent){

/// @todo  implementation a finir 

	Double_t IntResFactor = 3.;
	
	Int_t nParam =24;
	Int_t nChamber = gSimulatedSpectrometer->GetNChamber();
	if(6*nChamber > 24 ) nParam = 6*nChamber;

	Double_t *param = new Double_t[nParam];
	for(int i=0;i<nParam;i++) param[i] =0.;
	
		
	Int_t nParamLMS ;
	nParamLMS = 6*nChamber;
	fSimulParamLMS.Set(nParamLMS);
	fSimulParamTMS = new Double_t*[nChamber];
	for(int i=0;i<nChamber;i++) fSimulParamTMS[i] = new Double_t[24];
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Choose randomly the displacement parameters for the chambers (paramLMS) and for the platforms on the chambers
// (paramTMS).
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
 	for (int iCh=0;iCh<nChamber;iCh++){
 		Chamber * CurrentChamber = gMeasuredSpectrometer->GetChamber(iCh);
 		for (int j=0;j<6;j++) {
 			fSimulParamLMS[6*iCh+j] = fRandObj->Gaus(0.,Plane::MAXDEP);
 			if(iCh==0) fSimulParamLMS[6*iCh+j] =0.;
 		}
 		CurrentChamber->SetSimulatedDisplacementParameters(fSimulParamLMS.GetArray(),6*iCh);
 		CurrentChamber->SetDisplacementParameters(fSimulParamLMS.GetArray(), 6*iCh);
 		CurrentChamber->CalculateDisplacementMatrix();
		// ---START :-  To be uncommented ..... 
 		for (int iPlatform=0;iPlatform<4;iPlatform++) {
 			for(int j=0;j<6;j++){
 				fSimulParamTMS[iCh][iPlatform*6+j] = fRandObj->Gaus(0.,1.e-4);
 //  				if((j>1 && j<5) || (j==5 && iPlatform == 0)) continue; // Do not induce displacments in ThetaZ, X and Y for Platforms.
  				if((j>1 && j<5)  ) fSimulParamTMS[iCh][iPlatform*6+j] = 0.; // Do not induce displacments in ThetaZ, X and Y for Platforms.
 				//else fSimulParamTMS[iCh][iPlatform*6+j] = (2.*fRandObj->Rndm() - 1.) * Plane::MAXDEP;
 			}
 			CurrentChamber->GetPlatform(iPlatform)->SetSimulatedDisplacementParameters((fSimulParamTMS[iCh])+(6*iPlatform));	
 			CurrentChamber->GetPlatform(iPlatform)->SetDisplacementParameters((fSimulParamTMS[iCh])+(6*iPlatform));	
 			CurrentChamber->GetPlatform(iPlatform)->CalculateDisplacementMatrix();	
 		}
		// --END : --  To be uncommented ..... 
 	}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Do step movement of Chamber 7
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//	for (int iCh=0;iCh<nChamber;iCh++){
//		Chamber * CurrentChamber = gMeasuredSpectrometer->GetChamber(iCh);
//		for (int j=0;j<6;j++) {
//			fSimulParamLMS[6*iCh+j] =0.;
// 			if(iCh==1 && j == 3) fSimulParamLMS[6*iCh+j] = 100e-6* (iEvent+1); // 100 micron steps
// 			if(iCh==1 && j == 5) fSimulParamLMS[6*iCh+j] = 100e-6* (iEvent+1); // 100 micron steps
//			if(iCh==1 && j == 1) fSimulParamLMS[6*iCh+j] = 5e-3* (iEvent+1); // 500 microrad steps
//		}
//		CurrentChamber->SetSimulatedDisplacementParameters(fSimulParamLMS+(6*iCh));
//		CurrentChamber->SetDisplacementParameters(fSimulParamLMS+(6*iCh));
//		CurrentChamber->CalculateDisplacementMatrix();
//		for (int iPlatform=0;iPlatform<4;iPlatform++) {
//			for(int j=0;j<6;j++){
//				fSimulParamTMS[iCh][iPlatform*6+j] = 0.;
//			}
//			CurrentChamber->GetPlatform(iPlatform)->SetSimulatedDisplacementParameters((fSimulParamTMS[iCh])+(6*iPlatform));	
//			CurrentChamber->GetPlatform(iPlatform)->SetDisplacementParameters((fSimulParamTMS[iCh])+(6*iPlatform));	
//			CurrentChamber->GetPlatform(iPlatform)->CalculateDisplacementMatrix();	
//		}
//	}
	
// 	// Initialize the displacement of the chambers of the measured spectrometer.
// 	gMeasuredSpectrometer->InitDepChambers(fSimulParamLMS);
// 	
// 	// Initialize the displacement of the platforms for each chamber of the measured spectrometer.
// 	for (int i=0;i<nChamber;i++) gMeasuredSpectrometer->InitDepPlatforms(gMeasuredSpectrometer->GetChamber(i),fSimulParamTMS[i]);
		
	if(fPrintLevel>1)	
		for(int iCh=0;iCh<gMeasuredSpectrometer->GetNChamber();iCh++){
			Chamber * CurrentChamber = gMeasuredSpectrometer->GetChamber(iCh);
			CurrentChamber->PrintSimulatedDisplacementParameters();
			CurrentChamber->PrintDisplacementMatrix();
			for(int iPlatform=0;iPlatform<4;iPlatform++){
				CurrentChamber->GetPlatform(iPlatform)->PrintSimulatedDisplacementParameters();
				CurrentChamber->GetPlatform(iPlatform)->PrintDisplacementMatrix();
			}
		}

	// Random initialization of the position and orientation of the optical boxes.
	// The boxes are randomly positionned only for the gMeasuredSpectrometer into simulation mode.
	// Therefore we use the position and orientation of the boxes in the gSimulatedSpectrometer as a reference.
	gMeasuredSpectrometer->InitRandomBoxPosition(fRandObj,gSimulatedSpectrometer);
	 
	// Random initialization of the deformation of the walls.
	//gMeasuredSpectrometer->InitRandomWallDeformation(fRandObj);
	 	 	 
	// Calculation of the Reference images
	gMeasuredSpectrometer->CalcRefImages();
	 
	/* on peut deplacer les chambres a partir de leur position reelle,
	puisque la minimisation se fait sur les mesures des BCAM,
	independamment de la maniere dont elles ont ete obtenues */
	gMeasuredSpectrometer->CalcFinalImages();
	 
	/* ajout d'une erreur intrinseque de mesure */
	//gMeasuredSpectrometer->AddIntrinsicError(fRandObj,IntResFactor);

	// Remove optical lines which are break down
	//SetBreakDownLines();

	//---------------------------------
	// Start the minimization procedure
	//---------------------------------
					
	// Extract the position and orientation of the platforms of each chamber (Transverse Monitoring System)
	InitFitterTMS(param);
	FitTMS(param);

	// Extract the position and orientation of each chamber (Longitudinal Monitoring System) - First pass
	InitFitterLMS(param);
	FitLMS(param,1);

	// Extract the position and orientation of chamber 9 with respect to the wall (External Monitoring System) 
	if(gSimulatedSpectrometer->GetNBlocEMS()) FitEMS(param);

	// Extract the position and orientation of each chamber (Longitudinal Monitoring System) - Second pass
	// Necessary only if there is some EMS blocs
	if(gSimulatedSpectrometer->GetNBlocEMS()) FitLMS(param,2);
	
	delete param;
	
}
//_________________________________________________________________________________________________________________________________

void GMSAnalyser::MinimizeMonitor(){

	AliInfo(Form("Opening data file %s\n",fRawDataFileName.Data()));
	if(!(fRawDataFile = fopen(fRawDataFileName.Data(),"r"))){
		TString Message = "GMSAnalyser::MinimizeMonitor, \nEnable to open data file ";
		Message += fRawDataFileName.Data();
		Message += "\n GMSAnalyser will Abort";
		Int_t retval;
		new TGMsgBox(gClient->GetRoot(), gGMSGui,"Fatal Error", Message.Data(),
	                kMBIconExclamation, kMBOk, &retval,0,kTextLeft);
		
		AliFatal(Form("Enable to open data file %s\n",fRawDataFileName.Data()));
	}
		
	int hour,min,sec,day,month,year;

	if(fMode == kAlignMode) // Choose the "Theoritical" image as reference (alignment)
		gMeasuredSpectrometer->CalcRefImages();
	else{  // Choose the first image of the file as reference image (monitoring)
		gMeasuredSpectrometer->ReadRefImages(fRawDataFile,fPrintLevel);
	}
	Int_t nParam =24;
	Int_t nChamber = gSimulatedSpectrometer->GetNChamber();
	if(6*nChamber > 24 ) nParam = 6*nChamber;

	Double_t *param = new Double_t[nParam];
	for(int i=0;i<nParam;i++) param[i] =0.;
	
	InitFitterTMS(param);
	
	if(gSimulatedSpectrometer->GetNBlocLMS()) InitFitterLMS(param);

	// BEGIN : Loop over the Data
	int iEvent=0;
	while(EOF!=fscanf(fRawDataFile,"%4d%02d%02d%02d%02d%02d \n",&year,&month,&day,&hour,&min,&sec)){		
		
		gGMSGui->PrintOut("\nEvent %d : Taken %02d/%02d/%4d at %02dh%02d:%02d",iEvent,day,month,year,hour,min,sec);
		
		iEvent++;
		if ((iEvent%50 == 0) || (iEvent==1)){
			printf("%6d : ",iEvent);
			printf("Data taking at : %02dh%02d:%02d on %02d-%02d-%4d \n",hour,min,sec,day,month,year);
			fTimer->Stop();
			fTimer->Print("m");
			fTimer->Continue();
		}
	 	fOutputFile = fopen(fTextOutputFileName,"a");
		fprintf(fOutputFile,"%4d%02d%02d%02d%02d%02d\n",year,month,day,hour,min,sec);
		fclose(fOutputFile);	   
		// Read the real measurements at time T :
		gMeasuredSpectrometer->ReadFinalImages(fRawDataFile,fPrintLevel);
	 
		// Remove optical lines which are break down
		SetBreakDownLines();
		
		// Start the minimization procedure
				
		// Extract the position and orientation of the platforms of each chamber (Transverse Monitoring System)
		FitTMS(param);
		
		// Extract the position and orientation of each chamber (Longitudinal Monitoring System) - First pass
		if(gSimulatedSpectrometer->GetNBlocLMS()) FitLMS(param,1);
		// Extract the position and orientation of chamber 9 with respect to the wall (External Monitoring System) 
		if(gSimulatedSpectrometer->GetNBlocEMS()) FitEMS(param);
		
		// Extract the position and orientation of each chamber (Longitudinal Monitoring System) - Second pass
		// Necessary only if there is some EMS blocs
		if(gSimulatedSpectrometer->GetNBlocEMS()) FitLMS(param,2);
		
		fNEvent = iEvent;
 		//if(fTreeOut) fTreeOut->Fill();
 	} //END : Loop over the data 
	//((BlocTMS*)gSimulatedSpectrometer->GetBloc(0))->GetChamber()->GetPlatform(2)->PrintDisplacementParameters();
}
//_________________________________________________________________________________________________________________________________

void GMSAnalyser::MinimizeSimulGlobal(Int_t iEvent){

/// @todo  implementation a finir 

	Double_t IntResFactor = 3.;
	
	Int_t nParam =24;
	Int_t nChamber = gSimulatedSpectrometer->GetNChamber();
	if(30*nChamber > 24 ) nParam = 30*nChamber;

	Double_t *param = new Double_t[nParam];
	for(int i=0;i<nParam;i++) param[i] =0.;
	
	InitFitterGlobal(param);
		
	fSimulParamGlobal = new Double_t[nParam];
	
	// Choose randomly the displacement parameters for the chambers (paramLMS) and for the platforms on the chambers (paramTMS).
	for (int iCh=0;iCh<nChamber;iCh++){
		Chamber * CurrentChamber = gMeasuredSpectrometer->GetChamber(iCh);
		for (int j=0;j<6;j++) {
			fSimulParamGlobal[30*iCh+j] = fRandObj->Gaus(0.,Plane::MAXDEP);
			if(iCh==2) fSimulParamGlobal[30*iCh+j] /=1000.;
		}
		CurrentChamber->SetSimulatedDisplacementParameters(fSimulParamGlobal+(30*iCh));
		for (int iPlatform=0;iPlatform<4;iPlatform++) {
			for(int j=0;j<6;j++){
// 				fSimulParamGlobal[30*iCh+(iPlatform+1)*6+j] = 0.;
				fSimulParamGlobal[30*iCh+(iPlatform+1)*6+j] = fRandObj->Gaus(0,1.e-4);
				if((j>1 && j<5) || (iPlatform==0 && j==5)) continue; // Do not induce displacments in ThetaZ, X and Y for Platforms.
			}
			CurrentChamber->GetPlatform(iPlatform)->SetSimulatedDisplacementParameters((fSimulParamGlobal)+(30*iCh+6*(iPlatform+1)));	
		}
	}
	
	// Initialize the displacement of the chambers of the measured spectrometer.
	gMeasuredSpectrometer->InitGlobalDisplacements(fSimulParamGlobal);
			
	if(fPrintLevel>1)	
		for(int iCh=0;iCh<gMeasuredSpectrometer->GetNChamber();iCh++){
			Chamber * CurrentChamber = gMeasuredSpectrometer->GetChamber(iCh);
			CurrentChamber->PrintSimulatedDisplacementParameters();
			for(int iPlatform=0;iPlatform<4;iPlatform++)
				CurrentChamber->GetPlatform(iPlatform)->PrintSimulatedDisplacementParameters();
		}

	// Random initialization of the position and orientation of the optical boxes.
	// The boxes are randomly positionned only for the gMeasuredSpectrometer into simulation mode.
	// Therefore we use the position and orientation of the boxes in the gSimulatedSpectrometer as a reference.
	//gMeasuredSpectrometer->InitRandomBoxPosition(fRandObj,gSimulatedSpectrometer);
	 
	// Random initialization of the deformation of the walls.
	//gMeasuredSpectrometer->InitRandomWallDeformation(fRandObj);
	 	 	 
	// Calculation of the Reference images
	gMeasuredSpectrometer->CalcRefImages();
	 
	/* on peut deplacer les chambres a partir de leur position reelle,
	puisque la minimisation se fait sur les mesures des BCAM,
	independamment de la maniere dont elles ont ete obtenues */
	gMeasuredSpectrometer->CalcFinalImages();
	 
	/* ajout d'une erreur intrinseque de mesure */
	//gMeasuredSpectrometer->AddIntrinsicError(fRandObj,IntResFactor);

	// Remove optical lines which are break down
	//SetBreakDownLines();

	//---------------------------------
	// Start the minimization procedure
	//---------------------------------
				
		// Do a global fit
		FitGlobal(param);
					
	
}
//_________________________________________________________________________________________________________________________________

void GMSAnalyser::MinimizeGlobal(){

	if(!(fRawDataFile = fopen(fRawDataFileName.Data(),"r")))
		AliFatal(Form("Enable to open data file %s\n",fRawDataFileName.Data()));
	
		
	int hour,min,sec,day,month,year;

	if(fMode == kAlignMode) // Choose the "Theoritical" image as reference (alignment)
		gMeasuredSpectrometer->CalcRefImages();
	else{  // Choose the first image of the file as reference image (monitoring)
		gMeasuredSpectrometer->ReadRefImages(fRawDataFile,fPrintLevel);
	}
	Int_t nParam =24;
	Int_t nChamber = gSimulatedSpectrometer->GetNChamber();
	if(30*nChamber > 24 ) nParam = 30*nChamber;

	Double_t *param = new Double_t[nParam];
	for(int i=0;i<nParam;i++) param[i] =0.;
	
	InitFitterGlobal(param);
	
	// BEGIN : Loop over the Data
	int iEvent=0;
	while(EOF!=fscanf(fRawDataFile,"%4d%02d%02d%02d%02d%02d \n",&year,&month,&day,&hour,&min,&sec)){		
		iEvent++;
		if ((iEvent%50 == 0) || (iEvent==1)){
			printf("%6d : ",iEvent);
			printf("Data taking at : %02dh%02d:%02d on %02d-%02d-%4d \n",hour,min,sec,day,month,year);
			fTimer->Stop();
			fTimer->Print("m");
			fTimer->Continue();
		}
	 	fOutputFile = fopen(fTextOutputFileName,"a");
		fprintf(fOutputFile,"%4d%02d%02d%02d%02d%02d\n",year,month,day,hour,min,sec);
		fclose(fOutputFile);	   
		// Read the real measurements at time T :
		gMeasuredSpectrometer->ReadFinalImages(fRawDataFile,fPrintLevel);
	 
		// Remove optical lines which are break down
		SetBreakDownLines();
		
		// Start the minimization procedure
				
		// Do a global fit
		FitGlobal(param);
		
		
		fNEvent = iEvent;
 		//if(fTreeOut) fTreeOut->Fill();
 	} //END : Loop over the data 
}
//_________________________________________________________________________________________________________________________________

void GMSAnalyser::MinimizeMixed(){

	if(!(fRawDataFile = fopen(fRawDataFileName.Data(),"r")))
		AliFatal(Form("Enable to open data file %s\n",fRawDataFileName.Data()));
	
		
	int hour,min,sec,day,month,year;

	if(fMode == kAlignMode) // Choose the "Theoritical" image as reference (alignment)
		gMeasuredSpectrometer->CalcRefImages();
	else{  // Choose the first image of the file as reference image (monitoring)
		gMeasuredSpectrometer->ReadRefImages(fRawDataFile,fPrintLevel);
	}
	Int_t nParam =24;
	Int_t nChamber = gSimulatedSpectrometer->GetNChamber();
	if(30*nChamber > 24 ) nParam = 30*nChamber;

	Double_t *param = new Double_t[nParam];
	for(int i=0;i<nParam;i++) param[i] =0.;
	
	InitFitterGlobal(param);
	
	// BEGIN : Loop over the Data
	int iEvent=0;
	while(EOF!=fscanf(fRawDataFile,"%4d%02d%02d%02d%02d%02d \n",&year,&month,&day,&hour,&min,&sec)){		
		iEvent++;
		if ((iEvent%50 == 0) || (iEvent==1)){
			printf("%6d : ",iEvent);
			printf("Data taking at : %02dh%02d:%02d on %02d-%02d-%4d \n",hour,min,sec,day,month,year);
			fTimer->Stop();
			fTimer->Print("m");
			fTimer->Continue();
		}
	 	fOutputFile = fopen(fTextOutputFileName,"a");
		fprintf(fOutputFile,"%4d%02d%02d%02d%02d%02d\n",year,month,day,hour,min,sec);
		fclose(fOutputFile);	   
		// Read the real measurements at time T :
		gMeasuredSpectrometer->ReadFinalImages(fRawDataFile,fPrintLevel);
	 
		// Remove optical lines which are break down
		SetBreakDownLines();
		
		// Start the minimization procedure
				
		// Do a global fit
		FitGlobal(param);
		
		InitFitterTMSMixed(param);
	
		if(gSimulatedSpectrometer->GetNBlocLMS()) InitFitterLMSMixed(param);
		
		
		FitTMS(param);
		
		if(gSimulatedSpectrometer->GetNBlocLMS()) FitLMS(param,1);
		
		fNEvent = iEvent;
 		//if(fTreeOut) fTreeOut->Fill();
 	} //END : Loop over the data 
}
//_________________________________________________________________________________________________________________________________

void GMSAnalyser::CalculateSigmas(){

	if(!(fRawDataFile = fopen(fRawDataFileName.Data(),"r")))
		AliFatal(Form("Enable to open data file %s\n",fRawDataFileName.Data()));
	
		
	int hour,min,sec,day,month,year;
	
	// FIRST PASS : Calculate the mean of each measurement
	// BEGIN : Loop over the Data
	int iEvent=0;
	while(EOF!=fscanf(fRawDataFile,"%4d%02d%02d%02d%02d%02d \n",&year,&month,&day,&hour,&min,&sec)){		
		iEvent++;
		if(fPrintLevel>0) 
			if ((iEvent%50 == 0) || (iEvent==1)){
				printf("%6d : ",iEvent);
				printf("Data taking at : %02dh%02d:%02d on %02d-%02d-%4d \n",hour,min,sec,day,month,year);
			}

		gMeasuredSpectrometer->ReadFinalImages(fRawDataFile,fPrintLevel);
	 
		for(int ibloc=0;ibloc<gMeasuredSpectrometer->GetNBloc();ibloc++)
			for(int iline=0;iline<gMeasuredSpectrometer->GetBloc(ibloc)->GetNLines();iline++)
				for(int imeas=0;imeas<gMeasuredSpectrometer->GetBloc(ibloc)->GetNMeasuresPerLine();imeas++)	{
					double val = gMeasuredSpectrometer->GetBloc(ibloc)->GetRefImage(iline, imeas);
					val *= iEvent -1;
					val += gMeasuredSpectrometer->GetBloc(ibloc)->GetFinalImage(iline, imeas);
					val /= (double)iEvent;
					gMeasuredSpectrometer->GetBloc(ibloc)->SetRefImage(iline, imeas,val);
				}
 	} //END : FIRST PASS
	
	fclose(fRawDataFile);
	fRawDataFile = fopen(fRawDataFileName.Data(),"r");
	
	// Second PASS : Calculate the Sigma of each measurement
	iEvent=0;
	while(EOF!=fscanf(fRawDataFile,"%4d%02d%02d%02d%02d%02d \n",&year,&month,&day,&hour,&min,&sec)){		
		iEvent++;
		if(fPrintLevel>0) 
			if ((iEvent%50 == 0) || (iEvent==1)){
				printf("%6d : ",iEvent);
				printf("Data taking at : %02dh%02d:%02d on %02d-%02d-%4d \n",hour,min,sec,day,month,year);
			}

		gMeasuredSpectrometer->ReadFinalImages(fRawDataFile,fPrintLevel);
	 
		for(int ibloc=0;ibloc<gMeasuredSpectrometer->GetNBloc();ibloc++)
			for(int iline=0;iline<gMeasuredSpectrometer->GetBloc(ibloc)->GetNLines();iline++)
				for(int imeas=0;imeas<gMeasuredSpectrometer->GetBloc(ibloc)->GetNMeasuresPerLine();imeas++)	{
					double mean = gMeasuredSpectrometer->GetBloc(ibloc)->GetRefImage(iline, imeas);
					double val = gMeasuredSpectrometer->GetBloc(ibloc)->GetFinalImage(iline, imeas);
					double sigma = gMeasuredSpectrometer->GetBloc(ibloc)->GetSigma(iline, imeas);
					sigma *= iEvent -1;
					sigma += TMath::Power(val-mean,2.);
					sigma /= (double)iEvent;
// 					printf("bloc %d line %d meas %d sigma %e  val %e mean %e\n"
// 						,ibloc,iline,imeas,sigma,mean,val);
					gMeasuredSpectrometer->GetBloc(ibloc)->SetSigma(iline, imeas,sigma);
				}
 	} //END : SECOND PASS
	
	// Now take the square root
	for(int ibloc=0;ibloc<gMeasuredSpectrometer->GetNBloc();ibloc++)
		for(int iline=0;iline<gMeasuredSpectrometer->GetBloc(ibloc)->GetNLines();iline++)
			for(int imeas=0;imeas<gMeasuredSpectrometer->GetBloc(ibloc)->GetNMeasuresPerLine();imeas++)	{
				double sigma = gMeasuredSpectrometer->GetBloc(ibloc)->GetSigma(iline, imeas);
				sigma = TMath::Sqrt(sigma);
// 				printf("bloc %d line %d meas %d sigma %e \n"
// 					,ibloc,iline,imeas,sigma);
				gMeasuredSpectrometer->GetBloc(ibloc)->SetSigma(iline, imeas,sigma);
			}
	if(!(fSigmasFile = fopen(fSigmasFileName.Data(),"w")))
		AliFatal(Form("Enable to open data file %s\n",fSigmasFileName.Data()));	
	gMeasuredSpectrometer->WriteSigmas(fSigmasFile);
	
	fNEvent = iEvent;
}
//_________________________________________________________________________________________________________________________________

void GMSAnalyser::FitLMS(Double_t * param, Int_t PassNumber){		
			

	// Disable all Bloc which are not part of the LMS
	for (int iBloc=0;iBloc<gSimulatedSpectrometer->GetNBloc();iBloc++)
		if(gSimulatedSpectrometer->GetBloc(iBloc)->GetBlocType() != Bloc::kBlocLMS)
			gSimulatedSpectrometer->DisableBloc(iBloc); 


	if(PassNumber == 2){ // During the second pass of the LMS, the External chamber is fixed 
		int ChId = gSimulatedSpectrometer->GetExternalChamberID();
		for(int i=5;i>=0;i--) {
			int ipar = 6*ChId+i;
			fFitterLMS->SetFixedVariable(ipar,GetLMSVariableName(ChId, i),param[ipar]);
		}
	}
	fChi2LMS->NFree(fFitterLMS->NFree());
// 	for(int i=0;i<fFitterLMS->GetNumberTotalParameters();i++) { 
// 		param[i] =0.;
// 	}

	Bool_t FitStatus = fFitterLMS->Minimize();
	gSimulatedSpectrometer->CalcFinalImages();
	if(fPrintLevel >=1)
		for (int iBloc=0;iBloc<gSimulatedSpectrometer->GetNBloc();iBloc++)
			{
				cout<<"Final Simulated Images"<<endl;
			 	gSimulatedSpectrometer->PrintDisplacements(iBloc);
				cout<<"Final Measured Images"<<endl;
			 	gMeasuredSpectrometer->PrintDisplacements(iBloc);
			}
	
	
//     for(int i=0;i<gSimulatedSpectrometer->GetNBloc();i++) 
// 		for(int j=0;j<gSimulatedSpectrometer->GetNLine(i);j++) 
// 	    	for(int k=0;k<gSimulatedSpectrometer->GetNMeasurePerLine(i);k++) 
//  				printf("Bloc %d Line %d Mesure %d   SpectroMes %e   SpectroSim %e Diff %e\n",i, j, k,  gMeasuredSpectrometer->GetImageDisplacement(i,j,k) ,
//  						gSimulatedSpectrometer->GetImageDisplacement(i,j,k),gMeasuredSpectrometer->GetImageDisplacement(i,j,k) -gSimulatedSpectrometer->GetImageDisplacement(i,j,k));


	const double * Values = fFitterLMS->X();
	const double * Errors = fFitterLMS->Errors();
	double ChiSquare = fFitterLMS->MinValue();
	
	for(int iCh=0;iCh<gSimulatedSpectrometer->GetNChamber();iCh++){
		Plane * CurrentChamber = gSimulatedSpectrometer->GetChamber(iCh);
		for(int i=0;i<6;i++){
			CurrentChamber->SetDisplacementParameter(i,Values[6*iCh + i]);
			CurrentChamber->SetDisplacementParameterError(i,Errors[6*iCh + i]);
		}
		CurrentChamber->CalculateDisplacementMatrix();
		CurrentChamber->FillDisplacementMatrix();
	}
			
	/*----------------------------*/
	/* Record the results         */
	/*----------------------------*/
	/* Only if Minuit converge correctly */
  // printf("ISTAT = %d \n",istat);
	if(FitStatus) {
		fOutputFile = fopen(fTextOutputFileName,"a");
		fSimulationFile = fopen(fSimulationFileName,"a");

		if(fPrintLevel >=1) {
			printf("-----------------------------------------------------\n");
			printf("\tTheta X(rad) \t Theta Y (rad) \t Theta Z(rad)\t  X(mm) \t  Y (mm) \t Z(mm)\n");		
		}		
		if(fPrintLevel >=1) {
			gGMSGui->PrintOut("---------- LMS -------------------------------------------");
			gGMSGui->PrintOut("\tTheta X(rad) \t Theta Y (rad) \t Theta Z(rad)\t  X(mm) \t  Y (mm) \t Z(mm)");		
		}		
		fprintf(fOutputFile,"%22.16lf \n",ChiSquare);
		for(int iCh=0;iCh<gSimulatedSpectrometer->GetNChamber();iCh++){
			Plane * CurrentChamber = gSimulatedSpectrometer->GetChamber(iCh);
		    Plane * CurrentChamberGen = gMeasuredSpectrometer->GetChamber(iCh);
			for(int i=0;i<6;i++) 
				fprintf(fOutputFile,"%+22.16le ",CurrentChamber->GetDisplacementParameter(i));
			for(int i=0;i<6;i++) 
				fprintf(fOutputFile,"%+22.16le ",CurrentChamber->GetDisplacementParameterError(i));
			fprintf(fOutputFile,"\n");
			if(fMode==kSimulMode) {
				for(int i=0;i<6;i++) 
					fprintf(fSimulationFile,"%+22.16le ",CurrentChamberGen->GetSimulatedDisplacementParameter(i));
				fprintf(fSimulationFile,"\n");				
			}
			
			if(fPrintLevel >=1) {
				for(int i=0;i<6;i++){
					if(i<3) {
						printf("\t%+.3e ",CurrentChamber->GetDisplacementParameter(i));
					}else{
						printf("\t%+.3e ",1.e3*CurrentChamber->GetDisplacementParameter(i));
					}
				}
				printf("\n");
			}
		}
		if(fPrintLevel >=1) printf("-----------------------------------------------------\n");
		fprintf(fOutputFile,"\n");
		fclose(fOutputFile);			
		if(fMode==kSimulMode) {
			fprintf(fSimulationFile,"\n");
			fclose(fSimulationFile);			
		}
	} else {
		std::cout << "Fitting failed !!!! " << std::endl;
	}
	
	// Reenable all bloc after the LMS
	for (int iBloc=0;iBloc<gSimulatedSpectrometer->GetNBloc();iBloc++)
		gSimulatedSpectrometer->EnableBloc(iBloc);
}
//_________________________________________________________________________________________________________________________________

void GMSAnalyser::FitGlobal(Double_t * param){		
		
	/// @todo Implementation en cours. Il faut finir

	Double_t ChiSquare,edm,errdef;
	Int_t nvpar,nparx,istat;
	Double_t param_err[100];
	Double_t arglist[10];



	fFitterGlobal->ExecuteCommand("MIGRAD",arglist,0);
	gSimulatedSpectrometer->CalcFinalImages();
	
//     for(int i=0;i<gSimulatedSpectrometer->GetNBloc();i++) 
// 		for(int j=0;j<gSimulatedSpectrometer->GetNLine(i);j++) 
// 	    	for(int k=0;k<gSimulatedSpectrometer->GetNMeasurePerLine(i);k++) 
//  				printf("Bloc %d Line %d Mesure %d   SpectroMes %e   SpectroSim %e \n",i, j, k,  gMeasuredSpectrometer->GetImageDisplacement(i,j,k) ,
//  						gSimulatedSpectrometer->GetImageDisplacement(i,j,k));


	istat = fFitterGlobal->GetStats(ChiSquare,edm,errdef,nvpar,nparx);
	//fFitterLMS->PrintResults(3,amin);

	for(int iCh=0;iCh<gSimulatedSpectrometer->GetNChamber();iCh++){
		Plane * CurrentChamber = gSimulatedSpectrometer->GetChamber(iCh);
		for(int i=0;i<6;i++){
			CurrentChamber->SetDisplacementParameter(i,fFitterGlobal->GetParameter(30*iCh + i));
			CurrentChamber->SetDisplacementParameterError(i,fFitterGlobal->GetParError(30*iCh + i));
		}
		for(int iPl=0;iPl<4;iPl++){
			for(int i=0;i<6;i++){
				((Chamber *)CurrentChamber)->GetPlatform(iPl)->SetDisplacementParameter(i,fFitterGlobal->GetParameter(30*iCh + 6 * (iPl+1) + i));
			    ((Chamber *)CurrentChamber)->GetPlatform(iPl)->SetDisplacementParameterError(i,fFitterGlobal->GetParError(30*iCh + 6 * (iPl+1) + i));
			}
		}
		
	}
	for(int i=0;i<fFitterGlobal->GetNumberTotalParameters();i++) { 
		param[i]  = fFitterGlobal->GetParameter(i);
		param_err[i] = fFitterGlobal->GetParError(i);
		
	}
			
	/*----------------------------*/
	/* Record the results         */
	/*----------------------------*/
	/* Only if Minuit converge correctly */
  // printf("ISTAT = %d \n",istat);
// 	if(istat>=2) {
// 		fOutputFile = fopen(fTextOutputFileName,"a");
// 		if(fMode==kSimulMode) fSimulationFile = fopen(fSimulationFileName,"a");
// 
// 		if(fPrintLevel >=1) {
// 			printf("-----------------------------------------------------\n");
// 			printf("\tTheta X(rad) \t Theta Y (rad) \t Theta Z(rad)\t  X(mm) \t  Y (mm) \t Z(mm)\n");		
// 		}		
// 		fprintf(fOutputFile,"%22.16lf \n",ChiSquare);
// 		for(int iCh=0;iCh<gSimulatedSpectrometer->GetNChamber();iCh++){
// 			Plane * CurrentChamber = gSimulatedSpectrometer->GetChamber(iCh);
// 		    Plane * CurrentChamberGen = gMeasuredSpectrometer->GetChamber(iCh);
// 			for(int i=0;i<6;i++) 
// 				fprintf(fOutputFile,"%+22.16le ",CurrentChamber->GetDisplacementParameter(i));
// 			for(int i=0;i<6;i++) 
// 				fprintf(fOutputFile,"%+22.16le ",CurrentChamber->GetDisplacementParameterError(i));
// 			fprintf(fOutputFile,"\n");
// 			if(fMode==kSimulMode) {
// 				for(int i=0;i<6;i++) 
// 					fprintf(fSimulationFile,"%+22.16le ",CurrentChamberGen->GetSimulatedDisplacementParameter(i));
// 				fprintf(fSimulationFile,"\n");				
// 			}
// 			
// 			for(int iPl=0;iPl<4;iPl++){
// 				for(int i=0;i<6;i++)
// 					fprintf(fOutputFile,"%+22.16le ",((Chamber *)CurrentChamber)->GetPlatform(iPl)->GetDisplacementParameter(i));
// 				for(int i=0;i<6;i++) 
// 					fprintf(fOutputFile,"%+22.16le ",((Chamber *)CurrentChamber)->GetPlatform(iPl)->GetDisplacementParameterError(i));
// 				fprintf(fOutputFile,"\n");
// 			}
// 			if(fMode==kSimulMode) {
// 				for(int iPl=0;iPl<4;iPl++){
// 					for(int i=0;i<6;i++)
// 						fprintf(fSimulationFile,"%+22.16le ",((Chamber *)CurrentChamberGen)->GetPlatform(iPl)->GetDisplacementParameter(i));
// 					fprintf(fSimulationFile,"\n");
// 				}
// 				fprintf(fSimulationFile,"\n");
// 			}
// 			
// 			
// 			if(fPrintLevel >=1) {
// 				for(int i=0;i<6;i++){
// 					if(i<3) printf("\t%+.3e ",CurrentChamber->GetDisplacementParameter(i));
// 					else printf("\t%+.3e ",1.e3*CurrentChamber->GetDisplacementParameter(i));
// 				}
// 				printf("\n");
// 			}
// 			fprintf(fOutputFile,"\n");
// 		}
// 		if(fPrintLevel >=1) printf("-----------------------------------------------------\n");
// 		fprintf(fOutputFile,"\n");
// 		fclose(fOutputFile);			
// 		if(fMode==kSimulMode) {
// 			fprintf(fSimulationFile,"\n");
// 			fclose(fSimulationFile);			
// 		}
// 	}
	

}
//_________________________________________________________________________________________________________________________________

void GMSAnalyser::FitEMS(Double_t * param){
/// @todo A faire
}
//_________________________________________________________________________________________________________________________________

void GMSAnalyser::FitTMS(Double_t * paramTMS){
	//Info("GMSAnalyser::FitTMS","START");

	int iBlocTMS =0;

	for (int iBloc=0;iBloc<gMeasuredSpectrometer->GetNBloc();iBloc++){
		if(gMeasuredSpectrometer->GetBloc(iBloc)->GetBlocType() != Bloc::kBlocTMS) continue;	
		
		fCurrentBloc = iBloc;
		gSimulatedSpectrometer->InitDepPlatforms(((BlocTMS*)gSimulatedSpectrometer->GetBloc(fCurrentBloc))->GetChamber(),
																	(Double_t*)paramTMS);
		
		Double_t param_err[24];
		
		fChi2TMS[iBlocTMS]->NFree(fFitterTMS[iBlocTMS]->NFree());
		
	 	Bool_t FitStatus = fFitterTMS[iBlocTMS]->Minimize();
	 	gSimulatedSpectrometer->CalcFinalImages(fCurrentBloc);
		// if(fPrintLevel >=1)
		// {
		// 	cout<<"Final Simulated Images"<<endl;
		//  	gSimulatedSpectrometer->PrintFinalImages(iBloc);
		// 	cout<<"Final Masured Images"<<endl;
		//  	gMeasuredSpectrometer->PrintFinalImages(iBloc);
		// }
		const double * Values = fFitterTMS[iBlocTMS]->X();
		const double * Errors = fFitterTMS[iBlocTMS]->Errors();
		double ChiSquare = fFitterTMS[iBlocTMS]->MinValue();
		
		
		
		for(int iPlatform=0;iPlatform<4;iPlatform++){
			Plane * CurrentPlatform = ((BlocTMS*)gSimulatedSpectrometer->GetBloc(fCurrentBloc))->GetChamber()->GetPlatform(iPlatform);
			for(int i=0;i<6;i++) {
				CurrentPlatform->SetDisplacementParameter(i,Values[6*iPlatform + i]);
				CurrentPlatform->SetDisplacementParameterError(i,Errors[6*iPlatform + i]);
			}				
			CurrentPlatform->CalculateDisplacementMatrix();

			//CurrentPlatform->WriteDisplacementMatrix();
		}
		for(int i=0;i<24;i++) {
			paramTMS[i]  = Values[i];
			param_err[i] = Errors[i];
		}
			
		/*----------------------------*/
		/* enregistrement du resultat */
		/*----------------------------*/
		/* uniquement si ca a bien converge */
		if(FitStatus && ChiSquare>0.) {
	 		fOutputFile = fopen(fTextOutputFileName,"a");
			if(fMode==kSimulMode) fSimulationFile = fopen(fSimulationFileName,"a");
	    
			if(fPrintLevel >=1) {
				gGMSGui->PrintOut("---------- TMS CHAMBER %02d -------------------------------------------",((BlocTMS*)gSimulatedSpectrometer->GetBloc(fCurrentBloc))->GetChamber()->GetID());
				gGMSGui->PrintOut("\tTheta X(rad) \t Theta Y (rad) \t Z(mm)");		
			}		
// 				printf("-----------------------------------------------------\n");
// 			((BlocTMS*)gSimulatedSpectrometer->GetBloc(iBloc))->PrintFinalImages();
// 				printf("-----------------------------------------------------\n");
// 			((BlocTMS*)gSimulatedSpectrometer->GetBloc(iBloc))->PrintImagesDisplacement();
// 				printf("-----------------------------------------------------\n");
// 			((BlocTMS*)gMeasuredSpectrometer->GetBloc(iBloc))->PrintImagesDisplacement();
// 				printf("-----------------------------------------------------\n");
// 			((BlocTMS*)gMeasuredSpectrometer->GetBloc(iBloc))->PrintFinalImages();
// 				printf("-----------------------------------------------------\n");
// 			((BlocTMS*)gMeasuredSpectrometer->GetBloc(iBloc))->PrintRefImages();
// 				printf("-----------------------------------------------------\n");
// 				printf("-----------------------------------------------------\n");
		    fprintf(fOutputFile,"%22.16lf \n",ChiSquare);
			for(int iPlatform=0;iPlatform<4;iPlatform++){
				Plane * CurrentPlatformSim = ((BlocTMS*)gSimulatedSpectrometer->GetBloc(fCurrentBloc))->GetChamber()->GetPlatform(iPlatform);
				Plane * CurrentPlatformMes = ((BlocTMS*)gMeasuredSpectrometer->GetBloc(fCurrentBloc))->GetChamber()->GetPlatform(iPlatform);
				double dthx = CurrentPlatformSim->GetDisplacementParameter(0);
				double dthy = CurrentPlatformSim->GetDisplacementParameter(1);
				double dz   = CurrentPlatformSim->GetDisplacementParameter(5);
			
				for(int i=0;i<6;i++) 
					fprintf(fOutputFile,"%+22.16le ",CurrentPlatformSim->GetDisplacementParameter(i));
				for(int i=0;i<6;i++) 
					fprintf(fOutputFile,"%+22.16le ",CurrentPlatformSim->GetDisplacementParameterError(i));
				fprintf(fOutputFile,"\n");
				if(fMode==kSimulMode) {
					for(int i=0;i<6;i++) 
						fprintf(fSimulationFile,"%+22.16le ",CurrentPlatformMes->GetSimulatedDisplacementParameter(i));
					fprintf(fSimulationFile,"\n");				
				}
				if(fPrintLevel >=1) gGMSGui->PrintOut("\t%+.3e  \t\t%+.3e  \t\t%+.3e	",dthx,dthy,1.e3*dz);
// 				if(fPrintLevel >=1) printf("\t%+.3e \t %+.3e \t %+.3e \n"
// 					,CurrentPlatformSim->GetDisplacementParameter(0)
// 					,CurrentPlatformSim->GetDisplacementParameter(1)
// 					,1.e3*CurrentPlatformSim->GetDisplacementParameter(5));
			}
			if(fPrintLevel >=1) gGMSGui->PrintOut("------------------------------------------------------------");
			fprintf(fOutputFile,"\n");

			fclose(fOutputFile);			
			if(fMode==kSimulMode) {
				fprintf(fSimulationFile,"\n");
				fclose(fSimulationFile);			
			}
		}
		iBlocTMS++;

	} // END : Loop over the Blocs

}
//_______________________________________________________________________________________________________________

/// Initialisation of the Minuit Fitter for the LMS
void GMSAnalyser::InitFitterLMS(Double_t * param){

	Int_t NChamber = gSimulatedSpectrometer->GetNChamber();
	Int_t NbParam;
	Char_t ParamName[10];
	Char_t varname[10];
	Double_t step = 0.0001;	 
	
	// create minimizer class with desired algorithm
	//----------------------------------------------
 	fFitterLMS = ROOT::Math::Factory::CreateMinimizer(fMinimizerType.Data());
	
	if (!fFitterLMS) AliFatal("Enable to create Minimizer object\n");
	
	// create the chi2 function
	//-------------------------
	int nMeas = 0;
	int nBloc =0;
	int BlocID[20], nLinePerBloc[20], nMeasPerLine[20];
	
    for(int i=0;i<gSimulatedSpectrometer->GetNBloc();i++) {
		if (gSimulatedSpectrometer->GetBloc(i)->GetBlocType() != Bloc::kBlocLMS) continue;
		BlocID[nBloc] = i;
		nLinePerBloc[nBloc] = gSimulatedSpectrometer->GetNLine(i);
		for(int j=0;j<nLinePerBloc[nBloc];j++) {
	  		//if(gSimulatedSpectrometer->IsLineDead(i,j)) continue; // Be sure that this given line is not dead
			nMeasPerLine[nBloc] = gSimulatedSpectrometer->GetNMeasurePerLine(i) ;
			nMeas += nMeasPerLine[nBloc];
		}
		nBloc++;
	}
	int nParam = 6*NChamber;
	fChi2LMS  = new LMSChi2Function(nParam,nMeas,nBloc,BlocID,nLinePerBloc,nMeasPerLine);
	
	// set minimizer function (chi2) 
	// for minuit it needs to be set before setting the variables
	//-----------------------------------------------------------
	fFitterLMS->SetFunction(*fChi2LMS); 
	
	
	for (int iCh=0;iCh<NChamber;iCh++){
		for(int iVar=0;iVar<6;iVar++){
			NbParam = 6*iCh+iVar;
			sprintf(ParamName,"%s",GetLMSVariableName(iCh, iVar));
 			fFitterLMS->SetVariable(NbParam,ParamName,param[NbParam],step);
			if(iCh==0) fFitterLMS->SetFixedVariable(NbParam,ParamName,param[NbParam]); //Example of how to fix one parameter
		}
	}
	fChi2LMS->NFree(fFitterLMS->NFree());

	fFitterLMS->SetPrintLevel(fPrintLevel);
	fFitterLMS->SetMaxFunctionCalls(10000);

			
}
//_________________________________________________________________________________________________________________________________
//
///// Initialisation of the Minuit Fitter for the LMS
void GMSAnalyser::InitFitterLMSMixed(Double_t * param){
//
//	Int_t NChamber = gSimulatedSpectrometer->GetNChamber();
//	Int_t NbParam;
//	Char_t ParamName[10];
//	Char_t varname[10];
//	Double_t step = 0.0001;	 
//	Double_t arglist[10];
//	
// 	fFitterLMS = new TFitter(6*NChamber);
//	fFitterLMS->SetFitMethod("chisquare");
//
//	arglist[0] = fPrintLevel - 2;
//	fFitterLMS->ExecuteCommand("SET PRI",arglist,1);
//	fFitterLMS->ExecuteCommand("SET NOW",arglist,0);
//
//	arglist[0] = 0.1;
//	fFitterLMS->ExecuteCommand("SET ERR",arglist,1);
//	
//	for (int iCh=0;iCh<NChamber;iCh++){
//		for(int j=0;j<6;j++){
//			NbParam = 6*iCh+j;
//			switch(j){
//				case 0:
//					sprintf(varname,"thX");
//					break;
//				case 1:
//					sprintf(varname,"thY");
//					break;
//				case 2:
//					sprintf(varname,"thZ");
//					break;
//				case 3:
//					sprintf(varname,"X");
//					break;
//				case 4:
//					sprintf(varname,"Y");
//					break;
//				case 5:
//					sprintf(varname,"Z");
//					break;
//			}
//			sprintf(ParamName,"Ch%d_%s",iCh,varname);
//// 			fFitterLMS->SetParameter(NbParam,ParamName,param[NbParam],step,0.,0.);
//			fFitterLMS->SetParameter(NbParam,ParamName,fFitterGlobal->GetParameter(iCh * 30 + j),step,0.,0.);
//			
//		}
//		
//	}
//	for (int iCh=0;iCh<NChamber;iCh++){
//		if(iCh==0) 
//			for(int j=0;j<6;j++){
//				NbParam = 6*iCh+j;
// 				fFitterLMS->FixParameter(NbParam);
//			}
//		else fFitterLMS->FixParameter(6*iCh + 5); // Fix the Z of all Chamber it is find by the global fit 
//
//	}
//			
//	fFitterLMS->SetFCN(FCN_LMS);
}

//_________________________________________________________________________________________________________________________________

/// Initialisation of the Minuit Fitter for the EMS
void GMSAnalyser::InitFitterEMS(Double_t * param){
/// @todo A faire
}

//_________________________________________________________________________________________________________________________________

/// Initialisation of the Minuit Fitter for the TMS
void GMSAnalyser::InitFitterTMS(Double_t * paramTMS){
	//Info("GMSAnalyser::InitFitterTMS","Start\n");
	int iBlocTMS =0;
	char ParamName[10];
	char varname[10];

	Double_t step = 0.0001;	 
	int NbParam;
	
 	fFitterTMS = new ROOT::Math::Minimizer*[gMeasuredSpectrometer->GetNBlocTMS()];
	
	fChi2TMS  = new TMSChi2Function*[gMeasuredSpectrometer->GetNBlocTMS()];
	
	for (int iBloc=0;iBloc<gMeasuredSpectrometer->GetNBloc();iBloc++){
		if(gMeasuredSpectrometer->GetBloc(iBloc)->GetBlocType() != Bloc::kBlocTMS) continue;	
  		
		// create minimizer class with desired algorithm
		//--------------------------------------------------------------
		
 		fFitterTMS[iBlocTMS] =  ROOT::Math::Factory::CreateMinimizer(fMinimizerType.Data());
		if (!fFitterTMS[iBlocTMS]) AliFatal("Enable to create Minimizer object\n");
		
		// create the chi2 function
		//--------------------------------
		int nLine = gMeasuredSpectrometer->GetNLine(iBloc);
		int nMeasPerLine = gMeasuredSpectrometer->GetNMeasurePerLine(iBloc);
		int nMeas = nLine * nMeasPerLine;
		int nParam = 24; // 6 parameters per platform
		fChi2TMS[iBlocTMS]  = new TMSChi2Function(nParam,nMeas,iBloc,nLine,nMeasPerLine);
		
		
		// set minimizer function (chi2) 
		// for minuit it needs to be set before setting the variables
		//-----------------------------------------------------------
		fFitterTMS[iBlocTMS]->SetFunction(*(fChi2TMS[iBlocTMS])); 
		
		
		// set minimization parameters
		//-----------------------------------------------------------
		for (int iPlatform=0;iPlatform<4;iPlatform++){			
			for(int iVar=0;iVar<6;iVar++){
				NbParam = 6*iPlatform+iVar;
 				fFitterTMS[iBlocTMS]->SetVariable(NbParam,GetTMSVariableName(iPlatform,iVar),paramTMS[NbParam],step);
			}
		}
		if(GetTMSMode() == GMSAnalyser::kThreeZFixed) //Z of 3 plateforms fixed
		{
			for (int iPlatform=3;iPlatform>=0;iPlatform--){
				if((iPlatform!=fTMSRefPlatform)){ 
					int NbParam = 6*iPlatform+5;
					fFitterTMS[iBlocTMS]->SetFixedVariable(NbParam,GetTMSVariableName(iPlatform,5),paramTMS[NbParam]);
				}
				for(int j=4;j>=2;j--) {// On fixe les deplacements en X et Y et ThetaZ
					int NbParam = 6*iPlatform+j;
					fFitterTMS[iBlocTMS]->SetFixedVariable(NbParam,GetTMSVariableName(iPlatform,j),paramTMS[NbParam]);
				}
			}
		}else{ // // Une Plateforme Fixe
			for (int iPlatform=3;iPlatform>=0;iPlatform--){
//				if((iPlatform==fTMSRefPlatform)){ 
//					for(int j=5;j>=0;j--) {
//						int NbParam = 6*iPlatform+j;
//						fFitterTMS[iBlocTMS]->SetFixedVariable(NbParam,GetTMSVariableName(iPlatform,j),paramTMS[NbParam]);
//					}
//				} else{
					for(int j=4;j>=2;j--) {// On fixe les deplacements en X et Y et ThetaZ
						int NbParam = 6*iPlatform+j;
						fFitterTMS[iBlocTMS]->SetFixedVariable(NbParam,GetTMSVariableName(iPlatform,j),paramTMS[NbParam]);
					}
				//}
			}
		}	
				
				
// // All platforms fixed
// 		for (int iPlatform=3;iPlatform>=0;iPlatform--){
// 			for(int j=5;j>=0;j--) {
// 				int NbParam = 6*iPlatform+j;
// 				fFitterTMS[iBlocTMS]->FixParameter(NbParam);
// 			}
// 		}
		

// // Tous les parametres libres
// 		for (int iPlatform=3;iPlatform>=0;iPlatform--){
// 			for(int j=4;j>=2;j--) {// On fixe les deplacements en X et Y et ThetaZ
// 				int NbParam = 6*iPlatform+j;
// 				fFitterTMS[iBlocTMS]->FixParameter(NbParam);
// 			}
// 		}
		
		
		fFitterTMS[iBlocTMS]->SetPrintLevel(fPrintLevel);
		fFitterTMS[iBlocTMS]->SetMaxFunctionCalls(100000);

		
		iBlocTMS++;
	}  
}
//_________________________________________________________________________________________________________________________________

/// Initialisation of the Minuit Fitter for the TMS
void GMSAnalyser::InitFitterTMSMixed(Double_t * paramTMS){
//	int iBlocTMS =0;
//	char ParamName[10];
//	char varname[10];
//
//	Double_t step = 0.0001;	 
//	int NbParam;
//	Double_t arglist[10];
//
// 	fFitterTMS = new TFitter*[gMeasuredSpectrometer->GetNBlocTMS()];
//
//	for (int iBloc=0;iBloc<gMeasuredSpectrometer->GetNBloc();iBloc++){
//		if(gMeasuredSpectrometer->GetBloc(iBloc)->GetBlocType() != Bloc::kBlocTMS) continue;	
//  		
// 		fFitterTMS[iBlocTMS] = new TFitter(24);
//		fFitterTMS[iBlocTMS]->SetFitMethod("chisquare");
//
//		arglist[0] = fPrintLevel - 2;
//		fFitterTMS[iBlocTMS]->ExecuteCommand("SET PRI",arglist,1);
//		fFitterTMS[iBlocTMS]->ExecuteCommand("SET NOW",arglist,0);
//
//		arglist[0] = 1.;
//		fFitterTMS[iBlocTMS]->ExecuteCommand("SET ERR",arglist,1);
//	
//		for (int iPlatform=0;iPlatform<4;iPlatform++){			
//			for(int j=0;j<6;j++){
//				NbParam = 6*iPlatform+j;
//				switch(j){
//					case 0:
//						sprintf(varname,"thX");
//						break;
//					case 1:
//						sprintf(varname,"thY");
//						break;
//					case 2:
//						sprintf(varname,"thZ");
//						break;
//					case 3:
//						sprintf(varname,"X");
//						break;
//					case 4:
//						sprintf(varname,"Y");
//						break;
//					case 5:
//						sprintf(varname,"Z");
//						break;
//				}
//				sprintf(ParamName,"Plat%d_%s",iPlatform,varname);
//// 				fFitterTMS[iBlocTMS]->SetParameter(NbParam,ParamName,paramTMS[NbParam],step,0.,0.);
//				fFitterTMS[iBlocTMS]->SetParameter(NbParam,ParamName,fFitterGlobal->GetParameter(iBloc * 30 + (iPlatform +1) *6 + j),step,0.,0.);
//			}
//		}
//// // 				
//// // Z of 1 plateform fixed
//// 		for (int iPlatform=3;iPlatform>=0;iPlatform--){
//// 			if((iPlatform==0)){ // On fixe en plus le Z pour les plateforme 0
//// 				int NbParam = 6*iPlatform+5;
//// 				fFitterTMS[iBlocTMS]->FixParameter(NbParam);
//// 			}
//// 			for(int j=4;j>=2;j--) {// On fixe les deplacements en X et Y et ThetaZ
//// 				int NbParam = 6*iPlatform+j;
//// 				fFitterTMS[iBlocTMS]->FixParameter(NbParam);
//// 			}
//// 		}
//// 				
//// Fixe all Z because it is found by the Global Fit
//		for (int iPlatform=3;iPlatform>=0;iPlatform--){
//			for(int j=5;j>=2;j--) {// On fixe les deplacements en X et Y et ThetaZ
//				int NbParam = 6*iPlatform+j;
//				fFitterTMS[iBlocTMS]->FixParameter(NbParam);
//			}
//		}
//				
//// // All platforms fixed
//// 		for (int iPlatform=3;iPlatform>=0;iPlatform--){
//// 			for(int j=5;j>=0;j--) {
//// 				int NbParam = 6*iPlatform+j;
//// 				fFitterTMS[iBlocTMS]->FixParameter(NbParam);
//// 			}
//// 		}
//		
//// // Une Plateforme Fixe
//// 		for (int iPlatform=3;iPlatform>=0;iPlatform--){
//// 			if((iPlatform==0)){ // On fixe la plateforme 0
//// 				for(int j=5;j>=0;j--) {
//// 					int NbParam = 6*iPlatform+j;
//// 					fFitterTMS[iBlocTMS]->FixParameter(NbParam);
//// 				}
//// 			} else{
//// 				for(int j=4;j>=2;j--) {// On fixe les deplacements en X et Y et ThetaZ
//// 					int NbParam = 6*iPlatform+j;
//// 					fFitterTMS[iBlocTMS]->FixParameter(NbParam);
//// 				}
//// 			}
//// 		}
//
//// // Tous les parametres libres
//// 		for (int iPlatform=3;iPlatform>=0;iPlatform--){
//// 			for(int j=4;j>=2;j--) {// On fixe les deplacements en X et Y et ThetaZ
//// 				int NbParam = 6*iPlatform+j;
//// 				fFitterTMS[iBlocTMS]->FixParameter(NbParam);
//// 			}
//// 		}
//		
//		fFitterTMS[iBlocTMS]->SetFCN(FCN_TMS);
//		iBlocTMS++;
//	}  
}
//_________________________________________________________________________________________________________________________________

/// Initialisation of the Minuit Fitter for the Global fit
void GMSAnalyser::InitFitterGlobal(Double_t * param){

	Int_t NChamber = gSimulatedSpectrometer->GetNChamber();
	Int_t NbParam;
	Char_t ParamName[20];
	Char_t varname[20];
	Double_t step = 0.001;	 
	Double_t arglist[10];
	
 	fFitterGlobal = new TFitter((30)*NChamber);
	fFitterGlobal->SetFitMethod("chisquare");

	arglist[0] = fPrintLevel - 1;
	fFitterGlobal->ExecuteCommand("SET PRI",arglist,1);
	fFitterGlobal->ExecuteCommand("SET NOW",arglist,0);

	arglist[0] = 1.;
	fFitterGlobal->ExecuteCommand("SET ERR",arglist,1);
	
	for (int iCh=0;iCh<NChamber;iCh++){
		for(int j=0;j<6;j++){
			NbParam = 30*iCh+j;
			switch(j){
				case 0:
					sprintf(varname,"thX");
					break;
				case 1:
					sprintf(varname,"thY");
					break;
				case 2:
					sprintf(varname,"thZ");
					break;
				case 3:
					sprintf(varname,"X");
					break;
				case 4:
					sprintf(varname,"Y");
					break;
				case 5:
					sprintf(varname,"Z");
					break;
			}
			sprintf(ParamName,"Ch%d_%s",iCh,varname);
			fFitterGlobal->SetParameter(NbParam,ParamName,param[NbParam],step,0.,0.);

		}
		for (int iPl=0;iPl<4;iPl++){
			for(int j=0;j<6;j++){
				NbParam = 30*iCh+6*(iPl+1)+j;
				switch(j){
					case 0:
						sprintf(varname,"thX");
						break;
					case 1:
						sprintf(varname,"thY");
						break;
					case 2:
						sprintf(varname,"thZ");
						break;
					case 3:
						sprintf(varname,"X");
						break;
					case 4:
						sprintf(varname,"Y");
						break;
					case 5:
						sprintf(varname,"Z");
						break;
				}
				sprintf(ParamName,"Ch%d_Plat%d_%s",iCh,iPl,varname);
				fFitterGlobal->SetParameter(NbParam,ParamName,param[NbParam],step,0.,0.);

			}
		}	
	}
	for (int iCh=0;iCh<NChamber;iCh++){
		if(iCh==0) 
			for(int j=0;j<6;j++){
				NbParam = 30*iCh+j;
 				fFitterGlobal->FixParameter(NbParam); // Fix One chamber
			}
		for (int iPl=0;iPl<4;iPl++){
			for(int j=2;j<5;j++){
				NbParam = 30*iCh+6*(iPl+1)+j;
				fFitterGlobal->FixParameter(NbParam); // Fix the ThZ, X and Y of the Platforms
			}
			if(iPl==0) fFitterGlobal->FixParameter(30*iCh+6*(iPl+1)+5); // Fix the Z of One of the platforms
		}
	}
			
	fFitterGlobal->SetFCN(FCN_Global);
}


//_________________________________________________________________________________________________________________________________

void GMSAnalyser::SetBreakDownLines(){
	int NbPannes = 0;
	/*			OBLIGATOIRE			*/
	/* reinitialisation a false de toute les lignes en panne */
	for(int r=0;r<gSimulatedSpectrometer->GetNBloc();r++) {
		for(int s=0;s<gSimulatedSpectrometer->GetNLine(r);s++) {
			gSimulatedSpectrometer->SetLineFunctional(r,s);
		}
	}
	/* choix aleatoire des lignes en panne */
	int nFBloc = gSimulatedSpectrometer->GetFirstIntBlocID();
	int nLBloc = gSimulatedSpectrometer->GetLastIntBlocID();
//        cout << "nFBloc ="<<nFBloc<<endl;
//        cout << "nLBloc ="<<nLBloc<<endl;
	int nbLigne;
	int numBloc;
	int numLigne=0;
	   
	for (int ipannes=0;ipannes<NbPannes;ipannes++){
		do {
			numBloc = nFBloc + (int)(fRandObj->Rndm()*(Double_t)(nLBloc - nFBloc + 1.));
			nbLigne = gSimulatedSpectrometer->GetNLine(numBloc);
			numLigne = (int)(fRandObj->Rndm()*(Double_t)nbLigne);
		} while (gSimulatedSpectrometer->IsLineFunctional(numBloc,numLigne));
		gSimulatedSpectrometer->SetLineBreakdown(numBloc,numLigne);
	}
}

//_________________________________________________________________________________________________________________________________
/// Read the Parameters of the Analyser from a file
void GMSAnalyser::ReadSettings()
{
 	if(!(fSettingsFile = fopen(fSettingsFileName.Data(),"r")))
		AliFatal(Form("Enable to open settings file %s\n",fSettingsFileName.Data()));
	
	TString strDump;
	
    int Counter = 0;
	char line[256];
	int idump;
	char cdump[256];
    while ( fgets ( line, sizeof(line), fSettingsFile ) != NULL ) {
        if( line [0] != '#') {
			switch (Counter){
				case 0: // Run Number
					sscanf ( line, "%d",&idump);
					SetPrintLevel(idump);
					AliDebug(1,Form("Print Level : %d",GetPrintLevel()));
					break;
				case 1: // Running Mode
					sscanf ( line, "%d",&idump);
					SetMode((GMSAnalyser::GMSAnalyserMode)idump);
					AliDebug(1,Form("Mode : %d",GetMode()));
					break;
				case 2:
					sscanf ( line, "%d",&idump);
					SetRunNumber(idump +1);
					AliDebug(1,Form("Run : %d ",GetRunNumber()));
					break;
				case 3:
					sscanf ( line, "%d",&idump);
					SetTMSMode((GMSAnalyser::TMSMode)idump);
					AliDebug(1,Form("TMS Mode  : %d",GetTMSMode()));
					break;
				case 4:
					sscanf ( line, "%s",&cdump);
					strDump = GetGMSAnalyserRootDir(); strDump += "/"; strDump += cdump;
					SetRawDataDir((char*)strDump.Data());
					AliDebug(1,Form("Raw Data Dir : %s",GetRawDataDir()));
					break;
				case 5:
					sscanf ( line, "%s",&cdump);
					strDump = GetGMSAnalyserRootDir(); strDump += "/"; strDump += cdump;
					SetOutputDir((char*)strDump.Data());
					AliDebug(1,Form("Output Dir : %s",GetOutputDir()));
					break;
				case 6:
					sscanf ( line, "%s",&cdump);
					strDump = GetGMSAnalyserRootDir(); strDump += "/"; strDump += cdump;
					SetChamberDBFileName((char*)strDump.Data());
					AliDebug(1,Form("Chamber Database : %s",GetChamberDBFileName()));
					break;
				case 7:
					sscanf ( line, "%s",&cdump);
					printf("%s",cdump);
					strDump = GetGMSAnalyserRootDir(); strDump += "/"; strDump += cdump;
					SetPlatformDBFileName((char*)strDump.Data());
					AliDebug(1,Form("Platform Database : %s",GetPlatformDBFileName()));
					break;
				case 8:
					sscanf ( line, "%s",&cdump);
					strDump = GetGMSAnalyserRootDir(); strDump += "/"; strDump += cdump;
					SetBlocDBFileName((char*)strDump.Data());
					AliDebug(1,Form("Line Database : %s",GetBlocDBFileName()));
					break;
				case 9:
					sscanf ( line, "%s",&cdump);
					strDump = GetGMSAnalyserRootDir(); strDump += "/"; strDump += cdump;
					SetSigmasFileName((char*)strDump.Data());
					AliDebug(1,Form("Sigmas File : %s",GetSigmasFileName()));
					break;
				case 10:
					sscanf ( line, "%s",&cdump);
					SetMinimizerType(cdump);
					AliDebug(1,Form("GMSAnalyser::ReadSettings","Minimizer Type : %s",GetMinimizerType()));
					if(strcmp(cdump,"Minuit") * strcmp(cdump,"Minuit2") * strcmp(cdump, "Fumili2") != 0 )
						AliFatal(Form("Minimizer type %s unknown. We only support Minuit, Minuit2 and Fumili2",cdump));
						
					break;
			}
			Counter++;
		}
	}

    fclose ( fSettingsFile );
	
}

//_________________________________________________________________________________________________________________________________

/// Save current parameters of the analyser to a file
void GMSAnalyser::SaveSettings()
{
 	if(!(fSettingsFile = fopen(fSettingsFileName.Data(),"w")))
		Fatal("GMSAnalyser::SaveSettings","Enable to open settings file %s\n",fSettingsFileName.Data());
    TString Dir;
	fprintf(fSettingsFile,"# Print Level \n");
	fprintf(fSettingsFile,"#  Off	 -> 0\n");
	fprintf(fSettingsFile,"#  Light  -> 1\n");
	fprintf(fSettingsFile,"#  Full   -> 2\n");
	fprintf(fSettingsFile,"#			 \n");
	fprintf(fSettingsFile,"%d\n",GetPrintLevel());
	
	fprintf(fSettingsFile,"# Running Mode\n");
	fprintf(fSettingsFile,"#  Monitoring -> 0\n");
	fprintf(fSettingsFile,"#  Alignment  -> 1\n");
	fprintf(fSettingsFile,"#  Simulation -> 2\n");
	fprintf(fSettingsFile,"#\n");
	fprintf(fSettingsFile,"%d\n",GetMode());
	
	fprintf(fSettingsFile,"#			 \n");
	fprintf(fSettingsFile,"# Last Run Number\n");
	fprintf(fSettingsFile,"%d\n",GetRunNumber());
	
	fprintf(fSettingsFile,"#			 \n");
	fprintf(fSettingsFile,"# TMS Mode  \n");
	fprintf(fSettingsFile,"#  One Platform Fixed	  -> 0\n");
	fprintf(fSettingsFile,"#  Z of 3 platforms fixed  -> 1\n");
	fprintf(fSettingsFile,"#			 \n");
	fprintf(fSettingsFile,"%d\n",GetTMSMode());
	
	fprintf(fSettingsFile,"#								\n");
	fprintf(fSettingsFile,"# Directory of the Raw Data files\n");
	fprintf(fSettingsFile,"#								\n");
	Dir = GetRawDataDir();
	Dir.Remove(0,strlen(GetGMSAnalyserRootDir())+1);
	fprintf(fSettingsFile,"%s\n",Dir.Data());
	
	fprintf(fSettingsFile,"#							  \n");
	fprintf(fSettingsFile,"# Directory of the output files\n");
	fprintf(fSettingsFile,"#							  \n");
	Dir = GetOutputDir();
	Dir.Remove(0,strlen(GetGMSAnalyserRootDir())+1);
	fprintf(fSettingsFile,"%s\n",Dir.Data());
	
	fprintf(fSettingsFile,"#								  \n");
	fprintf(fSettingsFile,"# File Name of the Chamber Database\n");
	fprintf(fSettingsFile,"#								  \n");
	Dir = GetChamberDBFileName();
	Dir.Remove(0,strlen(GetGMSAnalyserRootDir())+1);
	fprintf(fSettingsFile,"%s\n",Dir.Data());
	
	fprintf(fSettingsFile,"#								   \n");
	fprintf(fSettingsFile,"# File Name of the Platform Database\n");
	fprintf(fSettingsFile,"#								   \n");
	Dir = GetPlatformDBFileName();
	Dir.Remove(0,strlen(GetGMSAnalyserRootDir())+1);
	fprintf(fSettingsFile,"%s\n",Dir.Data());
	
	fprintf(fSettingsFile,"#										\n");
	fprintf(fSettingsFile,"# File Name of the Optical Lines Database\n");
	fprintf(fSettingsFile,"#										\n");
	Dir = GetBlocDBFileName();
	Dir.Remove(0,strlen(GetGMSAnalyserRootDir())+1);
	fprintf(fSettingsFile,"%s\n",Dir.Data());
	
	fprintf(fSettingsFile,"#										\n");
	fprintf(fSettingsFile,"# File Name of the Sigmas                \n");
	fprintf(fSettingsFile,"#										\n");
	Dir = GetSigmasFileName();
	Dir.Remove(0,strlen(GetGMSAnalyserRootDir())+1);
	fprintf(fSettingsFile,"%s\n",Dir.Data());
	
	fprintf(fSettingsFile,"#										\n");
	fprintf(fSettingsFile,"# Minimizer Type                         \n");
	fprintf(fSettingsFile,"#										\n");
	fprintf(fSettingsFile,"%s\n",GetMinimizerType());
	
    fclose ( fSettingsFile );
	
}

//_________________________________________________________________________________________________________________________________
/// Initialize the TGeometry (using the TGeoManager class)
void GMSAnalyser::InitGeometry()
{
	//--- Definition of the geometry Manager
	fGeoManager = new TGeoManager("GMS", "GMS Geometry");
	gGMSGeoManager = fGeoManager;
	
	//---> create some materials
	mat = new TGeoMaterial("Vacuum",0,0,0);
	mat->SetUniqueID(0);
	mat1 = new TGeoMaterial("Al", 26.98,13,2.7);
	mat1->SetUniqueID(1);
   
	//---> create mediums
	med0 = new TGeoMedium("Vacuum",0,0,0,0,0,20,0.1000000E+11,0.212,0.1000000E-02,1.150551);
	med1 = new TGeoMedium("Al",1,1,0,0,0,20,0.1000000E+11,0.212,0.1000000E-02,1.150551);
	TOP = gGMSGeoManager->MakeBox("TOP", med0, 200000, 200000, 20000);
	gGMSGeoManager->SetTopVolume(TOP);
	TOP->SetVisibility(kFALSE);
}

//_________________________________________________________________________________________________________________________________
/// Draw the Geometry to the display
void GMSAnalyser::DrawGeometry()
{
	//--- close the geometry
	gGMSGeoManager->CloseGeometry();
		
	//gGMSGeoManager->SetVisLevel(5);
	gGMSGui->GetGeomCanvas()->cd();
	TOP->Draw();
}

//_________________________________________________________________________________________________________________________________

/// FCN function used to calculate the Chi2 during the minimisation process of the TMS Blocs
void GMSAnalyser::FCN_TMS(Int_t &npar,Double_t *grad,Double_t &chi2,Double_t *param,Int_t iflag)
  {
	
    Int_t NMeas =0;
	Int_t iBloc = 	gGMSAnalyser->GetCurrentBloc();
	gSimulatedSpectrometer->InitDepPlatforms(((BlocTMS*)gSimulatedSpectrometer->GetBloc(iBloc))->GetChamber(),
																	(Double_t*)param);

	gSimulatedSpectrometer->CalcFinalImages(iBloc);
		
	chi2 = 0.;
	Double_t f_param;
   for(int j=0;j<gSimulatedSpectrometer->GetNLine(iBloc);j++){
		for(int k=0;k<gSimulatedSpectrometer->GetNMeasurePerLine(iBloc);k++){	
			if(k%2==0) continue; // We do not take into account the measurements in X for the TMS.
						
			f_param = (gMeasuredSpectrometer->GetImageDisplacement(iBloc,j,k) - 
		     	  				gSimulatedSpectrometer->GetImageDisplacement(iBloc,j,k)) 
					/ gMeasuredSpectrometer->GetSigma(iBloc,j,k);
			//cout<<iBloc<<" " <<1e6*gMeasuredSpectrometer->GetImageDisplacement(iBloc,j,k)<<" "<<1e6*gSimulatedSpectrometer->GetImageDisplacement(iBloc,j,k)<<" "<<gMeasuredSpectrometer->GetSigma(iBloc,j,k)<<endl;

 // printf("TMS Line %d Mesure %d   SpectroMes %e   SpectroSim %e  Diff %e\n", j, k,  gMeasuredSpectrometer->GetImageDisplacement(iBloc,j,k) ,
 //  						gSimulatedSpectrometer->GetImageDisplacement(iBloc,j,k),gMeasuredSpectrometer->GetImageDisplacement(iBloc,j,k) - gSimulatedSpectrometer->GetImageDisplacement(iBloc,j,k));
			chi2 += f_param*f_param;
				NMeas++;
		}	  
	}
//exit(0);
	//chi2/=NMeas-8;
// 	printf("Chi2 %e \n",chi2);
	//cout<<chi2<<" "<< TMath::Sqrt(chi2)/gSimulatedSpectrometer->GetNLine(iBloc)/gSimulatedSpectrometer->GetNMeasurePerLine(iBloc)<<endl;
}
//_________________________________________________________________________________________________________________________________

/// FCN function used to calculate the Chi2 during the minimisation process of the global minimization
void GMSAnalyser::FCN_Global(Int_t &npar,Double_t *grad,Double_t &chi2,Double_t *param,Int_t iflag)
  {
    Double_t f_param;
    Int_t NMeas =0;
    
    gSimulatedSpectrometer->InitGlobalDisplacements((double*)param);
    gSimulatedSpectrometer->CalcFinalImages();
    
    chi2 = 0.;
    for(int i=0;i<gSimulatedSpectrometer->GetNBloc();i++) {
		if (!gSimulatedSpectrometer->IsBlocEnabled(i)) continue;
		
		for(int j=0;j<gSimulatedSpectrometer->GetNLine(i);j++) {
	  		if(gSimulatedSpectrometer->IsLineDead(i,j)) continue; // Be sure that this given line is not dead
	    	for(int k=0;k<gSimulatedSpectrometer->GetNMeasurePerLine(i);k++) {
				//if(i==4 && k>1) continue;
	      		f_param = (gMeasuredSpectrometer->GetImageDisplacement(i,j,k) - gSimulatedSpectrometer->GetImageDisplacement(i,j,k)) 
								/  gMeasuredSpectrometer->GetSigma(i,j,k);

//  				printf("Bloc %d Line %d Mesure %d   SpectroMes %e   SpectroSim %e \n",i, j, k,  gMeasuredSpectrometer->GetImageDisplacement(i,j,k) ,
//  						gSimulatedSpectrometer->GetImageDisplacement(i,j,k));
	      		chi2 += f_param*f_param;
				NMeas++;
	    	}
		}
	}
	chi2/=NMeas-45.;
}


//_________________________________________________________________________________________________________________________________
/// FCN function used to calculate the Chi2 during the minimisation process of the EMS Blocs
void GMSAnalyser::FCN_EMS(Int_t &npar,Double_t *grad,Double_t &chi2,Double_t *param,Int_t iflag)
  {
	
	Int_t iBloc = 	gGMSAnalyser->GetCurrentBloc();
	gSimulatedSpectrometer->InitDepPlatforms(((BlocTMS*)gSimulatedSpectrometer->GetBloc(iBloc))->GetChamber(),
																	(Double_t*)param);

	gSimulatedSpectrometer->CalcFinalImages(iBloc);
		
	chi2 = 0.;
	Double_t f_param;
   for(int j=0;j<gSimulatedSpectrometer->GetNLine(iBloc);j++){
		for(int k=0;k<gSimulatedSpectrometer->GetNMeasurePerLine(iBloc);k++){	
			if(k%2==0) continue; // We do not take into account the measurements in X for the TMS.
						
			f_param = (gMeasuredSpectrometer->GetImageDisplacement(iBloc,j,k) - 
		     	  				gSimulatedSpectrometer->GetImageDisplacement(iBloc,j,k)) 
					/ gMeasuredSpectrometer->GetSigma(iBloc,j,k);

			chi2 += f_param*f_param;
		}	  
	}
}

//_________________________________________________________________________________________________________________________________
/// Prints out the running mode 
void GMSAnalyser::PrintMode(){
	if (fPrintLevel==0) return;
	switch(fMode){
		case kSimulMode:
		AliInfo("Starting GMSAnalyser into Simulation Mode");
		break;
		case kMonitMode:
		AliInfo("Starting GMSAnalyser into Monitoring Mode");
		break;
		case kAlignMode:
		AliInfo("Starting GMSAnalyser into Alignment Mode");
		break;
	}
}

//_________________________________________________________________________________________________________________________________
/// Set the name of the input data file
void GMSAnalyser::SetFileIn()
  {
	char run[10];
	sprintf (run,"%04d",fRunNumber);
	fRawDataFileName = fRawDataDir + "/" + "Acquisifier_Run_" + run + ".dat";
	if(fPrintLevel>1 ) Info("GMSAnalyser::SetFileIn","%s",fRawDataFileName.Data());
  }
//_________________________________________________________________________________________________________________________________
/// Set the name of the output files
void GMSAnalyser::SetFileOut()
  {
	char run[10];
	sprintf (run,"%03d",fRunNumber);
	fOutputFileName = fOutputDir + "/" + fOutputFilePrefix + "Run" + run +".root";
	if(fPrintLevel>1 ) Info("GMSAnalyser::SetFileOut"," ROOT Output : %s",fOutputFileName.Data());
	fTextOutputFileName = fOutputDir + "/" + fOutputFilePrefix + "Run" + run +".out";
	if(fPrintLevel>1 ) Info("GMSAnalyser::SetFileOut"," TEXT Output : %s",fTextOutputFileName.Data());
	if(fMode == kSimulMode){
		fSimulationFileName = fOutputDir + "/" + fOutputFilePrefix + "Sim" + run +".out";
		if(fPrintLevel>1 ) Info("GMSAnalyser::SetFileOut"," Simulation File : %s",fSimulationFileName.Data());
	}
  }
//_________________________________________________________________________________________________________________________________
void GMSAnalyser::OpenFileOut()
  {
/// Create the output files (text file and ROOT file)
	char title[50];
	sprintf(title,"GMS ROOT File - Run %03d",fRunNumber);

	fOutputFile = fopen(fTextOutputFileName,"w");
	fclose(fOutputFile);
	if(fMode == kSimulMode){
		fSimulationFile = fopen(fSimulationFileName,"w");
		fclose(fSimulationFile);
	}

  }
//_________________________________________________________________________________________________________________________________
const char * GMSAnalyser::GetLMSVariableName(Int_t iCh, Int_t iVar){
/// Return the name of the parameter used in the fit
	char sCh[5];
	sprintf(sCh,"%02d",iCh);
	TString ParamName = "Ch";
	ParamName += sCh;
	ParamName += "_";
	string varname;
	switch(iVar){
		case 0:
			varname = "thX";
			break;
		case 1:
			varname = "thY";
			break;
		case 2:
			varname = "thZ";
			break;
		case 3:
			varname = "X";
			break;
		case 4:
			varname = "Y";
			break;
		case 5:
			varname = "Z";
			break;
	}
	ParamName += varname;
	return ParamName.Data();		
}
//_________________________________________________________________________________________________________________________________
const char * GMSAnalyser::GetTMSVariableName(Int_t iPlatform, Int_t iVar){
	/// Return the name of the parameter used in the fit
	TString ParamName = "Plat";
	ParamName += iPlatform;
	string varname;			
	switch(iVar){
		case 0:
			varname="thX";
			break;
		case 1:
			varname="thY";
			break;
		case 2:
			varname="thZ";
			break;
		case 3:
			varname="X";
			break;
		case 4:
			varname="Y";
			break;
		case 5:
			varname="Z";
			break;
	}
	ParamName += varname;
	return ParamName.Data();		
}
//__________________________________________________________________________
void GMSAnalyser::Clean(){
	Int_t nChamber = gSimulatedSpectrometer->GetNChamber();
	
	delete fChi2LMS;
	delete fFitterLMS;
	delete [] fChi2TMS;
	delete [] fFitterTMS;
	for(int i=0;i<nChamber;i++) delete [] fSimulParamTMS[i];
	fSimulParamLMS.Reset();
	fSimulParamLMS.Set(0);
	
}

//_________________________________________________________________________________________________________________________________
/// Start the DIM Client
void *StartDimClient(void *arg)
  {
	int value = 0;
	GMSDimClient GMS("GMS_DIM_SERVICE");

	while(1)
	{
		printf("Cycle Number: %d \n" ,value);
		value++;
		sleep(30);
	}

  }
//_________________________________________________________________________________________________________________________________
//_________________________________________________________________________________________________________________________________
//_________________________________________________________________________________________________________________________________

int main(int argc, char *argv[])
{
	TApplication * theApp = new TApplication("GMSAnalyser", &argc, argv);
	gGMSApplication = theApp;
	//TCanvas *Canvas = new TCanvas("c1","c1");
	//Canvas->cd();
	GMSAnalyser TheGMSAnalyser(gClient->GetRoot(), 400, 220);
	//mainWindow = new GMSGui(gClient->GetRoot(), 400, 220);
	
	AliLog::SetGlobalDebugLevel(gGMSAnalyser->GetPrintLevel());
	
	gGMSAnalyser->InitGeometry();
	gGMSAnalyser->CreateSpectro();
	gGMSAnalyser->DrawGeometry();
	
    TThread *DimClientThread = new TThread("DimClientThread", StartDimClient, (void *) 0);
    DimClientThread->Run();
	
	theApp->Run();  
	
	DimClientThread->Kill();
	
	return EXIT_SUCCESS;
}
