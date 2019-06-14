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
#ifndef GMSANALYSER_H
#define GMSANALYSER_H
#ifndef ROOT_DllImport
#include "DllImport.h"
#endif

#include <TObject.h>

class GMSGui;
class Spectro;
class TGeoManager;
class TGeoMaterial;
class TGeoMedium;
class TGeoVolume;
class TGeoHMatrix;
class TGWindow;
class TString;
class TRandom3;
class TFitter;
class TFile;
class TTree;
class TBranch;
class TStopwatch;
class TClonesArray;
class TApplication;
class TObjString;
class LMSChi2Function;
class TMSChi2Function;
#include <Math/Minimizer.h>
#include <TArrayD.h>


/**
@author Raphael Tieulent
*/
//!  Class which describs the entire GMS Analyser program
/*!
This class call its Minimize method which is doing the entire minimization.
*/
class GMSAnalyser : public TObject{

public:
	enum GMSAnalyserMode{
		kMonitMode,
		kAlignMode,
		kSimulMode,
		kSigmaMode
	};

	enum TMSMode{
		kOnePlatformFixed,
		kThreeZFixed
	};

private:
	Spectro *fSimulatedSpectrometer ;
	Spectro *fMeasuredSpectrometer;
	GMSGui * fMainGui;
	GMSAnalyserMode fMode;
	Int_t fRunNumber;
	Int_t fNEvent;
	TString fRawDataDir;
	TString fOutputDir;
	TString fOutputFilePrefix;
	TString fGMSAnalyserRootDir;
	TString fRawDataFileName;
	TString fSigmasFileName;
	TString fOutputFileName;
	TString fSimulationFileName;
	TString fSettingsFileName;
	TString fTextOutputFileName;
	TString fChamberDBFileName;
	TString fPlatformDBFileName;
	TString fBlocDBFileName;
	TString fMinimizerType;

	Int_t fPrintLevel;
	TGeoManager *fGeoManager;
    TStopwatch *fTimer;
	
	TMSMode fTMSMode;
	Int_t fTMSRefPlatform;
	
	TArrayD fSimulParamLMS ;
	Double_t **fSimulParamTMS ;
	Double_t *fSimulParamGlobal ;

	
	FILE *fRawDataFile;
	FILE *fOutputFile;
	FILE *fSimulationFile;
	FILE *fSettingsFile;
	FILE *fSigmasFile;
	TRandom3 * fRandObj;
	
	//TFitter **fFitterTMS;
	
	ROOT::Math::Minimizer **fFitterTMS;
	TMSChi2Function ** fChi2TMS;
	
	ROOT::Math::Minimizer *fFitterLMS;
	LMSChi2Function * fChi2LMS;
	TFitter *fFitterEMS;
	TFitter *fFitterGlobal;
	
	TFile * fFileOut;
	TTree * fTreeOut;
	
		
	Int_t fCurrentBloc; /// Use at the minimization stage to know which bloc is currently treat
	
	static void FCN(Int_t &npar,Double_t *grad,Double_t &chi2,Double_t *param,Int_t iflag);
	static void FCN_ext(Int_t &npar,Double_t *grad,Double_t &chi2,Double_t *param,Int_t iflag);
	static void FCN_ext2(Int_t &npar,Double_t *grad,Double_t &chi2,Double_t *param,Int_t iflag);
	static void FCN2(Int_t &npar,Double_t *grad,Double_t &chi2,Double_t *param,Int_t iflag);
	static void FCN_int_gauche(Int_t &npar,Double_t *grad,Double_t &chi2,Double_t *param,Int_t iflag);
	static void FCN_int_droite(Int_t &npar,Double_t *grad,Double_t &chi2,Double_t *param,Int_t iflag);
	static void FCN_int_inter(Int_t &npar,Double_t *grad,Double_t &chi2,Double_t *param,Int_t iflag);
	static void FCN_int(Int_t &npar,Double_t *grad,Double_t &chi2,Double_t *param,Int_t iflag);
	static void FCN_TMS(Int_t &npar,Double_t *grad,Double_t &chi2,Double_t *param,Int_t iflag);
	static void FCN_EMS(Int_t &npar,Double_t *grad,Double_t &chi2,Double_t *param,Int_t iflag);
	static void FCN_Global(Int_t &npar,Double_t *grad,Double_t &chi2,Double_t *param,Int_t iflag);

	void Clean();
	
public:
	GMSAnalyser(const TGWindow *p, UInt_t w, UInt_t h);
	~GMSAnalyser();
	
	inline GMSAnalyserMode GetMode()const {return fMode;};
	inline void SetMode(GMSAnalyserMode mode){fMode = mode;};
	
	inline TMSMode GetTMSMode()const {return fTMSMode;};
	inline void SetTMSMode(TMSMode mode){fTMSMode = mode;};
	
	inline Int_t GetTMSRefPlatform()const {return fTMSRefPlatform;};
	inline void SetTMSRefPlatform(Int_t ref){fTMSRefPlatform = ref;};
	
	inline int  GetPrintLevel()const {return fPrintLevel;};
	inline void SetPrintLevel(int level){fPrintLevel = level;};
	inline const char * GetOutputDir()const {return fOutputDir.Data();};
	inline const char * GetRawDataDir()const {return fRawDataDir.Data();};
	inline void SetOutputDir(char * dir) {fOutputDir = dir;};
	inline void SetRawDataDir(char * dir) {fRawDataDir = dir;};
	inline Int_t GetCurrentBloc() const {return fCurrentBloc;};
	inline TFile * GetFileOut() const {return fFileOut;};	
	inline TTree * GetTree() const {return fTreeOut;};
	inline const char * GetGMSAnalyserRootDir()const {return fGMSAnalyserRootDir.Data();};
	inline const char * GetChamberDBFileName()const {return fChamberDBFileName.Data();};
	inline void SetChamberDBFileName(const char * name) {fChamberDBFileName = name;};
	inline const char * GetPlatformDBFileName()const {return fPlatformDBFileName.Data();};
	inline void SetPlatformDBFileName(const char * name) {fPlatformDBFileName = name;};
	inline const char * GetBlocDBFileName()const {return fBlocDBFileName.Data();};
	inline void SetBlocDBFileName(const char * name) {fBlocDBFileName = name;};
	inline const char * GetSigmasFileName()const {return fSigmasFileName.Data();};
	inline void SetSigmasFileName(const char * name) {fSigmasFileName = name;};
	inline void SetRunNumber(Int_t run) {fRunNumber = run;};
	inline Int_t GetRunNumber() {return fRunNumber;};
	inline void SetNEvent(Int_t evt) {fNEvent = evt;};
	inline Int_t GetNEvent() {return fNEvent;};
	inline Spectro * GetSimulatedSpectro(){return fSimulatedSpectrometer;};
	inline Spectro * GetMeasuredSpectro(){return fMeasuredSpectrometer;};
	inline void SetMinimizerType(const char * name){fMinimizerType = name;};
	inline const char * GetMinimizerType(){return fMinimizerType.Data() ;};

	const char * GetLMSVariableName(Int_t iCh, Int_t iVar);
	const char * GetTMSVariableName(Int_t iPlat, Int_t iVar);

	void Minimize();
	void CreateSpectro();
	void ReadSettings();
	void SaveSettings();
	void InitGeometry();
	void DrawGeometry();
	void MinimizeMonitor();
	void MinimizeGlobal();
	void MinimizeMixed();
	void MinimizeSimul(Int_t iEvent);
	void MinimizeSimulGlobal(Int_t iEvent);
	void SetBreakDownLines();
	void CalculateSigmas();
	
	void InitFitterTMS(Double_t * param);
	void InitFitterLMS(Double_t * param);
	void InitFitterEMS(Double_t * param);
	void InitFitterGlobal(Double_t * param);
	void InitFitterTMSMixed(Double_t * param);
	void InitFitterLMSMixed(Double_t * param);
	
	void FitTMS(Double_t * param);
	void FitLMS(Double_t * param, Int_t PassNumber);
	void FitEMS(Double_t * param);
	void FitGlobal(Double_t * param);
	
	void PrintMode();
	void SetFileIn();
	void OpenFileOut();
	void SetFileOut();
	//ClassDef(GMSAnalyser,1) 

};

extern TBranch *branch;
extern TClonesArray *fOutputArray;	

R__EXTERN GMSAnalyser * gGMSAnalyser;
R__EXTERN TApplication * gGMSApplication;
R__EXTERN Spectro *gSimulatedSpectrometer ;
R__EXTERN Spectro *gMeasuredSpectrometer;

R__EXTERN TGeoManager * gGMSGeoManager;
extern TGeoMaterial *mat;
extern TGeoMedium *med0 ;
extern TGeoMedium *med1 ;
extern TGeoVolume *TOP ;
#endif

