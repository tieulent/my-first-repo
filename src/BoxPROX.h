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
#ifndef BOITEPROX_H
#define BOITEPROX_H
#include <TGeoVolume.h>
#include <TGeoMatrix.h>

#include "Box.h"
//!  Class describing Box of Type "PROX". 
/*!
*/
class BoxPROX : public Box{

public:

/* nb mesures */
static const int NB_MES ;
/* incertitudes intrinseques de mesures des PROX entre
les chambres 0 et 1. */
static const double SIGMA_X_PROX ; // 1 mkm
static const double SIGMA_Y_PROX ; // 1 mkm
//static const double SIGMA_T_PROX ; // 1 mkm
static const double SIGMA_M_PROX ; // 5e-5
static const double SIGMA_A_PROX ; // 0.2 mrad
/* incertitudes intrinseques sur les deplacements des mesures des PROX entre
les chambres 0 et 1. */
static const double SIGMA_X_PROX_DEP; // 1mkm
static const double SIGMA_Y_PROX_DEP; // 1mkm
//static const double SIGMA_T_PROX_DEP ; // 1mkm
static const double SIGMA_M_PROX_DEP; // 5e-5
static const double SIGMA_A_PROX_DEP; // 0.2mrad
/* distance CCD lentille dans les boites */
static const double d_CCD_lens ;
/* taille de la CCD */
static const double size_CCD;
static const Int_t MaskType;
static const Int_t TubeType;

// BoxPROX(int ChID, int PlatformID, int PlaceID, int BoxID );
BoxPROX(int ChID, int PlatformID, int PlaceID, char * BoxID, int BoxType );
~BoxPROX();
void SetLensZ(Float_t z){fLensZ = z;};
Float_t GetLensZ() const {return(fLensZ);};

virtual void random_box(TRandom3 *rand_obj,const Box* boite_th);
virtual void CreateVolume(Chamber * Ch,int) ;
virtual void CreateVolume(Wall * Ch, int) ;
virtual void LoadCalibrationParameters();

private:

/* position z de la lentille par rapport au point de la boite qui est sur la */
/* chambre, dans le referentiel de la boite */
	Float_t fLensZ; ///< Z position of the lens
	Int_t fType; // 0 if this box is a Mask and 1 if it is a tube

/* incertitudes de positionnement de la boite, utile uniquement pour la methode
random_box */
//static const double SIGMA_CENTRE = 0.00005;
static const double SIGMA_X ;
static const double SIGMA_Y ;
static const double SIGMA_Z ;

static const double SIGMA_THETAX;
static const double SIGMA_THETAY;
static const double SIGMA_THETAZ;

	//ClassDef(BoxPROX,1) 

};
#endif
