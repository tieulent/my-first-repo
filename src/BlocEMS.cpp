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


#include <iostream>
using namespace std;

#include <TMath.h>
#include <TObjArray.h>

#include "Bloc.h"
#include "Box.h"
#include "GMSImage.h"
#include "BCAMImageResult.h"
#include "BoxBCAM.h"
#include "Chamber.h"
#include "Wall.h"
#include "BlocEMS.h"

//ClassImp(BlocEMS)


//______________________________________________________________________________

/** \brief Class constructor
	
	
	\author Raphael Tieulent IPN Lyon
	\date 2006-10-08
	\param  NLines Number of optical lines in this bloc
	\param  ch Pointer to the Chamber to which the EMS bloc is attached
	\param  BCAMch Table containing the BCAM boxes which are on the chamber
	\param  mur  Pointer to the Wall
	\param  BCAMmur Table containing the BCAM boxes which are on the wall
	\sa Bloc
**/
BlocEMS::BlocEMS(const int nLines) : Bloc(nLines,2*BoxBCAM::NB_MES){
  	SetBlocType(Bloc::kBlocEMS);
}

//BlocEMS::BlocEMS(const Int_t NLines,
//		      Chamber *const ch,Box *const *const BCAMch,
//		      Wall *const mur,Box *const *const BCAMmur)
//		      : Bloc(NLines,2*BoxBCAM::NB_MES)
//  {
//	SetBlocType(Bloc::kBlocEMS);
//    fCh = ch;
//    fWall = mur;
//    
//    fBCAMch = new Box*[NLines];
//    fBCAMwall = new Box*[NLines];
//    
//    for(Int_t i=0;i<NLines;i++){
//		fBCAMch[i] = BCAMch[i];
//		fBCAMwall[i] = BCAMmur[i];
//	}
//  }


//______________________________________________________________________________
BlocEMS::~BlocEMS()
  {
   // delete [] numWallSup;
  }

//______________________________________________________________________________

/** \brief Prints out to file "display_bloc.out" the addresses recorded in the boxes tables	
	
	\author Philippe Pillot - IPN Lyon
	\date 2006-10-08
**/
void BlocEMS::Display() const
  {
//    FILE *test_bloc = fopen("display_bloc.out","a");
//    fprintf(test_bloc,"display BlocEMS :\n");
//    fprintf(test_bloc,"	boites :\n");
//    for(Int_t i=0;i<fNLines;i++)
//      {
//        fprintf(test_bloc,"%x %x   %x %x\n",&(fBCAMch[i]),fBCAMch[i],
//					    &(fBCAMwall[i]),fBCAMwall[i]);
//      }
//    fprintf(test_bloc,"	chambres :\n");
//    fprintf(test_bloc,"%x %x\n",fCh,fWall);
//    fclose(test_bloc);
  }
  
//______________________________________________________________________________
/** \brief Reads the references images
	
		Reads the references images from data file
	
	\param  fp Pointer to the data file
	\param  PrintFlag PrintOut flag
**/
void BlocEMS::ReadRefImages(FILE * fp, Int_t PrintFlag){
	char sdump[20];
	Double_t XR, YR, XL, YL;		
	
	for(Int_t iline =0;iline<GetNLines();iline++){
			for(Int_t iimage =0;iimage<GetNMeasuresPerLine()/4;iimage++){

				fscanf(fp,"%s %lf %lf %lf %lf\n",&sdump,&XL,&YL,&XR,&YR);
 				if (PrintFlag) printf("Img0  Line %d -> Left=(%.2f,%.2f) Right=(%.2f,%.2f)\n"
 									,iline,XL,YL,XR,YR);
 				SetRefImage(iline,4*iimage,XL/1.e6);
				SetRefImage(iline,4*iimage+1,YL/1.e6);
				SetRefImage(iline,4*iimage+2,XR/1.e6);
				SetRefImage(iline,4*iimage+3,YR/1.e6);
			}
		}

}

//______________________________________________________________________________

/** \brief Reads the Final images
	
		Reads the Final images from data file
	
	\param  fp Pointer to the data file
	\param  PrintFlag PrintOut flag
**/
void BlocEMS::ReadFinalImages(FILE * fp, Int_t PrintFlag){
	char sdump[20];
	Double_t XR, YR, XL, YL;
	
	for(Int_t iline =0;iline<GetNLines();iline++){	
		for(Int_t iimage =0;iimage<GetNMeasuresPerLine()/4;iimage++){
			fscanf(fp,"%s %lf %lf %lf %lf\n",&sdump,&XL,&YL,&XR,&YR);
			if (PrintFlag) 
				printf("Data Bloc : Line %d Image %d -> Left=(%.2f,%.2f) Right=(%.2f,%.2f)\n"
					,iline,iimage,XL,YL,XR,YR);
			SetFinalImage(iline,4*iimage,XL/1.e6);
			SetFinalImage(iline,4*iimage+1,YL/1.e6);
			SetFinalImage(iline,4*iimage+2,XR/1.e6);
			SetFinalImage(iline,4*iimage+3,YR/1.e6);
		}
	}
}
//______________________________________________________________________________

/// calcul de la position de la LED sur l'image renvoyee par le BCAM 
void BlocEMS::CalcRefImages()
  {
    TVector3 pos_LED, FinalPosLED;

    for (Int_t i=0;i<fNLines;i++){

		GMSImage * currentImage =  GetImage(i);
		BCAMImageResult * referenceResult = (BCAMImageResult *) currentImage->GetReference();
		BoxBCAM * objBox = (BoxBCAM*)currentImage->GetObjectBox();
		BoxBCAM * ccdBox = (BoxBCAM*)currentImage->GetCCDBox();

		Float_t refImage[4];
		
		for (Int_t iLED=0;iLED<2;iLED++){
			// Box Wall -> Platform Wall
			if(iLED==0) pos_LED = objBox->GetLeftLED();
			else pos_LED = objBox->GetRightLED();

			FinalPosLED = objBox->GetMatChgtBoxPlan() * pos_LED;
			FinalPosLED += objBox->GetCenter();
			
			// Platform Wall -> Wall
			FinalPosLED *= (objBox->GetPlatform())->GetMatChgtPlaneGlob();
			FinalPosLED += (objBox->GetPlatform())->GetCenter();
			
			// Wall -> Global
			FinalPosLED *= (objBox->GetChamber())->GetMatChgtPlaneGlob();
			FinalPosLED += (objBox->GetChamber())->GetCenter();

			// Global -> Chamber
			FinalPosLED -= (ccdBox->GetChamber())->GetCenter();
			FinalPosLED *= (ccdBox->GetChamber())->GetMatChgtGlobPlane();

			// Chamber -> Platform chamber
			FinalPosLED -= (ccdBox->GetPlatform())->GetCenter();
			FinalPosLED *= (ccdBox->GetPlatform())->GetMatChgtGlobPlane();
			
			// Platform chamber -> Box Chamber
			FinalPosLED -= ccdBox->GetCenter();
			FinalPosLED *= ccdBox->GetMatChgtPlanBox();

			// boite CCD -> Center of the lens
			FinalPosLED -= ccdBox->GetPivot();

	    	// Calculation of the image projected on the CCD plan
			for(Int_t j =0;j<2;j++)
	    		refImage[2*iLED+j] = -(ccdBox->GetFocalLength()/FinalPosLED[2])*FinalPosLED[j];

			// Take Int_to account the bad orientation of the center of the CCD.
			Double_t IMGX, IMGY;
			refImage[2*iLED]   += ccdBox->GetFocalLength() 
										* TMath::Tan(ccdBox->GetTx()); 
			refImage[2*iLED+1] += ccdBox->GetFocalLength() 
										* TMath::Tan(ccdBox->GetTy()); 
			IMGX = refImage[2*iLED];
			IMGY = refImage[2*iLED+1];
				
			// Take Int_to account the rotation of the CCD around the optical axis
			refImage[2*iLED]   =  TMath::Cos(ccdBox->GetPsi())*IMGX 
											+ TMath::Sin(ccdBox->GetPsi())*IMGY; 
			refImage[2*iLED+1] = -TMath::Sin(ccdBox->GetPsi())*IMGX 
											+ TMath::Cos(ccdBox->GetPsi())*IMGY; 
			
			// Displace the center of the frame to the top left corner of the CCD 
			refImage[2*iLED]   -= BoxBCAM::CCDWidth/2.; 
			refImage[2*iLED+1] -= BoxBCAM::CCDHeight/2.; 
		
			// Rotate the axis to have Y axis down
			refImage[2*iLED]   = -refImage[2*iLED];  
			refImage[2*iLED+1] = -refImage[2*iLED+1];  
			
			// Record in the image object
			referenceResult->SetLED(iLED, refImage[2*iLED], refImage[2*iLED+1]);
		} // End loop over LED
	
	} // End loop over lines
  }


//______________________________________________________________________________

/// calcul de la position deplacee de la LED sur l'image renvoyee par les BCAM
/// fixes sur les murs, apres avoir deplacer les chambres a partir de leur position
/// theorique.
void BlocEMS::CalcTheoFinalImages()
  {
    TVector3 pos_LED, FinalPosLED;


    for (Int_t i=0;i<fNLines;i++){
		
		GMSImage * currentImage =  GetImage(i);
		BCAMImageResult * finalResult = (BCAMImageResult *) currentImage->GetResult();
		BoxBCAM * objBox = (BoxBCAM*)currentImage->GetObjectBox();
		BoxBCAM * ccdBox = (BoxBCAM*)currentImage->GetCCDBox();
		
		Float_t finalImage[4];
		
		for (Int_t iLED=0;iLED<2;iLED++){
			if(iLED==0) pos_LED = objBox->GetLeftLED();
			else pos_LED = objBox->GetRightLED();

			// boite LED -> plateforme LED deplacee
			FinalPosLED = objBox->GetMatChgtBoxPlan() * pos_LED;
			FinalPosLED += objBox->GetCenter();

			// plateforme LED deplacee -> plateforme LED
			FinalPosLED *= (objBox->GetPlatform())->GetDisplacementMatrix();
			FinalPosLED += (objBox->GetPlatform())->GetDisplacementVector();

			// plateforme LED -> chambre LED deplacee
			FinalPosLED *= (objBox->GetPlatform())->GetMatChgtPlaneGlob();
			FinalPosLED += (objBox->GetPlatform())->GetCenter();

			// chambre LED deplacee -> chambre LED theorique deplacee
			FinalPosLED *= (objBox->GetChamber())->GetMatChgtPlaneGlob() ;
			FinalPosLED += (objBox->GetChamber())->GetCenter();
			FinalPosLED -= (objBox->GetChamber())->GetCenterTheo();

			// chambre LED theorique deplacee -> chambre LED theorique
			FinalPosLED *= (objBox->GetChamber())->GetDisplacementMatrix() ;
			FinalPosLED += (objBox->GetChamber())->GetDisplacementVector();

			// chambre LED theorique -> global
			FinalPosLED += (objBox->GetChamber())->GetCenterTheo();

			// global -> chambre CCD theorique
			FinalPosLED -= (ccdBox->GetChamber())->GetCenterTheo();

			// chambre CCD theorique -> chambre CCD theorique deplacee
			FinalPosLED -= (ccdBox->GetChamber())->GetDisplacementVector();
			FinalPosLED *= (ccdBox->GetChamber())->GetDisplacementMatrixTransposed() ;

			// chambre CCD theorique deplacee -> chambre CCD deplacee
			FinalPosLED += (ccdBox->GetChamber())->GetCenterTheo();
			FinalPosLED -= (ccdBox->GetChamber())->GetCenter();
			FinalPosLED *= (ccdBox->GetChamber())->GetMatChgtGlobPlane() ;

			// chambre CCD deplacee -> plateforme CCD
			FinalPosLED -= (ccdBox->GetPlatform())->GetCenter();
			FinalPosLED *= (ccdBox->GetPlatform())->GetMatChgtGlobPlane() ;

			// plateforme CCD -> plateforme CCD deplacee
			FinalPosLED -= (ccdBox->GetPlatform())->GetDisplacementVector();
			FinalPosLED *= (ccdBox->GetPlatform())->GetDisplacementMatrixTransposed() ;

			// plateforme CCD deplacee -> boite CCD
			FinalPosLED -= ccdBox->GetCenter();
			FinalPosLED *= ccdBox->GetMatChgtPlanBox();

			// boite CCD -> Center of the lens
			FinalPosLED -= ccdBox->GetPivot();

	    	// Calculation of the image projected on the CCD plan
			for(Int_t j =0;j<2;j++)
	    		finalImage[2*iLED+j] = -(ccdBox->GetFocalLength()/FinalPosLED[2])*FinalPosLED[j];

			// Take Int_to account the bad orientation of the center of the CCD.
			Double_t IMGX, IMGY;
			finalImage[2*iLED]   += ccdBox->GetFocalLength()  * TMath::Tan(ccdBox->GetTx()); 
			finalImage[2*iLED+1] += ccdBox->GetFocalLength()  * TMath::Tan(ccdBox->GetTy()); 
			IMGX = finalImage[2*iLED];
			IMGY = finalImage[2*iLED+1];
				
			// Take Int_to account the rotation of the CCD around the optical axis
			finalImage[2*iLED]   =  TMath::Cos(ccdBox->GetPsi())*IMGX + TMath::Sin(ccdBox->GetPsi())*IMGY; 
			finalImage[2*iLED+1] = -TMath::Sin(ccdBox->GetPsi())*IMGX + TMath::Cos(ccdBox->GetPsi())*IMGY; 
			
			// Displace the center of the frame to the top left corner of the CCD 
			finalImage[2*iLED]   -= BoxBCAM::CCDWidth/2.; 
			finalImage[2*iLED+1] -= BoxBCAM::CCDHeight/2.; 
		
			// Rotate the axis to have Y axis down
			finalImage[2*iLED]   = -finalImage[2*iLED];  
			finalImage[2*iLED+1] = -finalImage[2*iLED+1];  
			
			// Record to the image object
			finalResult->SetLED(iLED, finalImage[2*iLED], finalImage[2*iLED+1]);
			
			// calcul Displacements
			currentImage->CalculateImageDisplacement();
		} // End loop over LED
	}// End loop over the lines
  }


//______________________________________________________________________________

/// calcul de la position deplacee de la LED sur l'image renvoyee par les BCAM
/// fixes sur les murs, apres avoir deplacer les chambres a partir de leur position
/// reelle ou mesuree.
void BlocEMS::CalcFinalImages()
  {

    TVector3 pos_LED, FinalPosLED;

    for (Int_t i=0;i<fNLines;i++){
		
		GMSImage * currentImage =  GetImage(i);
		BCAMImageResult * finalResult = (BCAMImageResult *) currentImage->GetResult();
		BoxBCAM * objBox = (BoxBCAM*)currentImage->GetObjectBox();
		BoxBCAM * ccdBox = (BoxBCAM*)currentImage->GetCCDBox();
		
		Float_t finalImage[4];
		
		for (Int_t iLED=0;iLED<2;iLED++){

			if(iLED==0) pos_LED = objBox->GetLeftLED();
			else pos_LED = objBox->GetRightLED();
			
			// boite LED -> plateforme LED deplacee
			FinalPosLED = objBox->GetMatChgtBoxPlan() * pos_LED;
			FinalPosLED += objBox->GetCenter();

			// plateforme LED deplacee -> plateforme LED
			 FinalPosLED *= (objBox->GetPlatform())->GetDisplacementMatrix();
			 FinalPosLED += (objBox->GetPlatform())->GetDisplacementVector();

			// plateforme LED -> chambre LED deplacee
			FinalPosLED *= (objBox->GetPlatform())->GetMatChgtPlaneGlob();
			FinalPosLED += (objBox->GetPlatform())->GetCenter();

			// chambre LED deplacee -> chambre LED
			FinalPosLED *= (objBox->GetChamber())->GetDisplacementMatrix() ;
			FinalPosLED += (objBox->GetChamber())->GetDisplacementVector();

			// chambre LED -> global
			FinalPosLED *= (objBox->GetChamber())->GetMatChgtPlaneGlob() ;
			FinalPosLED += (objBox->GetChamber())->GetCenter();

			// global -> chambre CCD
			FinalPosLED -= (ccdBox->GetChamber())->GetCenter();
			FinalPosLED *= (ccdBox->GetChamber())->GetMatChgtGlobPlane() ;

			// chambre CCD -> chambre CCD deplacee
			FinalPosLED -= (ccdBox->GetChamber())->GetDisplacementVector();
			FinalPosLED *= (ccdBox->GetChamber())->GetDisplacementMatrixTransposed() ;

			// chambre CCD deplacee -> plateforme CCD
			FinalPosLED -= (ccdBox->GetPlatform())->GetCenter();
			FinalPosLED *= (ccdBox->GetPlatform())->GetMatChgtGlobPlane();

			// plateforme CCD -> plateforme CCD deplacee
			FinalPosLED -= (ccdBox->GetPlatform())->GetDisplacementVector();
			FinalPosLED *= (ccdBox->GetPlatform())->GetDisplacementMatrixTransposed();

			// plateforme CCD deplacee -> boite CCD
			FinalPosLED -= ccdBox->GetCenter();
			FinalPosLED *= ccdBox->GetMatChgtPlanBox();
			
			// boite CCD -> Center of the lens
			FinalPosLED -= ccdBox->GetPivot();

	    	// Calculation of the image projected on the CCD plan
			for(Int_t j =0;j<2;j++)
	    		finalImage[2*iLED+j] = -(ccdBox->GetFocalLength()/FinalPosLED[2])*FinalPosLED[j];

			// Take Int_to account the bad orientation of the center of the CCD.
			Double_t IMGX, IMGY;
			finalImage[2*iLED]   += ccdBox->GetFocalLength() * TMath::Tan(ccdBox->GetTx()); 
			finalImage[2*iLED+1] += ccdBox->GetFocalLength() * TMath::Tan(ccdBox->GetTy()); 
			IMGX = finalImage[2*iLED];
			IMGY = finalImage[2*iLED+1];
				
			// Take Int_to account the rotation of the CCD around the optical axis
			finalImage[2*iLED]   =  TMath::Cos(ccdBox->GetPsi())*IMGX + TMath::Sin(ccdBox->GetPsi())*IMGY; 
			finalImage[2*iLED+1] = -TMath::Sin(ccdBox->GetPsi())*IMGX + TMath::Cos(ccdBox->GetPsi())*IMGY; 
			
			// Displace the center of the frame to the top left corner of the CCD 
			finalImage[2*iLED]   -= BoxBCAM::CCDWidth/2.; 
			finalImage[2*iLED+1] -= BoxBCAM::CCDHeight/2.; 
		
			// Rotate the axis to have Y axis down
			finalImage[2*iLED]   = -finalImage[2*iLED];  
			finalImage[2*iLED+1] = -finalImage[2*iLED+1];  
			
			// Record to the image object
			finalResult->SetLED(iLED, finalImage[2*iLED], finalImage[2*iLED+1]);
			
			// calcul Displacements
			currentImage->CalculateImageDisplacement();
			
		} // end loop over LED
	
	} // End loop over lines
  }


//______________________________________________________________________________

/// Add the Intrinsic error of the measurement
void BlocEMS::AddIntrinsicError(TRandom3 *rand_obj,Double_t IntResFactor){
	
	for (Int_t i=0;i<fNLines;i++){
		
		GMSImage * currentImage =  GetImage(i);
		BCAMImageResult * referenceResult = (BCAMImageResult *) currentImage->GetReference();
		BCAMImageResult * finalResult = (BCAMImageResult *) currentImage->GetResult();

		
		referenceResult->SetXleft(rand_obj->Gaus(referenceResult->GetXleft(),IntResFactor*BoxBCAM::SIGMA_X_BCAM));
		referenceResult->SetYleft(rand_obj->Gaus(referenceResult->GetYleft(),IntResFactor*BoxBCAM::SIGMA_Y_BCAM));
		referenceResult->SetXright(rand_obj->Gaus(referenceResult->GetXright(),IntResFactor*BoxBCAM::SIGMA_X_BCAM));
		referenceResult->SetYright(rand_obj->Gaus(referenceResult->GetYright(),IntResFactor*BoxBCAM::SIGMA_Y_BCAM));
		
		finalResult->SetXleft(rand_obj->Gaus(finalResult->GetXleft(),IntResFactor*BoxBCAM::SIGMA_X_BCAM));
		finalResult->SetYleft(rand_obj->Gaus(finalResult->GetYleft(),IntResFactor*BoxBCAM::SIGMA_Y_BCAM));
		finalResult->SetXright(rand_obj->Gaus(finalResult->GetXright(),IntResFactor*BoxBCAM::SIGMA_X_BCAM));
		finalResult->SetYright(rand_obj->Gaus(finalResult->GetYright(),IntResFactor*BoxBCAM::SIGMA_Y_BCAM));
		
		// calcul Displacements
		currentImage->CalculateImageDisplacement();
	
	}
}
void BlocEMS::PrintFinalImages(){}
void BlocEMS::PrintRefImages(){}
void BlocEMS::PrintDisplacements(){}

