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
#ifndef BLOCLMSBCAM_H
#define BLOCLMSBCAM_H

#include "Bloc.h"


/**
@author Philippe Pillot
*/

//!  Class describing a bloc of BCAM optical line which are part of the LMS (Longitudinal Monitoring System). 

class BlocLMSBCAM : public Bloc
{
public:
	BlocLMSBCAM(const int nLines);
    BlocLMSBCAM(const int nLines,
		  Chamber *const ch1,Box *const *const BCAMch1,
		  Chamber *const ch2,Box *const *const BCAMch2);

    ~BlocLMSBCAM();

	void Display() const;
	virtual void CalcRefImages()	;
	virtual void CalcTheoFinalImages();
	virtual void CalcFinalImages()  ;
	virtual void PrintFinalImages()  ;
	virtual void PrintDisplacements()  ;
	virtual void PrintRefImages()  ;
	void AddIntrinsicError(TRandom3 *rand_obj,double IntResFactor);
virtual void CalcNewAngle();
	virtual void ReadRefImages(FILE *fp, int PrintFlag) ;
	virtual void ReadFinalImages(FILE *fp, int PrintFlag) ;	
private:
	//Box ***fBCAM;
	//Chamber ** fCh;
	
//	ClassDef(BlocLMSBCAM,1) 

};

#endif
