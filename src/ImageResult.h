/*
 *  ImageResult.h
 *  GMS
 *
 *  Created by tieulent on 22/04/09.
 *  Copyright 2009 Institut de Physique Nucléaire. All rights reserved.
 *
 */

#ifndef IMAGERESULT_H
#define IMAGERESULT_H

#include <TNamed.h>

class TArrayF;

/// \ingroup "GMSAnalyser"
/// \class ImageResult
/// \brief Base class which describes the result from an Image taken using the LWDAQ system
///
//  Author Raphael Tieulent, IPN-Lyon


class ImageResult : public TNamed {
public:
	ImageResult(const char * Name);
	~ImageResult();
	
	virtual void Print() {};

	// Getters
	Float_t GetExposureTime(){return fExposureTime;};
	TArrayF * GetData(){return fData;};
	
	// Setters
	void SetData(TArrayF data);
	void SetExposureTime(Float_t val){ fExposureTime = val;};

	
private:
	Float_t fExposureTime;
	TArrayF * fData;
	
//	ClassDef(ImageResult,1) 

	
};

#endif
