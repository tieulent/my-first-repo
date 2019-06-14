/*
 *  PROXImageResult.cpp
 *  GMS
 *
 *  Created by tieulent on 22/04/09.
 *  Copyright 2009 Institut de Physique Nucléaire. All rights reserved.
 *
 */

#include "AliLog.h"
#include "PROXImageResult.h"
//ClassImp(PROXImageResult)

PROXImageResult::PROXImageResult(const char * Name) : ImageResult(Name){
	AliInfo(Form("Creating PROX Img Result "));
		GetData()->Set(5);
		AliInfo(Form("Data Add = %x",GetData()));
	
}

PROXImageResult::~PROXImageResult(){
}

void PROXImageResult::Print(){
	AliInfo(Form("Line Name: %s",GetName()));
	AliInfo(Form("Xpos     : %.2f",GetData()->At(0)));
	AliInfo(Form("Ypos     : %.2f",GetData()->At(1)));
	AliInfo(Form("Xmag     : %.2f",GetData()->At(2)));
	AliInfo(Form("Ymag     : %.2f",GetData()->At(3)));
	AliInfo(Form("Rotation : %.2f",GetData()->At(4)));
}
