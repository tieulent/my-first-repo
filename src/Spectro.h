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
#ifndef SPECTRO_H
#define SPECTRO_H

#include <TRandom3.h>
#include <TMap.h>

#include "Chamber.h"
#include "Bloc.h"

class TObjArray;
class TMap;

class Wall;
class Box;
class ImageResult;

/** \brief  Base class which describs the spectrometer
	
An object "Spectro" contains the chambers, the walls, the blocs of optical lines and the optical Boxes (BCAM or PROX).
	
© Copyright 2006 IPN Lyon -  Philippe Pillot. All Rights Reserved.

	\author  Philippe Pillot IPN Lyon
	\author Raphael Tieulent
	\date 2004-10-08
	\date 2004-10-08
	\version 1.0
**/

class Spectro : public TObject{

public:
	enum ESpectroType{ kSimulated, kMeasured };

private:
	ESpectroType fType;		//!< Type of spectrometer (simulated spectro = kSimulated; measured = kMeasured)

	Int_t fNbChamber;		//!< Total number of Chamber
	Chamber **fChamber;		//!< Table of pointers to the Chambers

	Int_t fNbWall;			//!< Total number of Wall
	Wall **fWall;			//!< Table of pointers to the Walls

	Int_t fNbBox;			//!< Total number of Boxes
	TObjArray * fBox;				//!< Table of pointers to the Boxes

	Int_t fNbBloc;
	TObjArray * fBloc;

	Bool_t **LineDead; 		//!< BreakDown lines. Is a table of nb_tot_blocs*nb_line elements.
	Bool_t *fBlocEnabled; 	//!< Bloc to be used if kTRUE, kFALSE otherwise. Is a table of nb_tot_blocs elements.

	TMap * fImages;
	TMap * fReferenceImages;

	Int_t fNbExternalBloc;
	Int_t fExternalChamberID;
	Int_t fNBlocTMS;
	Int_t fNBlocLMS;
	Int_t fNBlocEMS;

	void CreateChambers(const char * FileName);
	void CreateBlocs(const char * FileName);
	Int_t GetNbBoxes(const char * FileName);
	Int_t GetNbLines();

public:

	Spectro(const char * ChamberFileName, const char * BlocFileName, ESpectroType Type);
	~Spectro();

	TMap * GetImages(){return fImages;};
	ImageResult * GetImage(TObjString * Name){return (ImageResult *)fImages->GetValue((TObject*)Name);};
	void AddImage(TObjString * Name, ImageResult * Data){fImages->Add((TObject*)Name,(TObject*)Data);};
	
	TMap * GetReferenceImages(){return fReferenceImages;};
	ImageResult * GetReferenceImage(TObjString * Name){return (ImageResult *)fReferenceImages->GetValue((TObject*)Name);};
	void AddReferenceImage(TObjString * Name, ImageResult * Data){fReferenceImages->Add((TObject*)Name,(TObject*)Data);};
	
	/// Set the number of TMS blocs
	void  SetNBlocTMS(Int_t val) {fNBlocTMS = val;};
	/// Return the number of TMS blocs.
	Int_t GetNBlocTMS() const {return fNBlocTMS;};   
	/// Set the number of LMS blocs.
	void  SetNBlocLMS(Int_t val) {fNBlocLMS = val;}; 
	/// Return the number of LMS blocs.
	Int_t GetNBlocLMS() const {return fNBlocLMS;}; 
	/// Set the number of EMS blocs.
	void  SetNBlocEMS(Int_t val) {fNBlocEMS = val;}; 
	/// Return the number of EMS blocs.
	Int_t GetNBlocEMS() const {return fNBlocEMS;};
	
	Int_t GetNChamber()const {return(fNbChamber);};		//!< Return the number of chambers.
	Int_t GetNBloc()const {return(fNbBloc);};//!< Return the total number of bloc.
	Int_t GetFirstExtBlocID()const {return(0);};  //!< Return the number of the first external bloc.
	Int_t GetLastExtBlocID()const {return(fNbExternalBloc-1);}; //!< Return the number of the last external bloc.
	Int_t GetFirstIntBlocID()const{return(fNbExternalBloc);};//!< Return the number of the first internal bloc.
	Int_t GetLastIntBlocID()const{return(fNbBloc-1);};//!< Return the number of the last internal bloc.
	Int_t GetNLine(const Int_t i)const{return(((Bloc*)fBloc->At(i))->GetNLines());}; //!< Return the number of optical lines in the bloc i.
	/// Return the number of measure per optical lines in the bloc i.
	Int_t GetNMeasurePerLine(const Int_t i)const{return(((Bloc*)fBloc->At(i))->GetNMeasuresPerLine());};	
	
	//!< Return the position x(i=0), y(i=1) or z(i=2) of the center of the chamber "num_ch".
	Double_t GetChamberCenter(const Int_t num_ch,const Int_t i)const {return (fChamber[num_ch]->GetCenter(i));}; 
	
	/// Return the angles of frame transformation glob->plan of the chamber "num_ch"(i=0->theta_X, i=1->theta_Y and i=2->theta_Z).
	Double_t GetChamberAngle(const Int_t num_ch,const Int_t i)const{return (fChamber[num_ch]->GetAngle(i));}; 
	
	/// Return the value of image0[num_ligne][num_mes] of the bloc "num_bloc".
	Double_t GetRefImage(const Int_t num_bloc,const Int_t num_ligne,const Int_t num_mes)const{ return(((Bloc*)fBloc->At(num_bloc))->GetRefImage(num_ligne,num_mes));}; 
	
	/// Return the value of image0[num_ligne][num_mes] of the bloc "num_bloc".
	Double_t GetFinalImage(const Int_t num_bloc,const Int_t num_ligne,const Int_t num_mes)const{return(((Bloc*)fBloc->At(num_bloc))->GetFinalImage(num_ligne,num_mes));}; 
	
	/// Return the displacement of the image[num_ligne][num_mes] of the bloc "num_bloc".
	Double_t GetImageDisplacement(const Int_t num_bloc,const Int_t num_ligne,const Int_t num_mes)const{return(((Bloc*)fBloc->At(num_bloc))->GetImageDisplacement(num_ligne,num_mes));};
	
	/// Return the value of sigma_mes[num_ligne][num_mes] of the bloc "num_bloc".
	Double_t GetSigma(const Int_t num_bloc,const Int_t num_ligne,const Int_t num_mes)const{return(((Bloc*)fBloc->At(num_bloc))->GetSigma(num_ligne,num_mes));};
	
	///Set the number of the external chamber.
	void  SetExternalChamberID(const Int_t num_ch){fExternalChamberID = num_ch;}; 
	
	/// Return the number of the external chamber.
	Int_t GetExternalChamberID()const {return fExternalChamberID;}; 
	
	/// Return a pointer to the chamber "ch".
	Chamber * GetChamber(Int_t ch) const {return fChamber[ch];}; 
	
	/// Return a pointer to the chambers table.
	Chamber ** GetChambers() const {return fChamber;}; 
	
	/// Return a pointer to the Bloc "blocid".
	Bloc * GetBloc(Int_t blocid) const {return ((Bloc*)fBloc->At(blocid));};
	
	/// Return a pointer to the Box "boxid".
	Box * GetBox(Int_t boxid) const {return ((Box*)fBox->At(boxid));};
	

	/// Return TRUE if spectro is the measured one
	Bool_t IsMeasured() const {return  (fType == kMeasured) ? kTRUE : kFALSE;}; 

	Chamber * GetChamberByID(Int_t chid) const;
	Wall * GetWall(Int_t wallid) const;
	void Print() const; 
	//void InitChamberPosition(Chamber * ch, Double_t shift[3], Double_t ang[3]); 

void init_dep_glob(Double_t parametres[6]);
void InitDepPlatforms(Chamber * ch, Double_t *parametres);
void InitDepChambers(Double_t *parametres);
void InitGlobalDisplacements(Double_t *parametres);
void init_dep_chambre_ext(Double_t *parametres);
/* ATTENTION : parametres ne contient que les deplacements de la chambre
fExternalChamberID (6 parametres quelque soit le type de chambre : la pliure des 2
demi-plan est un deplacement interne -> ce parametre est initialise par un fit
interne prealable quand on cherche les deplacements de la chambre "externe"). */
void init_dep_chambre_ext2(Double_t *parametres);
void init_dep_chambres_int(Double_t *parametres);

void InitRandomBoxPosition(TRandom3 *rand_obj,const Spectro *SimulSpectro);
void InitRandomWallDeformation(TRandom3 *rand_obj);

inline void SetLineBreakdown(Int_t iBloc, Int_t iLine){LineDead[iBloc][iLine] = kTRUE;};			//!< Declare line iLine of bloc iBloc to be Dead.
inline void SetLineFunctional(Int_t iBloc, Int_t iLine){LineDead[iBloc][iLine] = kFALSE;};			//!< Declare line iLine of bloc iBloc to be Functional.
inline Bool_t IsLineFunctional(Int_t iBloc, Int_t iLine)const {return(!(LineDead[iBloc][iLine]));};	//!< Return kTRUE if line iLine of bloc iBloc is Functional kFALSE otherwise.
inline Bool_t IsLineDead(Int_t iBloc, Int_t iLine)const {return(LineDead[iBloc][iLine]);};			//!< Return kTRUE if line iLine of bloc iBloc is Dead kFALSE otherwise.
inline Bool_t IsBlocEnabled(Int_t iBloc)const {return(fBlocEnabled[iBloc]);};						//!< Return kTRUE if bloc iBloc is enabled kFALSE otherwise.
inline void EnableBloc(Int_t iBloc) {fBlocEnabled[iBloc] = kTRUE;};									//!< Enable bloc iBloc
inline void DisableBloc(Int_t iBloc) {fBlocEnabled[iBloc] = kFALSE;};								//!< Disable bloc iBloc

void ReadRefImages(FILE *fp, Int_t PrintDataFlag);
void ReadFinalImages(FILE *fp, Int_t PrintDataFlag);

void CalcFinalImages(Int_t iBloc);
void PrintFinalImages(Int_t iBloc);
void PrintDisplacements(Int_t iBloc);
void CalcRefImages(Int_t iBloc);
void CalcFinalImages();
void CalcRefImages();
void CalcTheoFinalImages();

void calc_imagef_extTh();
void calc_imagef_ext();
void calc_imagef_int();
void AddIntrinsicError(TRandom3 *rand_obj,Double_t IntResFactor);
/* ATTENTION : le fichier doit etre range comme les blocs les lignes et les
mesures dans spectro -> il vaut mieux calculer les sigma en utilisant les memes
boucles que dans la methode set_sigma_mes */
void ReadSigmas(FILE *sig);
void WriteSigmas(FILE *sig);

void FindTMSLine(TString LineName, int &Bloc, int &Line);
void FindTMSLine(TString * LineName, int &Bloc, int &Line);
void FindLMSPROXLine(TString LineName, int &Bloc, int &Line);
void FindLMSPROXLine(TString * LineName, int &Bloc, int &Line);

 //	ClassDef(Spectro,1) 


};

#endif

