/*
 *  ImageResult.cpp
 *  GMS
 *
 *  Created by tieulent on 22/04/09.
 *  Copyright 2009 Institut de Physique Nucléaire. All rights reserved.
 *
 */
#include <TArrayF.h>
#include "AliLog.h"
#include "ImageResult.h"

//ClassImp(ImageResult)

ImageResult::ImageResult(const char * Name) : TNamed(Name,Name){
	
	fData = new TArrayF();
	AliInfo(Form("Creating  Img Result "));
	AliInfo(Form("Data Add = %x",fData));
}

ImageResult::~ImageResult(){
	delete fData;
}

void ImageResult::SetData(TArrayF data){
	for(int i=0;i<fData->GetSize();i++) fData->SetAt(data[i],i);
}