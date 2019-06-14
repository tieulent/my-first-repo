/*
 *  BCAMImageResult.cpp
 *  GMS
 *
 *  Created by tieulent on 22/04/09.
 *  Copyright 2009 Institut de Physique Nucléaire. All rights reserved.
 *
 */
#include <TArrayF.h>

#include "AliLog.h"
#include "BCAMImageResult.h"

//ClassImp(BCAMImageResult)

BCAMImageResult::BCAMImageResult(const char * Name) : ImageResult(Name){
	AliInfo(Form("Creating BCAM Img Result "));
//	GetData()->Set(4);
//	AliInfo(Form("Data Add = %x",GetData()));
}

BCAMImageResult::~BCAMImageResult(){
}

void BCAMImageResult::Print(){
	AliInfo(Form("-----------"));
	AliInfo(Form("Line Name: %s",GetName()));
	AliInfo(Form("Xleft  : %.3f",GetData()->At(0)));
	AliInfo(Form("Yleft  : %.3f",GetData()->At(1)));
	AliInfo(Form("Xright : %.3f",GetData()->At(2)));
	AliInfo(Form("Yright : %.3f",GetData()->At(3)));
}

void BCAMImageResult::SetLED(Int_t iLed, Float_t x, Float_t y){
	// iLed = 0 => Left
	// iLed = 1 => Right
	GetData()->SetAt(x,2*iLed); 
	GetData()->SetAt(y,2*iLed + 1);
}
