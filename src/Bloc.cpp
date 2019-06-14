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
#include <AliLog.h>
#include <TObjArray.h>
#include <TArrayF.h>
#include "Chamber.h"
#include "GMSImage.h"
#include "ImageResult.h"
#include "Bloc.h"

//ClassImp(Bloc)

//______________________________________________________________________________

/** \brief Constructor
	
		longer description
	
	\author Raphael Tieulent 
	\date 2006-10-02
	\param  nb_lignes: Number of optical which is part of this Bloc
	\param  nb_mes_par_ligne: Number of measurements per optical lines
	\sa
**/
Bloc::Bloc(const Int_t nb_lignes,const Int_t nb_mes_par_ligne) : TObject() {
    Int_t i,j;
    
    this->fNLines = nb_lignes;
    this->fNMeasuresPerLine  = nb_mes_par_ligne;

	fImages = new TObjArray();
	fImages->SetOwner(1);
    
  }


//______________________________________________________________________________

/** \brief Class destructor
	\author Raphael Tieulent 
	\date 2006-10-02
	\sa
**/
Bloc::~Bloc(){
	delete fImages;
}

//______________________________________________________________________________
TString Bloc::GetImageName(const Int_t iLine){
	if(iLine>fImages->GetEntriesFast()) 
		AliError(Form("Looking for line number %d, but Image array contains only %d images.",iLine,fImages->GetEntriesFast()));
	
	TString Name(GetImage(iLine)->GetName());
	return Name;
}

//______________________________________________________________________________

/// Set the value of the uncertancie for the measurement "iMeasure" of the line "iLine" to "val"
void Bloc::SetSigma(const Int_t iLine, const Int_t iMeasure, const Double_t val){
	if(iLine>fImages->GetEntriesFast()) 
		  AliFatal(Form("Looking for line number %d, but Image array contains only %d images.",iLine,fImages->GetEntriesFast()));
	AliInfo(Form("GetImage(iLine) %x",GetImage(iLine)));
	AliInfo(Form("(GetImage(iLine)->GetSigma()) %x",(GetImage(iLine)->GetSigma())));
	AliInfo(Form("(GetImage(iLine)->GetSigma())->GetData() %x",(GetImage(iLine)->GetSigma())->GetData()));
	(GetImage(iLine)->GetSigma())->GetData()->SetAt(val,iMeasure);
}


//______________________________________________________________________________

/// Return the uncertancy for the measurement "iMeasure" of the line "iLine"
Double_t Bloc::GetSigma(const Int_t iLine, const Int_t iMeasure)const{
	
	if(iLine>fImages->GetEntriesFast()) 
		AliFatal(Form("Looking for line number %d, but Image array contains only %d images.",iLine,fImages->GetEntriesFast()));

	return((GetImage(iLine)->GetSigma())->GetData()->GetAt(iMeasure));
}


//______________________________________________________________________________

/// Returns the reference measurement "iMeasure" of the line "iLine"
Double_t Bloc::GetRefImage(const Int_t iLine, const Int_t iMeasure)const{
	if(iLine>fImages->GetEntriesFast()) 
		AliFatal(Form("Looking for line number %d, but Image array contains only %d images.",iLine,fImages->GetEntriesFast()));
	
	return((GetImage(iLine)->GetReference())->GetData()->GetAt(iMeasure));
}
//______________________________________________________________________________

/// Returns the final measurement "iMeasure" of the line "iLine"
Double_t Bloc::GetFinalImage(const Int_t iLine, const Int_t iMeasure)const {
	if(iLine>fImages->GetEntriesFast()) 
		AliFatal(Form("Looking for line number %d, but Image array contains only %d images.",iLine,fImages->GetEntriesFast()));
	
	return((GetImage(iLine)->GetResult())->GetData()->GetAt(iMeasure));
}


//______________________________________________________________________________

/// Returns the displacement of the measurement "iMeasure" of the line "iLine"
Double_t Bloc::GetImageDisplacement(const Int_t iLine, const Int_t iMeasure)const {
	if(iLine>fImages->GetEntriesFast()) 
		AliFatal(Form("Looking for line number %d, but Image array contains only %d images.",iLine,fImages->GetEntriesFast()));
    	
	return((GetImage(iLine)->GetDisplacement())->GetData()->GetAt(iMeasure));
}
  
  
//______________________________________________________________________________

/// Returns the displacement of the measurement "iMeasure" of the line "iLine"
void Bloc::SetImageDisplacement(const Int_t iLine, const Int_t iMeasure, Double_t val) {
	if(iLine>fImages->GetEntriesFast()) 
		AliFatal(Form("Looking for line number %d, but Image array contains only %d images.",iLine,fImages->GetEntriesFast()));
		
	(GetImage(iLine)->GetDisplacement())->GetData()->SetAt(val,iMeasure);
}

//______________________________________________________________________________

/// Set the value of the reference measurement "iMeasure" of the line "iLine" to "val"
void Bloc::SetRefImage(const Int_t iLine, const Int_t iMeasure, Double_t val){
	if(iLine>fImages->GetEntriesFast()) 
		AliFatal(Form("Looking for line number %d, but Image array contains only %d images.",iLine,fImages->GetEntriesFast()));
		
	(GetImage(iLine)->GetReference())->GetData()->SetAt(val,iMeasure);
}
//______________________________________________________________________________

/// Set the value of the reference measurements of the line "iLine"
void Bloc::SetRefImages(const Int_t iLine, Double_t *val) {
	if(iLine>fImages->GetEntriesFast()) 
		AliFatal(Form("Looking for line number %d, but Image array contains only %d images.",iLine,fImages->GetEntriesFast()));
		
    for(int iMeasure = 0; iMeasure < fNMeasuresPerLine; iMeasure++)
		(GetImage(iLine)->GetReference())->GetData()->SetAt(val[iMeasure],iMeasure);
}

//______________________________________________________________________________

/// Set the value of the final measurement "iMeasure" of the line "iLine" to "val"
void Bloc::SetFinalImage(const Int_t iLine, const Int_t iMeasure, Double_t val) {
	if(iLine>fImages->GetEntriesFast()) 
		AliFatal(Form("Looking for line number %d, but Image array contains only %d images.",iLine,fImages->GetEntriesFast()));
		
	(GetImage(iLine)->GetResult())->GetData()->SetAt(val,iMeasure);
}
//______________________________________________________________________________

/// Set the value of the final measurements of the line "iLine" 
void Bloc::SetFinalImages(const Int_t iLine, Double_t *val) {
	if(iLine>fImages->GetEntriesFast()) 
		AliFatal(Form("Looking for line number %d, but Image array contains only %d images.",iLine,fImages->GetEntriesFast()));
		
    for(int iMeasure = 0; iMeasure < fNMeasuresPerLine; iMeasure++)
		(GetImage(iLine)->GetResult())->GetData()->SetAt(val[iMeasure],iMeasure);
}

//______________________________________________________________________________
void Bloc::CalculateImageDisplacements(const Int_t iLine){
/// Set la valeur de fFinalImage[iLine][iMeasure] 
	if(iLine>fImages->GetEntriesFast()) 
		AliFatal(Form("Looking for line number %d, but Image array contains only %d images.",iLine,fImages->GetEntriesFast()));
	
	GetImage(iLine)->CalculateImageDisplacement();
}
