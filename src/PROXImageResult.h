/*
 *  PROXImageResult.h
 *  GMS
 *
 *  Created by tieulent on 22/04/09.
 *  Copyright 2009 Institut de Physique Nucléaire. All rights reserved.
 *
 */

#ifndef PROXIMAGERESULT_H
#define PROXIMAGERESULT_H

/// \ingroup "GMSAnalyser"
/// \class PROXImageResult
/// \brief Class which describes the result from a Proximity Image taken using the LWDAQ system
///
//  Author Raphael Tieulent, IPN-Lyon

#include <TArrayF.h>
#include "ImageResult.h"

class PROXImageResult : public ImageResult {
	
public:
	PROXImageResult(const char * Name);
	~PROXImageResult();
	
	Float_t GetXpos(){return GetData()->At(0);;};
	Float_t GetYpos(){return GetData()->At(1);;};
	Float_t GetXmag(){return GetData()->At(2);;};
	Float_t GetYmag(){return GetData()->At(3);;};
	Float_t GetRotation(){return GetData()->At(4);;};

	void SetXpos(Float_t val){ GetData()->SetAt(val,0);};
	void SetYpos(Float_t val){ GetData()->SetAt(val,1);};
	void SetXmag(Float_t val){ GetData()->SetAt(val,2);};
	void SetYmag(Float_t val){ GetData()->SetAt(val,3);};
	void SetRotation(Float_t val){ GetData()->SetAt(val,4);};
	
	virtual void Print();

 	//ClassDef(PROXImageResult,1) 
};

#endif
