/*
 *  GMSImage.cpp
 *  GMSdev
 *
 *  Created by tieulent on 27/04/09.
 *  Copyright 2009 Institut de Physique Nucléaire. All rights reserved.
 *
 */

#include <TNamed.h>
#include <TString.h>
#include <TObjString.h>
#include "AliLog.h"
#include "BCAMImageResult.h"
#include "PROXImageResult.h"

#include "GMSImage.h"

//ClassImp(GMSImage)

//------------------------------------------------------------------------------------------------------------------------------------------
GMSImage::GMSImage(TObjString * Name, GMSImageType type) : TNamed(Name->String().Data(),Name->String().Data()), 
	fObjectBox(NULL), 
	fCCDBox(NULL), 
	fResult(NULL), 
	fReference(NULL), 
	fType(type)
{
	TString sName = Name->String();
	if(type = kBCAM) {
		fDisplacement = new BCAMImageResult(sName.Data());
		fSigma = new BCAMImageResult(sName.Data());
	} else if(type = kPROX) {
		fDisplacement = new PROXImageResult(sName.Data());
		fSigma = new PROXImageResult(sName.Data());
	} else 
		AliError(Form("Type of image %s not valid", sName.Data()));
}

//------------------------------------------------------------------------------------------------------------------------------------------
GMSImage::GMSImage(TObjString * Name, GMSImageType type, Box * objBox, Box * ccdBox) : TNamed(Name->String().Data(),Name->String().Data()),
	fObjectBox(objBox), 
	fCCDBox(ccdBox), 
	fResult(NULL), 
	fReference(NULL), 
	fType(type)
{	
	TString sName = Name->String();
	
	if(type = kBCAM) {
		fDisplacement = new BCAMImageResult(sName.Data());
		fSigma = new BCAMImageResult(sName.Data());
	} else if(type = kPROX) {
		fDisplacement = new PROXImageResult(sName.Data());
		fSigma = new PROXImageResult(sName.Data());
	} else 
		AliError(Form("Type of image %s not valid", sName.Data()));
}

//------------------------------------------------------------------------------------------------------------------------------------------
GMSImage::GMSImage(TObjString * Name, GMSImageType type, Box * objBox, Box * ccdBox, ImageResult * res, ImageResult * ref) : TNamed(Name->String().Data(),Name->String().Data()), 
	fObjectBox(objBox), 
	fCCDBox(ccdBox), 
	fResult(res), 
	fReference(ref), 
	fType(type)
{	
	TString sName = Name->String();
	AliInfo(Form("image %s to be created with type %d", sName.Data(),type));
	if(type = kBCAM) {
		fDisplacement = new BCAMImageResult(sName.Data());
		fSigma = new BCAMImageResult(sName.Data());
	} else if(type = kPROX) {
		fDisplacement = new PROXImageResult(sName.Data());
		fSigma = new PROXImageResult(sName.Data());
	} else 
		AliError(Form("Type of image %s not valid", sName.Data()));
	
	AliInfo(Form("Image add : %x , Sigma add %x  Data add %x ", this, fSigma,fSigma->GetData()));
}

//------------------------------------------------------------------------------------------------------------------------------------------
GMSImage::~GMSImage(){
	delete fSigma;
	delete fDisplacement;
}

//------------------------------------------------------------------------------------------------------------------------------------------
/// Calculate the displacement of the image between the Reference measurement and the last measurement
/// fDisplacement = fResult - fReference
void GMSImage::CalculateImageDisplacement(){
	for (int i; i<GetDisplacement()->GetData()->GetSize(); i++) 
		GetDisplacement()->GetData()->SetAt(GetResult()->GetData()->GetAt(i) - GetReference()->GetData()->GetAt(i),i);

}

