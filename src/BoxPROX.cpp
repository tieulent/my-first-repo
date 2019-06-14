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
#include <TGeoBBox.h>
#include <TGeoManager.h>
#include <TMath.h>
#include <Chamber.h>
#include "Box.h"
#include "BoxPROX.h"
#include "GMSAnalyser.h"

///ClassImp(BoxPROX)


/* nb mesures */
const int BoxPROX::NB_MES = 5;
/* incertitudes intrinseques de mesures des PROX entre
les chambres 0 et 1. */
const double BoxPROX::SIGMA_X_PROX = 0.000001; // 1 mkm
const double BoxPROX::SIGMA_Y_PROX = 0.000001; // 1 mkm
//static const double SIGMA_T_PROX = 0.0000001; // 1 mkm
const double BoxPROX::SIGMA_M_PROX = 0.00005; // 5e-5
const double BoxPROX::SIGMA_A_PROX = 0.0002; // 0.2 mrad
/* incertitudes intrinseques sur les deplacements des mesures des PROX entre
les chambres 0 et 1. */
const double BoxPROX::SIGMA_X_PROX_DEP = 0.000001; // 1mkm
const double BoxPROX::SIGMA_Y_PROX_DEP = 0.000001; // 1mkm
//static const double SIGMA_T_PROX_DEP = 0.0000001; // 1mkm
const double BoxPROX::SIGMA_M_PROX_DEP = 0.00005; // 5e-5
const double BoxPROX::SIGMA_A_PROX_DEP = 0.0002; // 0.2mrad
/* distance CCD lentille dans les boites */
//const double BoxPROX::d_CCD_lens = 0.059;
const double BoxPROX::d_CCD_lens = 0.109;
/* taille de la CCD */
const double BoxPROX::size_CCD = 0.008;
const double BoxPROX::SIGMA_X = 0.0005;
const double BoxPROX::SIGMA_Y = 0.0005;
const double BoxPROX::SIGMA_Z = 0.0005;

const double BoxPROX::SIGMA_THETAX = 0.0005;
const double BoxPROX::SIGMA_THETAY = 0.0005;
const double BoxPROX::SIGMA_THETAZ = 0.0005;

const Int_t BoxPROX::MaskType = 0;
const Int_t BoxPROX::TubeType = 1;

BoxPROX::~BoxPROX()
{
}

// BoxPROX::BoxPROX(int ChID, int PlatformID, int PlaceID, int ID ) 
BoxPROX::BoxPROX(int ChID, int PlatformID, int PlaceID, char * ID , int BoxType) 
    : Box(ChID, PlatformID, PlaceID, ID){
	fType = BoxType;
	if(fType == 1) { //Tube
		fVolSize[0] = 0.;
		fVolSize[1] = 0.015;
		fVolSize[2] = 0.190;
		SetLensZ(-0.031);
	}else if(fType == 0) { //Mask
		fVolSize[0] = 0.034;
		fVolSize[1] = 0.034;
		fVolSize[2] = 0.023;
	} else{
		Fatal("BoxPROX::BoxPROX","The type of the box is invalid");
	}
}


/* positionnement aleatoire pour les PROX */
void BoxPROX::random_box(TRandom3 *rand_obj,const Box* boite_th)
  {
//     this->fCenter[0] = rand_obj->Gaus(boite_th->GetCenter(0),BoxPROX::SIGMA_X);
//     this->fCenter[1] = rand_obj->Gaus(boite_th->GetCenter(1),BoxPROX::SIGMA_Y);
//     this->fCenter[2] = rand_obj->Gaus(boite_th->GetCenter(2),BoxPROX::SIGMA_Z);
// 
//     this->fAngles[0] = rand_obj->Gaus(boite_th->GetAngle(0),BoxPROX::SIGMA_THETAZ);
//     this->fAngles[1] = rand_obj->Gaus(boite_th->GetAngle(1),BoxPROX::SIGMA_THETAY);
//     this->fAngles[2] = rand_obj->Gaus(boite_th->GetAngle(2),BoxPROX::SIGMA_THETAX);
 
 	double Factor = 2.;
    this->fCenter[0] =boite_th->GetCenter(0)+Factor*BoxPROX::SIGMA_X;
    this->fCenter[1] =boite_th->GetCenter(1)+Factor*BoxPROX::SIGMA_Y;
    this->fCenter[2] =boite_th->GetCenter(2)+Factor*BoxPROX::SIGMA_Z;

    this->fAngles[0] =boite_th->GetAngle(0)+Factor*BoxPROX::SIGMA_THETAZ;
    this->fAngles[1] =boite_th->GetAngle(1)+Factor*BoxPROX::SIGMA_THETAY;
    this->fAngles[2] =boite_th->GetAngle(2)+Factor*BoxPROX::SIGMA_THETAX;
   
	this->ComputeTransformationMatrix();
  }


/// Create TGeoVolume of the Box
void BoxPROX::CreateVolume(Chamber * Ch,int Color=0){	
	fName = (Form("PROX_%d_%d_%d",fChID,fPlatformID,fLocationID));
	
	TString RotName("Rot_");
	RotName += fName.Data();
	
	if(fType == 0) { // Mask
		fBoxVolume = gGMSGeoManager->MakeBox(fName.Data(),med1, fVolSize[0]/2., fVolSize[1]/2., fVolSize[2]/2.);
		fBoxVolume->SetLineColor(1);
	}else{ //Tube
		fBoxVolume = gGMSGeoManager->MakeTube(fName.Data(),med1, fVolSize[0]/2., fVolSize[1]/2., fVolSize[2]/2.);
		fBoxVolume->SetLineColor(1);
	}
	
	fBoxRotation = new TGeoRotation(RotName.Data(), GetMatChgtBoxPlan().ThetaX()*TMath::RadToDeg(), GetMatChgtBoxPlan().PhiX()*TMath::RadToDeg()
											, GetMatChgtBoxPlan().ThetaY()*TMath::RadToDeg(), GetMatChgtBoxPlan().PhiY()*TMath::RadToDeg()
											, GetMatChgtBoxPlan().ThetaZ()*TMath::RadToDeg(), GetMatChgtBoxPlan().PhiZ()*TMath::RadToDeg());
	fBoxCombiTrans = new TGeoCombiTrans(fCenter[0],fCenter[1],fCenter[2], fBoxRotation);
	
    ((Ch->GetPlatformByID(fPlatformID))->fVolGlobal)->AddNode(fBoxVolume, 1,  fBoxCombiTrans);
}
/// Create TGeoVolume of the Box
void BoxPROX::CreateVolume(Wall * Ch, int Color){	
}

/// Load the calibration parameters of the Box
void BoxPROX::LoadCalibrationParameters(){	
}

