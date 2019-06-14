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
#include "BCAMImageResult.h"
#include "GMSImage.h"
#include "BoxBCAM.h"
#include "Chamber.h"
#include "BlocLMSBCAM.h"

//ClassImp(BlocLMSBCAM)

BlocLMSBCAM::BlocLMSBCAM(const int nLines) : Bloc(nLines,2*BoxBCAM::NB_MES){
  	SetBlocType(Bloc::kBlocLMS);
}
	
//BlocLMSBCAM::BlocLMSBCAM(const int nb_lignes,
//		  Chamber *const ch1,Box *const *const BCAMch1,
//		  Chamber *const ch2,Box *const *const BCAMch2)
//		  : Bloc(nb_lignes,2*BoxBCAM::NB_MES)
//  {
//  	SetBlocType(Bloc::kBlocLMS);
//	this->fCh = new Chamber*[2];
//	fCh[0] = ch1;
//	fCh[1] = ch2;
//	
//	fBCAM = new Box**[2];
//	for(int i=0;i<2;i++) fBCAM[i] =new Box*[nb_lignes];
//
//	for(int i=0;i<nb_lignes;i++){
//		fBCAM[0][i] = BCAMch1[i];
//		fBCAM[1][i] = BCAMch2[i];
//	}
// }


/// Destructor of the class
BlocLMSBCAM::~BlocLMSBCAM()
  {
//  	for(int i=0;i<2;i++) delete [] fBCAM[i];
//	delete [] fBCAM;
//	delete [] fCh;
  }


/** \brief Print to stdout the addresses recorded into the Box table 
	\author Raphael Tieulent IPN-Lyon
	\date 2006-10-02
	\sa
**/
void BlocLMSBCAM::Display() const
  {
//    FILE *test_bloc = fopen("display_bloc.out","a");
//    fprintf(test_bloc,"display BlocLMSBCAM :\n");
//    fprintf(test_bloc,"	boites :\n");
//    for(int i=0;i<fNLines;i++)
//        fprintf(test_bloc,"%x %x   %x %x\n",&(fBCAM[0][i]),fBCAM[0][i],
//					    &(fBCAM[1][i]),fBCAM[1][i]);
//    fprintf(test_bloc,"	chambres :\n");
//    fprintf(test_bloc,"%x %x\n",fCh[0],fCh[1]);
//    fclose(test_bloc);
  }

  
void BlocLMSBCAM::ReadRefImages(FILE * fp, int PrintFlag){
	char sdump[20];
	double XR, YR, XL, YL;		
	
	for(int iline =0;iline<GetNLines();iline++){
			for(int iimage =0;iimage<GetNMeasuresPerLine()/4;iimage++){

				fscanf(fp,"%s %lf %lf %lf %lf\n",&sdump,&XL,&YL,&XR,&YR);
 				if (PrintFlag) printf("Img0  Line %d -> Left=(%.2f,%.2f) Right=(%.2f,%.2f)    dX=%.2f  dY=%.2f dL=%.2f\n"
 									,iline,XL,YL,XR,YR,(XR-XL),YR-YL,TMath::Sqrt((XR-XL)*(XR-XL)+(YR-YL)*(YR-YL)));
 				SetRefImage(iline,4*iimage,XL/1.e6);
				SetRefImage(iline,4*iimage+1,YL/1.e6);
				SetRefImage(iline,4*iimage+2,XR/1.e6);
				SetRefImage(iline,4*iimage+3,YR/1.e6);
			}
		}

}
  
void BlocLMSBCAM::ReadFinalImages(FILE * fp, int PrintFlag){
	char sdump[20];
	double XR, YR, XL, YL;
	
	for(int iline =0;iline<GetNLines();iline++){	
		for(int iimage =0;iimage<GetNMeasuresPerLine()/4;iimage++){
			fscanf(fp,"%s %lf %lf %lf %lf\n",&sdump,&XL,&YL,&XR,&YR);
			if (PrintFlag) 
				printf("Data Bloc : Line %d Image %d -> Left=(%.2f,%.2f) Right=(%.2f,%.2f)    dX=%.2f  dY=%.2f dL=%.2f\n"
					,iline,iimage,XL,YL,XR,YR,(XR-XL),YR-YL,TMath::Sqrt((XR-XL)*(XR-XL)+(YR-YL)*(YR-YL)));
			SetFinalImage(iline,4*iimage,XL/1.e6);
			SetFinalImage(iline,4*iimage+1,YL/1.e6);
			SetFinalImage(iline,4*iimage+2,XR/1.e6);
			SetFinalImage(iline,4*iimage+3,YR/1.e6);
		}
	}
	CalcNewAngle();

}
void BlocLMSBCAM::CalcNewAngle(){
//	char sdump[20];
//	double XR, YR, XL, YL;
//	
//	for(int i =0;i<GetNLines();i++){	
//		for(int iBCAM=0 ;iBCAM<2 ;iBCAM++ ){
//			int ich1, ich2;
//			if( iBCAM == 0 ){
//				ich1 = 0;
//				ich2 = 1;
//			}else{
//				ich1 = 1;
//				ich2 = 0;
//			}
//			
//			for (int iLED=0;iLED<2;iLED++){
//				double F = ((BoxBCAM*)fBCAM[ich1][i])->GetFocalLength()	;
//				double DiffX = GetImageDisplacement(i,4*iBCAM + 2*iLED );
//				double DiffY = GetImageDisplacement(i,4*iBCAM + 2*iLED +1);
//				printf("Line %d BCAM %d LED %d : Diff X = %.2f  Ang X  = %.9f   Diff Y = %.2f  Ang Y  = %.9f \n"
//					,i,iBCAM,iLED,DiffX*1.e6,DiffX/F,DiffY*1.e6,DiffY/F);
//			}
//		}
//	}
}
/// Calculate the position of the LED image on the BCAM CCD
void BlocLMSBCAM::CalcRefImages()
  {
    TVector3 pos_LED,FinalPosLED;

    for (int i=0;i<fNLines;i++){
		
		GMSImage * currentImage =  GetImage(i);
		BCAMImageResult * referenceResult = (BCAMImageResult *) currentImage->GetReference();
		BoxBCAM * objBox = (BoxBCAM*)currentImage->GetObjectBox();
		BoxBCAM * ccdBox = (BoxBCAM*)currentImage->GetCCDBox();
		
		Float_t refImage[4];
		
		for (int iLED=0;iLED<2;iLED++){
				if(iLED==0) pos_LED = objBox->GetLeftLED();
				else pos_LED = objBox->GetRightLED();
				//printf("\n\npos led %f %f %f \n",pos_LED[0],pos_LED[1],pos_LED[2]);
				// boite LED -> plateforme LED
				FinalPosLED = objBox->GetMatChgtBoxPlan() * pos_LED;
				FinalPosLED += objBox->GetCenter();
				//printf("Plat LED %f %f %f \n",FinalPosLED[0],FinalPosLED[1],FinalPosLED[2]);
	    
				// plateforme LED -> chambre LED
				FinalPosLED *= (objBox->GetPlatform())->GetMatChgtPlaneGlob();
				FinalPosLED += (objBox->GetPlatform())->GetCenter();
				//printf("ch LED %f %f %f \n",FinalPosLED[0],FinalPosLED[1],FinalPosLED[2]);

				// chambre LED -> global
				FinalPosLED *= (objBox->GetChamber())->GetMatChgtPlaneGlob();
				FinalPosLED += (objBox->GetChamber())->GetCenter();
				//printf("Global %f %f %f \n",FinalPosLED[0],FinalPosLED[1],FinalPosLED[2]);

				// global -> chambre CCD
				FinalPosLED -= (ccdBox->GetChamber())->GetCenter();
				FinalPosLED *= (ccdBox->GetChamber())->GetMatChgtGlobPlane();
				//printf("ch CCD %f %f %f \n",FinalPosLED[0],FinalPosLED[1],FinalPosLED[2]);

				// chambre CCD -> plateforme CCD
				 FinalPosLED -= (ccdBox->GetChamber())->GetCenter();
				 FinalPosLED *= (ccdBox->GetChamber())->GetMatChgtGlobPlane();
				//printf("Plat CCD %f %f %f \n",FinalPosLED[0],FinalPosLED[1],FinalPosLED[2]);

				// plateforme CCD -> boite CCD
				FinalPosLED -= ccdBox->GetCenter();
				FinalPosLED *= ccdBox->GetMatChgtPlanBox();
				//printf("BCAM CCD %f %f %f \n",FinalPosLED[0],FinalPosLED[1],FinalPosLED[2]);

				// boite CCD -> Center of the lens
				FinalPosLED -= ccdBox->GetPivot();
				//printf("Lens Center %f %f %f \n",FinalPosLED[0],FinalPosLED[1],FinalPosLED[2]);
	    
	    		// Calculation of the image projected on the CCD plan
				for(int j =0;j<2;j++)
	    			refImage[2*iLED+j] = -(ccdBox->GetFocalLength()/FinalPosLED[2]) *FinalPosLED[j];
		
			// Take into account the bad orientation of the center of the CCD.
				double IMGX, IMGY;
				refImage[2*iLED]   += ccdBox->GetFocalLength()*TMath::Tan(ccdBox->GetTx()); 
				refImage[2*iLED+1] += ccdBox->GetFocalLength()*TMath::Tan(ccdBox->GetTy()); 

				IMGX = refImage[2*iLED];
				IMGY = refImage[2*iLED+1];
				
			// Take into account the rotation of the CCD around the optical axis
				refImage[2*iLED]   =  TMath::Cos(ccdBox->GetPsi())*IMGX + TMath::Sin(ccdBox->GetPsi())*IMGY; 
				refImage[2*iLED+1] = -TMath::Sin(ccdBox->GetPsi())*IMGX + TMath::Cos(ccdBox->GetPsi())*IMGY; 
			
			// Displace the center of the frame to the top left corner of the CCD 
				refImage[2*iLED]   -= BoxBCAM::CCDWidth/2.; 
				refImage[2*iLED+1] -= BoxBCAM::CCDHeight/2.; 
		
			// Rotate the axis to have Y axis down
				refImage[2*iLED]   = -refImage[2*iLED];  
				refImage[2*iLED+1] = -refImage[2*iLED+1];  

			// Record in the image object
			referenceResult->SetLED(iLED, refImage[2*iLED], refImage[2*iLED+1]);

			} // end loop over the LED
	} // End loop over the line of the given Bloc
  }


/// calcul de la position deplacee de la LED sur l'image renvoyee par les BCAM
/// apres avoir deplacer les chambres a partir de leur position theorique.
void BlocLMSBCAM::CalcTheoFinalImages()
  {
    TVector3 pos_LED,FinalPosLED;
	
    for (int i=0;i<fNLines;i++){
		
		GMSImage * currentImage =  GetImage(i);
		BCAMImageResult * finalResult = (BCAMImageResult *) currentImage->GetResult();
		BoxBCAM * objBox = (BoxBCAM*)currentImage->GetObjectBox();
		BoxBCAM * ccdBox = (BoxBCAM*)currentImage->GetCCDBox();
		
		Float_t finalImage[4];
		
		for (int iLED=0;iLED<2;iLED++){

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
			FinalPosLED *= (objBox->GetChamber())->GetMatChgtPlaneGlob();
			FinalPosLED += (objBox->GetChamber())->GetCenter();
			FinalPosLED -= (objBox->GetChamber())->GetCenterTheo();

			// chambre LED theorique deplacee -> chambre LED theorique
			FinalPosLED *= (objBox->GetChamber())->GetDisplacementMatrix();
			FinalPosLED += (objBox->GetChamber())->GetDisplacementVector();

			// chambre LED theorique -> global
			FinalPosLED += (objBox->GetChamber())->GetCenterTheo();

			// global -> chambre CCD theorique
			FinalPosLED -= (ccdBox->GetChamber())->GetCenterTheo();

			// chambre CCD theorique -> chambre CCD theorique deplacee
			FinalPosLED -= (ccdBox->GetChamber())->GetDisplacementVector();
			FinalPosLED *= (ccdBox->GetChamber())->GetDisplacementMatrixTransposed();

			// chambre CCD theorique deplacee -> chambre CCD deplacee
			FinalPosLED += (ccdBox->GetChamber())->GetCenterTheo();
			FinalPosLED -= (ccdBox->GetChamber())->GetCenter();
			FinalPosLED *= (ccdBox->GetChamber())->GetMatChgtGlobPlane();

			// chambre CCD deplacee -> plateforme CCD
			FinalPosLED -= (ccdBox->GetPlatform())->GetCenter();
			FinalPosLED *= (ccdBox->GetPlatform())->GetMatChgtGlobPlane();

			// plateforme CCD -> plateforme CCD deplacee
			FinalPosLED -=  (ccdBox->GetPlatform())->GetDisplacementVector();
			FinalPosLED *= 	(ccdBox->GetPlatform())->GetDisplacementMatrixTransposed();

			// plateforme CCD deplacee -> boite CCD
			FinalPosLED -= ccdBox->GetCenter();
			FinalPosLED *= ccdBox->GetMatChgtPlanBox();

			// boite CCD -> Center of the lens
			FinalPosLED -= ccdBox->GetPivot();
				
			// Calculation of the image projected on the CCD plan
			for(int j =0;j<2;j++)
				finalImage[2*iLED+j] = -(ccdBox->GetFocalLength()/FinalPosLED[2]) *FinalPosLED[j];


			// Take into account the bad orientation of the center of the CCD.
			double IMGX, IMGY;
			finalImage[2*iLED]   += ccdBox->GetFocalLength()*TMath::Tan(ccdBox->GetTx()); 
			finalImage[2*iLED+1] += ccdBox->GetFocalLength()*TMath::Tan(ccdBox->GetTy()); 

			IMGX = finalImage[2*iLED];
			IMGY = finalImage[2*iLED+1];

			// Take into account the rotation of the CCD around the optical axis
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
									 
			// test si les chambres ne se sont pas croisees 
			if (FinalPosLED[2]<=0) cout<<"des chambres se croisent (BCAM) !!!"<<endl;

		} // End loop over LED
	}// End loop over the line of the bloc
  }

//------------------------------------------------------------------------------------------------------------------------------------------
/* calcul de la position deplacee de la LED sur l'image renvoyee par les BCAM
apres avoir deplacer les chambres a partir de leur position reelle ou mesuree.*/
void BlocLMSBCAM::CalcFinalImages()
  {
    TVector3 pos_LED,FinalPosLED;

    for (int i=0;i<fNLines;i++){
		
		GMSImage * currentImage =  GetImage(i);
		BCAMImageResult * finalResult = (BCAMImageResult *) currentImage->GetResult();
		BoxBCAM * objBox = (BoxBCAM*)currentImage->GetObjectBox();
		BoxBCAM * ccdBox = (BoxBCAM*)currentImage->GetCCDBox();
		
		Float_t finalImage[4];
				
		for (int iLED=0;iLED<2;iLED++){

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
			FinalPosLED *= (objBox->GetChamber())->GetDisplacementMatrix();
			FinalPosLED += (objBox->GetChamber())->GetDisplacementVector();

			// chambre LED -> global
			FinalPosLED *= (objBox->GetChamber())->GetMatChgtPlaneGlob();
			FinalPosLED += (objBox->GetChamber())->GetCenter();

			// global -> chambre CCD
			FinalPosLED -= (ccdBox->GetChamber())->GetCenter();
			FinalPosLED *= (ccdBox->GetChamber())->GetMatChgtGlobPlane();

			// chambre CCD -> chambre CCD deplacee
			FinalPosLED -= (ccdBox->GetChamber())->GetDisplacementVector();
			FinalPosLED *= (ccdBox->GetChamber())->GetDisplacementMatrixTransposed();

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
			for(int j =0;j<2;j++) finalImage[2*iLED+j] = -(ccdBox->GetFocalLength()/FinalPosLED[2]) *FinalPosLED[j];


			// Take into account the bad orientation of the center of the CCD.
			double IMGX, IMGY;
			finalImage[2*iLED]   += ccdBox->GetFocalLength()*TMath::Tan(ccdBox->GetTx()); 
			finalImage[2*iLED+1] += ccdBox->GetFocalLength()*TMath::Tan(ccdBox->GetTy()); 

			IMGX = finalImage[2*iLED];
			IMGY = finalImage[2*iLED+1];

			// Take into account the rotation of the CCD around the optical axis
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

			// test si les chambres ne se sont pas croisees 
			if (FinalPosLED[2]<=0) cout<<"des chambres se croisent (BCAM) !!!"<<endl;

		} // End loop over the LED
		
		// calcul Displacements
		currentImage->CalculateImageDisplacement();
		
	} // End loop over the line
}



/// Add the Intrinsic error of the measurement
void BlocLMSBCAM::AddIntrinsicError(TRandom3 *rand_obj,Double_t IntResFactor){
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
void BlocLMSBCAM::PrintFinalImages(){}
void BlocLMSBCAM::PrintRefImages(){}
void BlocLMSBCAM::PrintDisplacements(){}
