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
#ifndef BOXBCAM_H
#define BOXBCAM_H

class Chamber;
#include <TGeoVolume.h>
#include <TGeoMatrix.h>


#include "Box.h"
/**
@author Philippe Pillot
*/

//!  Class describing BCAM Box. 
/*!
*/
class BoxBCAM : public Box{

public:

/* nb mesures */
static const int NB_MES;
/* incertitudes intrinseques de mesures des BCAM entre
les chambres 0 et 1. */
static const Double_t SIGMA_X_BCAM; 
static const Double_t SIGMA_Y_BCAM; 
/* incertitudes intrinseques sur les deplacements des mesures des BCAM entre
les chambres 0 et 1. */
static const Double_t SIGMA_X_BCAM_DEP; 
static const Double_t SIGMA_Y_BCAM_DEP; 

//BoxBCAM(int ChID, int PlatformID, int PlaceID, int BCAMID );
BoxBCAM(int ChID, int PlatformID, int PlaceID, const char * BCAMID );
~BoxBCAM();
virtual void random_box(TRandom3 *rand_obj,const Box* boite_th);
virtual void CreateVolume(Chamber * Ch, int Color) ;
virtual void CreateVolume(Wall * Ch, int Color) ;
virtual void LoadCalibrationParameters();
inline TVector3 GetLeftLED()const {return fLeftLED;};
inline TVector3 GetRightLED()const {return fRightLED;};
inline TVector3 GetPivot()const {return fPivot;};
inline Double_t GetFocalLength()const {return fFocalLength;};
inline Double_t GetPsi()const {return fPsi;};
inline Double_t GetTx()const {return fTx;};
inline Double_t GetTy()const {return fTy;};
inline Int_t GetType()const {return fType;};


private:

/* incertitudes de positionnement de la boite, utile uniquement pour la methode
random_box */
//static const Double_t SIGMA_CENTRE = 0.;
static const Double_t SIGMA_X;
static const Double_t SIGMA_Y;
static const Double_t SIGMA_Z;

static const Double_t SIGMA_THETAX;
static const Double_t SIGMA_THETAY;
static const Double_t SIGMA_THETAZ;


 TVector3 fPivot;/*! Position of the lens in the frame of the BCAM */
 Double_t fTx,fTy;/*! Orientation of the axis linking the pivot (lens) and the center of the CCD */
 Double_t fFocalLength;/*! Distance CCD - Lens */
 Double_t fPsi;/*! Orientation of the X and Y axis of the CCD */
 TVector3 fLeftLED,fRightLED;/*! Position of the LEDs in the frame of the BCAM */
 Int_t fType; /*! -1 = Blue BCAM, +1 = Blacl BCAM */

// 	ClassDef(BoxBCAM,1) 

};


#endif
