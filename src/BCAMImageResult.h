/*
 *  BCAMImageResult.h
 *  GMS
 *
 *  Created by tieulent on 22/04/09.
 *  Copyright 2009 Institut de Physique Nucléaire. All rights reserved.
 *
 */
#ifndef BCAMIMAGERESULT_H
#define BCAMIMAGERESULT_H

#include <TArrayF.h>
#include "ImageResult.h"
/// \ingroup "GMSAnalyser"
/// \class BCAMImageResult
/// \brief Class which describes the result from a BCAM Image taken using the LWDAQ system
///
//  Author Raphael Tieulent, IPN-Lyon

class BCAMImageResult : public ImageResult {
	
public:
	BCAMImageResult(const char * Name);
	~BCAMImageResult();
	
	Float_t GetXleft(){return GetData()->At(0);};
	Float_t GetYleft(){return GetData()->At(1);};
	Float_t GetXright(){return GetData()->At(2);};
	Float_t GetYright(){return GetData()->At(3);};
	
	void SetXleft(Float_t val){ GetData()->SetAt(val,0);};
	void SetYleft(Float_t val){ GetData()->SetAt(val,1);};
	void SetXright(Float_t val){ GetData()->SetAt(val,2);};
	void SetYright(Float_t val){ GetData()->SetAt(val,3);};
	
	void SetLeftLED(Float_t xl, Float_t yl) { GetData()->SetAt(xl,0); GetData()->SetAt(yl,1); };
	void SetRightLED(Float_t xl, Float_t yl) { GetData()->SetAt(xl,2); GetData()->SetAt(yl,3); };
	void SetLED(Int_t iLed, Float_t x, Float_t y);
		
	virtual void Print();
 	//ClassDef(BCAMImageResult,1) 

};

#endif

