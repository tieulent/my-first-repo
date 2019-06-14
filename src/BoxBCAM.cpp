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
#include "AliLog.h"

#include "Chamber.h"
#include "BoxBCAM.h"
#include "GMSAnalyser.h"

//ClassImp(BoxBCAM)


/* nb mesures */
const int BoxBCAM::NB_MES = 4;
const Double_t BoxBCAM::SIGMA_X_BCAM = 0.0000005; // 0.5 mkm
const Double_t BoxBCAM::SIGMA_Y_BCAM = 0.0000005; // 0.5 mkm
const Double_t BoxBCAM::SIGMA_X_BCAM_DEP = 0.000001414; // 1mkm*sqrt(2)
const Double_t BoxBCAM::SIGMA_Y_BCAM_DEP = 0.000001414; // 1mkm*sqrt(2)
const Double_t BoxBCAM::SIGMA_X = 0.0005;
const Double_t BoxBCAM::SIGMA_Y = 0.0005;
const Double_t BoxBCAM::SIGMA_Z = 0.0005;

const Double_t BoxBCAM::SIGMA_THETAX = 0.0005;
const Double_t BoxBCAM::SIGMA_THETAY = 0.0005;
const Double_t BoxBCAM::SIGMA_THETAZ = 0.0005;

//-----------------------------------------------------------------------------

BoxBCAM::~BoxBCAM()
{
	printf("debut Destruc de BoxBCAM\n");
	printf("fin Destruc de BoxBCAM\n");
}
//-----------------------------------------------------------------------------

//BoxBCAM::BoxBCAM(int ChID, int PlatformID, int PlaceID, int BCAMID ) 
BoxBCAM::BoxBCAM(int ChID, int PlatformID, int PlaceID, const char * BCAMID ) 
    : Box(ChID, PlatformID, PlaceID, BCAMID){
	
	AliInfo(Form("Creating BCAM_%d_%d_%d",ChID,PlatformID,PlaceID));
	fVolSize[0] = 0.055;
	fVolSize[1] = 0.030;
	fVolSize[2] = 0.091;	
	LoadCalibrationParameters();
	}
//-----------------------------------------------------------------------------

void BoxBCAM::CreateVolume(Wall * Ch, int Color){
}

//-----------------------------------------------------------------------------

/// Create TGeoVolume of the Box
void BoxBCAM::CreateVolume(Chamber * Ch, int Color){
	
	fName = (Form("BCAM_%d_%d_%d",fChID,fPlatformID,fLocationID));
	
	TString RotName("Rot_");
	RotName += fName.Data();

	fBoxVolume = gGMSGeoManager->MakeBox(fName.Data(),med1, fVolSize[0]/2., fVolSize[1]/2., fVolSize[2]/2.);
	fBoxVolume->SetLineColor(Color);
	
	fOpticalAxis = gGMSGeoManager->MakeTube(fName.Data(),med1, 0., 0.005, 1.);
	fOpticalAxis->SetLineColor(Color);
	if(gGMSAnalyser->GetPrintLevel() > 1){
		Info("BoxBCAM::CreateVolume","--------------------- TRotation --------------------------");
		Info("BoxBCAM::CreateVolume","%.4e %.4e %.4e ",GetMatChgtBoxPlan().XX(),GetMatChgtBoxPlan().XY(),GetMatChgtBoxPlan().XZ());
		Info("BoxBCAM::CreateVolume","%.4e %.4e %.4e ",GetMatChgtBoxPlan().YX(),GetMatChgtBoxPlan().YY(),GetMatChgtBoxPlan().YZ());
		Info("BoxBCAM::CreateVolume","%.4e %.4e %.4e ",GetMatChgtBoxPlan().ZX(),GetMatChgtBoxPlan().ZY(),GetMatChgtBoxPlan().ZZ());
		Info("BoxBCAM::CreateVolume","-------------------------------------------------------------");
	}
	
	fBoxRotation = new TGeoRotation(RotName.Data(), GetMatChgtBoxPlan().ThetaX()*TMath::RadToDeg(), GetMatChgtBoxPlan().PhiX()*TMath::RadToDeg()
											, GetMatChgtBoxPlan().ThetaY()*TMath::RadToDeg(), GetMatChgtBoxPlan().PhiY()*TMath::RadToDeg()
											, GetMatChgtBoxPlan().ThetaZ()*TMath::RadToDeg(), GetMatChgtBoxPlan().PhiZ()*TMath::RadToDeg());
	
	// In the Database file the position is the position of the front ball. Here we want the position of the center of the box 
//	double dZ = fVolSize[1]/2.+fCenter[2]+0.002;
	double dZ = fCenter[2];
	//if(fCenter[2]<0) dZ *= -1.;
	if(Color == 1){ // Black BCAM
		fBoxCombiTrans = new TGeoCombiTrans(fCenter[0]-0.00122,fCenter[1]+0.03995,dZ, fBoxRotation);
	}else{ 			//Blue BCAM
		fBoxCombiTrans = new TGeoCombiTrans(fCenter[0]+0.00122,fCenter[1]+0.03995,dZ, fBoxRotation);
	}	
	// printf("MakeCombiTrans: fBoxRotation %x\n", fBoxRotation);
	// fBoxCombiTrans = new TGeoCombiTrans(fCenter[0],fCenter[1],dZ, fBoxRotation);
	// fBoxCombiTrans->Print();
	
	//printf("Address fVolGlobal Platform %d : %x\n",fPlatformID,(Ch->GetPlatform(fPlatformID))->fVolGlobal);
	// printf("$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$\n");
	// printf("$$$$$$$$$$$ Plat_Glob_%d_%d\n",fChID,fPlatformID);
	// printf("$$$$$$$$$$$ Add Ch %X:  Add Plat %X\n",Ch,Ch->GetPlatformByID(fPlatformID));
	// printf("$$$$$$$$$$$ Add Volume %X \n",(Ch->GetPlatformByID(fPlatformID))->fVolume);
	// printf("$$$$$$$$$$$ Add VolGlobal %X \n",(Ch->GetPlatformByID(fPlatformID))->fVolGlobal);
	// printf("$$$$$$$$$$$ nb of node %d \n",(Ch->GetPlatformByID(fPlatformID))->fVolGlobal->GetNtotal());
    ((Ch->GetPlatformByID(fPlatformID))->fVolGlobal)->AddNode(fBoxVolume, 1,  fBoxCombiTrans);
    ((Ch->GetPlatformByID(fPlatformID))->fVolGlobal)->AddNode(fOpticalAxis, 1,  fBoxCombiTrans);
}
	
//-----------------------------------------------------------------------------
	
/// Place Randomly a BCAM box
void BoxBCAM::random_box(TRandom3 *rand_obj,const Box* boite_th)
  {
//     this->fCenter[0] = rand_obj->Gaus(boite_th->GetCenter(0),BoxBCAM::SIGMA_X);
//     this->fCenter[1] = rand_obj->Gaus(boite_th->GetCenter(1),BoxBCAM::SIGMA_Y);
//     this->fCenter[2] = rand_obj->Gaus(boite_th->GetCenter(2),BoxBCAM::SIGMA_Z);
//     
//     this->fAngles[0] = rand_obj->Gaus(boite_th->GetAngle(0),BoxBCAM::SIGMA_THETAZ);
//     this->fAngles[1] = rand_obj->Gaus(boite_th->GetAngle(1),BoxBCAM::SIGMA_THETAY);
//     this->fAngles[2] = rand_obj->Gaus(boite_th->GetAngle(2),BoxBCAM::SIGMA_THETAX);
	
 	double Factor = 2.;
    this->fCenter[0] = boite_th->GetCenter(0)+Factor*BoxBCAM::SIGMA_X;
    this->fCenter[1] = boite_th->GetCenter(1)+Factor*BoxBCAM::SIGMA_Y;
    this->fCenter[2] = boite_th->GetCenter(2)+Factor*BoxBCAM::SIGMA_Z;
    
    this->fAngles[0] = boite_th->GetAngle(0)+Factor*BoxBCAM::SIGMA_THETAZ;
    this->fAngles[1] = boite_th->GetAngle(1)+Factor*BoxBCAM::SIGMA_THETAY;
    this->fAngles[2] = boite_th->GetAngle(2)+Factor*BoxBCAM::SIGMA_THETAX;
	
    this->ComputeTransformationMatrix();
  }

//-----------------------------------------------------------------------------
/// Load the calibration parameters of the Box
void BoxBCAM::LoadCalibrationParameters(){	
	FILE *fp ;
	TString DBFileName(gGMSAnalyser->GetGMSAnalyserRootDir());
	DBFileName += "/data/BCAM_new_orientation.db";
	if(!(fp= fopen(DBFileName.Data(),"r"))) 
		Fatal("BoxBCAM::LoadCalibrationParameters","Enable to open the BCAM database file %s\n",DBFileName.Data());

	char cdump[15];
	Double_t fdump;
//	for(int i=0;i<13;i++) fscanf(fp,"%s ",&cdump);
// 	int ID;
// 	fscanf(fp,"%d",&ID);
//	while(ID!=fID){
	char ID[6];
	fscanf(fp,"%s",&ID);
	while(strcmp(ID,fID.Data()) !=0){
		for(int i=0;i<13;i++) 
			if(EOF==fscanf(fp,"%lf",&fdump)){
				fclose(fp);	
				Error("BoxBCAM::LoadCalibrationParameters","Calibration factors for BCAM %s unfound in BCAM database file %s\n",fID.Data(),DBFileName.Data());
			}
// 		fscanf(fp,"%d",&ID);
		fscanf(fp,"%s",&ID);
	}
	fscanf(fp,"%lf %lf %lf",&fPivot[0],&fPivot[1],&fPivot[2]);
	fscanf(fp,"%lf %lf %d %lf %lf",&fTx,&fTy,&fType,&fFocalLength,&fPsi);
	fscanf(fp,"%lf %lf %lf %lf %lf",&fLeftLED[0],&fLeftLED[1],&fRightLED[0],&fRightLED[1],&fLeftLED[2]);

	// if(fType == -1) // Blue BCAM
	// 	fscanf(fp,"%lf %lf %lf %lf %lf",&fLeftLED[0],&fLeftLED[1],&fRightLED[0],&fRightLED[1],&fLeftLED[2]);
	// else			// Black BCAM
	// 	fscanf(fp,"%lf %lf %lf %lf %lf",&fRightLED[0],&fRightLED[1],&fLeftLED[0],&fLeftLED[1],&fLeftLED[2]);
	// fRightLED[2] = fLeftLED[2];
	// Double_t NormFact;
	// if(fType == -1)	{
	// 	NormFact = -1.e-3;// Blue BCAM
	// 	fPsi *= 1.e-3;
	// }else{
	// 	NormFact =  1.e-3;// Black BCAM
	// 	fPsi = fPsi*1.e-3 -TMath::Pi();
	// }
	// //Psi *= -1.;
	// for(int i=0;i<2;i++) { // Put the blue BCAM parameters in the same coordinate system of the black one
	// 	fPivot[i] *=NormFact; 
	// 	fLeftLED[i] *= NormFact;
	// 	fRightLED[i] *= NormFact;
	// }
	// fPivot[2] *= 1.e-3 ;
	// fLeftLED[2] *= 1.e-3;
	// fRightLED[2] *= 1.e-3 ;
	// fTx *= NormFact;
	// fTy *= NormFact;
	// fFocalLength *= 1.e-3;
	
	fRightLED[2] = fLeftLED[2];
	Double_t NormFact = 1.e-3;
	fPsi *= NormFact;
	for(int i=0;i<3;i++) { 
		fPivot[i] *=NormFact; 
		fLeftLED[i] *= NormFact;
		fRightLED[i] *= NormFact;
	}
	fTx *= NormFact;
	fTy *= NormFact;
	fFocalLength *= NormFact;
	if(gGMSAnalyser->GetPrintLevel() > 1) 	Info("BoxBCAM::LoadCalibrationParameters","ID %s : Px=%lf Py=%lf Pz=%lf Psi=%lf\n", fID.Data(), fPivot[0], fPivot[1], fPivot[2],fPsi);

	fclose(fp);
}
