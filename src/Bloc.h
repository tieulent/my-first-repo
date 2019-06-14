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
#ifndef BLOC_H
#define BLOC_H

#include <TObject.h>
class TObjArray;
#include <TRandom3.h>
class TString;
class GMSImage;

/** \brief  Base class describing a bloc of optical line of the same type
	
 A bloc is a group of optical line of the same type (BCAM or PROX). 
 The class Bloc is the base class describing this type of object.
	
© Copyright 2006 IPN Lyon - Raphael Tieulent. All Rights Reserved.

	\author Raphael Tieulent IPN Lyon
	\date 2006-10-08
	\version 1.0	\sa
**/

class Bloc : public TObject{

public:

	enum EBlocType{
		kBlocTMS,   			
		kBlocEMS,
		kBlocLMS
	};
	
protected:

	Int_t fNLines; 					//!< Number of optical lines in this bloc 
	Int_t fNMeasuresPerLine;		//!< Number of measures per line 
	EBlocType fBlocType;			//!< Type of the bloc
	TObjArray * fImages;
	
public:

	Bloc(const Int_t nb_lignes,const Int_t nb_mes_par_ligne);
	virtual ~Bloc();

	// Getters
	TObjArray * GetImages() const {return fImages;};
	GMSImage * GetImage(Int_t img) const {return((GMSImage *) fImages->At(img) ); } ;
	
	void AddImage(GMSImage * img){fImages->Add((TObject *)img);};
	
	virtual void Display() const {};

	inline Int_t GetNLines()const {return(fNLines);};
	inline Int_t GetNMeasuresPerLine()const {return(fNMeasuresPerLine);};
	
	inline EBlocType GetBlocType() const {return fBlocType;};
	inline void SetBlocType(EBlocType type){fBlocType = type;};
	TString GetImageName(const Int_t LineNumber);

	Double_t GetRefImage(const Int_t num_ligne, const Int_t num_mes)const;
	void SetRefImage(const Int_t num_ligne, const Int_t num_mes,Double_t valeur);
	void SetRefImages(const Int_t iLine, Double_t *val);
	
	void SetFinalImage(const Int_t num_ligne, const Int_t num_mes,Double_t valeur);
	Double_t GetFinalImage(const Int_t num_ligne, const Int_t num_mes)const;
	void SetFinalImages(const Int_t iLine, Double_t *val);
	
	Double_t GetImageDisplacement(const Int_t num_ligne, const Int_t num_mes)const;
	void SetImageDisplacement(const Int_t num_ligne, const Int_t num_mes,Double_t valeur);
	void CalculateImageDisplacements(const Int_t num_ligne);

	Double_t GetSigma(const Int_t num_ligne, const Int_t num_mes)const;
	void SetSigma(const Int_t num_ligne, const Int_t num_mes, const Double_t val);

	virtual void ReadRefImages(FILE *fp, Int_t PrintFlag) = 0;
	virtual void ReadFinalImages(FILE *fp, Int_t PrintFlag) = 0;
	virtual void CalcRefImages() 		= 0;
	virtual void CalcTheoFinalImages() 	= 0;
	virtual void CalcFinalImages() 		= 0;
	virtual void PrintFinalImages() 	= 0;
	virtual void PrintRefImages() 		= 0;
	virtual void PrintDisplacements() 	= 0;
	virtual void AddIntrinsicError(TRandom3 *rand_obj,Double_t IntResFactor) = 0;
	
	//ClassDef(Bloc,1) 

};
#endif
