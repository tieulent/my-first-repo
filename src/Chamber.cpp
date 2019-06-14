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
//-----------------------------------------------------------------------------
/// \class Chamber
/// A Chamber is special type of plan. 
/// It contains "fNPlatforms" Platforms which are themselves Planes objects.
///
/// \author Raphael Tieulent, IPN-Lyon
//-----------------------------------------------------------------------------


#include <TRandom3.h>
#include <TMath.h>
#include <TGeoManager.h>
#include <TClonesArray.h>
#include <TTree.h>
#include <AliLog.h>
#include "Chamber.h"
#include "GMSAnalyser.h"

 //ClassImp(Chamber)

Chamber::Chamber() : Plane()
  {
  }

Chamber::Chamber(Int_t ID, const Double_t pos_th[3], const Double_t ang_th[3], const Double_t Size[3], const Int_t * platforms, const Int_t NPlatforms)
  : Plane(pos_th, ang_th, Size,Form("Chamber_%02d",ID) ), fID(ID)
  {
	for(Int_t i=0;i<6;i++) fDisplacementParameters[i] = 0.;
	  
	SetNPlatforms(NPlatforms);
	fPlatforms = new Plane[GetNPlatforms()];
	
	printf("ADRESS de platforms %x %x %x %x \n",fPlatforms,fPlatforms+1,fPlatforms+2,fPlatforms+3);
	
	CreateVolume(platforms);
	CalculateMatrixChgtGlobPlane(ang_th);
	CalculateMatrixChgtGlobPlane_v2(ang_th);
	printf("CHAMBER ID : %d \n", fID);
	printf("(*fOutputArray)[fID] %x\n",(*fOutputArray)[fID] );
	fAliROOTDisplacementMatrix = new((*fOutputArray)[fID]) TGeoHMatrix();
	printf("fAliROOTDisplacementMatrix %x\n", fAliROOTDisplacementMatrix);
	
	fAliROOTDisplacementMatrix->SetUniqueID(fID-6);

}
/* destructeur */
Chamber::~Chamber(){
	printf("Debut Destruc Chamber\n");
	delete[] fPlatforms;
	printf("Fin Destruc Chamber\n");
	
}

// ------------------------------------------------------------------------------------------------------------------
/// Creates the geometry objects to be displayed in the Open GL viewer.

void Chamber::CreateVolume(const Int_t * platforms){
	
	// Creating the Global volume of the chamber
	fGlobalRotation = new TGeoRotation(Form("Rot_Ch_Glob_%d",fID),  GetMatChgtPlaneGlob().ThetaX()*TMath::RadToDeg(), GetMatChgtPlaneGlob().PhiX()*TMath::RadToDeg()
											, GetMatChgtPlaneGlob().ThetaY()*TMath::RadToDeg(), GetMatChgtPlaneGlob().PhiY()*TMath::RadToDeg()
											, GetMatChgtPlaneGlob().ThetaZ()*TMath::RadToDeg(), GetMatChgtPlaneGlob().PhiZ()*TMath::RadToDeg());
	fGlobalCombiTrans = new TGeoCombiTrans(GetCenterTheo(0),GetCenterTheo(1),GetCenterTheo(2), fGlobalRotation);
	fVolGlobal = gGMSGeoManager->MakeBox(Form("Ch_Glob_%d",fID), med0, 2.*GetVolSize(0), 2.*GetVolSize(1), 2.*GetVolSize(2));
  	fVolGlobal->SetVisibility(kFALSE);
		
	// Creating the volume of the chamber itself
	fVolRotation = new TGeoRotation(Form("Rot_%s",fName.Data()), 0.,0.,0.);
	fVolCombiTrans = new TGeoCombiTrans(0.,0.,0., fVolRotation);

	fVolume = gGMSGeoManager->MakeBox(fName.Data(), med1, GetVolSize(0), GetVolSize(1), GetVolSize(2));
	//printf("$$$$$$$$$$$ Chamber %s @ %X\n",fName.Data(),this);
	//printf("$$$$$$$$$$$ fVolume add  %X\n",fVolume);
  	fVolume->SetLineColor(kBlack);
	fVolGlobal->AddNode(fVolume,1,fVolCombiTrans);
	
    for(Int_t i=0;i<4;i++) {
		Double_t pos[3], ang[3],size[3];
		LoadPlatformPosition(platforms[i],pos,ang,size);
		GetPlatform(i)->SetCenterTheo(pos);
		GetPlatform(i)->SetCenter(pos);
		GetPlatform(i)->SetRotationAngles(ang);
		GetPlatform(i)->SetVolumeSize(size);
		GetPlatform(i)->CalculateMatrixChgtGlobPlane(ang);
		TString PlatformName(Form("Platform_%d_Chamber_%d",platforms[i],fID));
		GetPlatform(i)->SetName(PlatformName);
		GetPlatform(i)->SetID(platforms[i]);
		
		// Creating the Global volume of the Platform i
		GetPlatform(i)->fGlobalRotation = new TGeoRotation(Form("Rot_Plat_Glob_%d_%02d",fID,platforms[i])
							, GetPlatform(i)->GetMatChgtPlaneGlob().ThetaX()*TMath::RadToDeg(), GetPlatform(i)->GetMatChgtPlaneGlob().PhiX()*TMath::RadToDeg()
							, GetPlatform(i)->GetMatChgtPlaneGlob().ThetaY()*TMath::RadToDeg(), GetPlatform(i)->GetMatChgtPlaneGlob().PhiY()*TMath::RadToDeg()
							, GetPlatform(i)->GetMatChgtPlaneGlob().ThetaZ()*TMath::RadToDeg(), GetPlatform(i)->GetMatChgtPlaneGlob().PhiZ()*TMath::RadToDeg());
		GetPlatform(i)->fGlobalCombiTrans = new TGeoCombiTrans(pos[0],pos[1],pos[2], GetPlatform(i)->fGlobalRotation);
		GetPlatform(i)->fVolGlobal = gGMSGeoManager->MakeBox(Form("Plat_Glob_%d_%02d",fID,platforms[i]), med0, size[0],size[1],size[2]);
		GetPlatform(i)->fVolGlobal->SetVisibility(kFALSE);
		GetPlatform(i)->fVolGlobal->Print();
		
		// Creating the volume of the platform i itself
		GetPlatform(i)->fVolRotation = new TGeoRotation(Form("Rot_Platform_%d_%02d",fID,platforms[i]), 0.,0.,0.);
		GetPlatform(i)->fVolCombiTrans = new TGeoCombiTrans(0.,0.,0., GetPlatform(i)->fVolRotation);

		GetPlatform(i)->fVolume = gGMSGeoManager->MakeBox(Form("Platform_%d_%02d",fID,platforms[i]), med1, size[0]/2.,size[1]/2.,size[2]/2.);
		
		GetPlatform(i)->fVolume->SetLineColor(kRed);
		GetPlatform(i)->fVolGlobal->AddNode(GetPlatform(i)->fVolume,1,GetPlatform(i)->fVolCombiTrans);		

		fVolGlobal->AddNode(GetPlatform(i)->fVolGlobal, 1, GetPlatform(i)->fGlobalCombiTrans) ;
		//printf("$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$\n");
		//printf("$$$$$$$$$$$ Plat_Glob_%d_%02d\n",fID,platforms[i]);
		//printf("$$$$$$$$$$$ Add Ch %X:  Add Plat %X\n",this,GetPlatform(i));
		//printf("$$$$$$$$$$$ Add Volume %X \n",GetPlatform(i)->fVolume);
		//printf("$$$$$$$$$$$ Add VolGlobal %X \n",GetPlatform(i)->fVolGlobal);
		//printf("$$$$$$$$$$$ nb of node %d \n",GetPlatform(i)->fVolGlobal->GetNtotal());
    }
      
	TOP->AddNode(fVolGlobal, 1, fGlobalCombiTrans);		
}


// ------------------------------------------------------------------------------------------------------------------
/// Load the position and orientation of the platform iPlatform

void Chamber::LoadPlatformPosition(Int_t iPlatform, Double_t *pos, Double_t *ang,  Double_t *size)
  {
    FILE * fp;
    if(!(fp = fopen(gGMSAnalyser->GetPlatformDBFileName(),"r")))
		AliFatal(Form("Enable to open chamber database file %s",gGMSAnalyser->GetPlatformDBFileName()));
	AliInfo(Form("Opening chamber database file %s. Looking for platform %d",gGMSAnalyser->GetPlatformDBFileName(),iPlatform));

    Int_t PlatformID, ChamberID;
    char line[256];
    while ( fgets ( line, sizeof(line), fp ) != NULL ){
		switch ( line [0] ){
			case '#': // Line commented
				break;	
			default:
			// Decode your values with a single scanf.
			sscanf ( line, " %d %lf %lf %lf %lf %lf %lf %lf %lf %lf ", &PlatformID, &pos[0], &pos[1], &pos[2]
														, &ang[0], &ang[1], &ang[2]
														, &size[0], &size[1], &size[2] );
			ChamberID = PlatformID/100;
			PlatformID = PlatformID%100;
			if((PlatformID == iPlatform)&&(ChamberID==fID)) {
				fclose ( fp );	
				for(int i = 0; i < 3; i++) ang[i] *= -1.;// Because the definition of Vanik is Opposite
				
				AliDebug(2,Form("Find position of Platform %d of Chamber %d",iPlatform,fID)); 
				AliDebug(2,Form("X = %.3f  Y = %.3f  Z = %.3f",pos[0],pos[1],pos[2]));  
				AliDebug(2,Form("ThX = %.3f  ThY = %.3f  ThZ = %.3f",ang[0],ang[1],ang[2]));  
				
				return;
			}    
			break;
		}	
    }	
    fclose ( fp );
    AliFatal(Form("Enable to find Position and Orientation of Platform %d of Chamber %d",iPlatform,fID));

  }

// ------------------------------------------------------------------------------------------------------------------
/// Return the platform "PlatformID"

Plane * Chamber::GetPlatformByID(Int_t PlatformID){
	
	for(int i = 0; i < 4; i++){
		if ( (fPlatforms+i)->GetID() == PlatformID) {
				return (fPlatforms+i);
		}
	}
	Fatal("Chamber::GetPlatformByID","Did not Find Platform : %02d\n",PlatformID);
	return NULL;
}

