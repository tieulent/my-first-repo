/*
 *  GMSImage.h
 *  GMSdev
 *
 *  Created by tieulent on 27/04/09.
 *  Copyright 2009 Institut de Physique Nucléaire. All rights reserved.
 *
 */
#ifndef GMSIMAGE_H
#define GMSIMAGE_H

#include <TNamed.h>

class TObjString;
class Box;
class ImageResult;

class GMSImage : public TNamed {

public:
	enum GMSImageType { kBCAM, kPROX };

	GMSImage(TObjString * Name, GMSImageType type);
	GMSImage(TObjString * Name, GMSImageType type, Box * objBox, Box * ccdBox);
	GMSImage(TObjString * Name, GMSImageType type, Box * objBox, Box * ccdBox, ImageResult * res, ImageResult * ref);
	~GMSImage();
	
	// Setters
	void SetObjectBox(Box * box){fObjectBox = box;};
	void SetCCDBox(Box * box){fCCDBox = box;};
	void SetResult(ImageResult * result){fResult = result;};
	void SetReference(ImageResult * ref){fReference = ref;};
	void SetSigma(ImageResult * sig){fSigma = sig;};
	void SetDisplacement(ImageResult * disp){fDisplacement = disp;};
	
	//Getters
	Box * GetObjectBox(){return fObjectBox;};
	Box * GetCCDBox(){return fCCDBox;};
	ImageResult * GetResult(){return fResult;};
	ImageResult * GetReference(){return fReference;};
	ImageResult * GetSigma(){return fSigma;};
	ImageResult * GetDisplacement(){return fDisplacement;};
	
	void CalculateImageDisplacement();
	
	
private:
	Box * fObjectBox;
	Box * fCCDBox;
	ImageResult * fResult;
	ImageResult * fReference;
	ImageResult * fDisplacement;
	ImageResult * fSigma;
	GMSImageType fType;
	
	//ClassDef(GMSImage,1) 

	
};

#endif
