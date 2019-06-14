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

#include "Chamber.h"
#include "Box.h"
#include "Bloc.h"
#include "BoxBCAM.h"
#include "BCAMImageResult.h"
#include "GMSImage.h"

#include "BlocTMS.h"

//ClassImp(BlocTMS)


/// Constructor of the class
BlocTMS::BlocTMS(const int nb_lignes, Chamber *const ch, Box *const *const BCAM1,
		Box *const *const BCAM2) : Bloc(nb_lignes,BoxBCAM::NB_MES)
  {
//	SetBlocType(Bloc::kBlocTMS);
//    this->fCh = ch;
//    fLineNames = new TString[nb_lignes];
//	
//	fBCAM = new Box**[2];
//	for(int i=0;i<2;i++) fBCAM[i] = new Box*[nb_lignes];
//
//	for(int i=0;i<nb_lignes;i++){
//		fBCAM[0][i] = BCAM1[i];
//		fBCAM[1][i] = BCAM2[i];
//		fLineNames[i] = Form("TMSCH%02dP%dP%d",BCAM1[i]->GetChamberID(),BCAM1[i]->GetLocationID()/10,BCAM2[i]->GetLocationID()/10);
//	}
	  
  }
BlocTMS::BlocTMS(const int nb_lignes) : Bloc(nb_lignes,BoxBCAM::NB_MES)
{
}	

/// Destructor of the class
BlocTMS::~BlocTMS()
  {
//	for(int i=0;i<2;i++) delete [] fBCAM[i];
//	delete [] fBCAM;
//    for(Int_t i=0;i<2;i++)delete [] fLineNames[i];
//    delete [] fLineNames;
  }


/// Print to the standard output the memory address of each box which is part of this bloc.
void BlocTMS::Display() const{
//	FILE *test_bloc = fopen("display_bloc.out","a");
//	fprintf(test_bloc,"display BlocTMS :\n");
//	fprintf(test_bloc,"	boites :\n");
//	for(int i=0;i<fNLines;i++)
//		fprintf(test_bloc,"%x %x   %x %x\n",&(fBCAM[0][i]),fBCAM[0][i], &(fBCAM[1][i]),fBCAM[1][i]);
//	fprintf(test_bloc,"	chambre :\n");
//	fprintf(test_bloc,"%x\n",fCh);
//	fclose(test_bloc);
}


void BlocTMS::CalcRefImages(){
	/// Calculate the position of the LED image on the CCD using the 3D setup.
	
	/// This position is given in the same reference frame as it is given by the acquisition programm of the GMS.
	/// The positions of the platforms and BCAM boxes are taken from the 3D setup file. 
	//Info("BlocTMS::CalcRefImages","Start\n");

    TVector3 pos_LED, FinalPosLED;
	
    for (int i=0;i<fNLines;i++){
		
		GMSImage * currentImage =  GetImage(i);
		BCAMImageResult * referenceResult = (BCAMImageResult *) currentImage->GetReference();
		BoxBCAM * objBox = (BoxBCAM*)currentImage->GetObjectBox();
		BoxBCAM * ccdBox = (BoxBCAM*)currentImage->GetCCDBox();
		
		TArrayF refImage(4);
		

		for (int iLED=0;iLED<2;iLED++){
				if(iLED==0) pos_LED = objBox->GetLeftLED();
				else pos_LED = objBox->GetRightLED();
				
	    	// boite LED -> plateforme LED
	    		FinalPosLED = objBox->GetMatChgtBoxPlan() * pos_LED;
				FinalPosLED += objBox->GetCenter();
	    
	    	// plateforme LED -> chambre
	    		FinalPosLED *= (objBox->GetPlatform())->GetMatChgtPlaneGlob() ;
				FinalPosLED += (objBox->GetPlatform())->GetCenter();
	    
	    	// chambre -> plateforme CCD
				FinalPosLED -= (ccdBox->GetPlatform())->GetCenter();
				FinalPosLED *= (ccdBox->GetPlatform())->GetMatChgtGlobPlane();
	    
	    	// plateforme CCD -> boite CCD
	    		FinalPosLED -= ccdBox->GetCenter();
				FinalPosLED *= ccdBox->GetMatChgtPlanBox();
	    	
			// boite CCD -> Center of the lens
				FinalPosLED -= ccdBox->GetPivot();

	    	// Calculation of the image projected on the CCD plan
				for(int j =0;j<2;j++)
	    			refImage[2*iLED+j] = -(ccdBox->GetFocalLength()/TMath::Abs(FinalPosLED[2])) *FinalPosLED[j];
			
			// Take into account the bad orientation of the optical axis of the BCAM compare to the axis of the Box.
				double IMGX, IMGY;
				refImage[2*iLED]   += ccdBox->GetFocalLength()*TMath::Tan(ccdBox->GetTx()); 
				refImage[2*iLED+1] += ccdBox->GetFocalLength()*TMath::Tan(ccdBox->GetTy()); 
				
				IMGX = refImage[2*iLED];
				IMGY = refImage[2*iLED+1];
				
			// Take into account the rotation of the CCD around the optical axis
				refImage[2*iLED]   =  TMath::Cos(ccdBox->GetPsi())*IMGX + TMath::Sin(ccdBox->GetPsi())*IMGY; 
				refImage[2*iLED+1] = -TMath::Sin(ccdBox->GetPsi())*IMGX + TMath::Cos(ccdBox->GetPsi())*IMGY; 
			
			// Rotate the axis to have Y axis down if Azimuthal BCAM
				if(TMath::Abs(ccdBox->GetType()) == 1 )
				{
					refImage[2*iLED]   = -refImage[2*iLED];  
					refImage[2*iLED+1] = -refImage[2*iLED+1];  
					///@todo Pourquoi est ce que je fais ca ici sur le TMS et pas pour le LMS ou le EMS ?????
				}
				
			// Displace the center of the frame to the top left corner of the CCD
				refImage[2*iLED]   += BoxBCAM::CCDWidth/2.; 
				refImage[2*iLED+1] += BoxBCAM::CCDHeight/2.; 
		
				if(iLED ==1 )
				{					
					///@todo Est il vraiment nécessaire de réaranger les spot ??? Je ne le fais ni pour l'EMS ni pour le LMS
					if(refImage[0] > refImage[2])
					{
						for( int ii = 0; ii < 2; ii++ )
						{
							Double_t Temp = refImage[2 + ii];
							refImage[2 + ii] = refImage[ii];
							refImage[ii] = Temp;
						}
						
					}	
//					double XL, XR, YL, YR;
//					XL = 1.e6*fRefImage[i][0];
//					YL = 1.e6*fRefImage[i][1];
//					XR = 1.e6*fRefImage[i][2];
//					YR = 1.e6*fRefImage[i][3];
								
					// printf("%s : %.2f %.2f %.2f %.2f  dX=%.2f  dY=%.2f dL=%.2f\n ", 
					// 	GetImageName(i).Data(),XL,YL,XR,YR,(XR-XL),YR-YL,TMath::Sqrt((XR-XL)*(XR-XL)+(YR-YL)*(YR-YL)));

				}
			// Record in the image object
			referenceResult->SetLED(iLED, refImage[2*iLED], refImage[2*iLED+1]);
				
				
				
	  		} // End loop over LEDs
	} // End loop over lines
	// Info("BlocTMS::CalcRefImages","End\n");


}

/// calcul de la position deplacee de la LED sur l'image renvoyee par les BCAM
/// apres avoir deplacer les chambres a partir de leur position theorique.
void BlocTMS::CalcTheoFinalImages()
  {
    CalcFinalImages();
  }


/// calcul de la position deplacee de la LED sur l'image renvoyee par les BCAM
/// apres avoir deplacer les chambres a partir de leur position reelle ou mesuree.
void BlocTMS::CalcFinalImages()
  {
	  
    TVector3 pos_LED, FinalPosLED;
	  
    for (int i=0;i<fNLines;i++){

		GMSImage * currentImage =  GetImage(i);
		BCAMImageResult * finalResult = (BCAMImageResult *) currentImage->GetResult();
		BoxBCAM * objBox = (BoxBCAM*)currentImage->GetObjectBox();
		BoxBCAM * ccdBox = (BoxBCAM*)currentImage->GetCCDBox();
		
		TArrayF finalImage(4);
				
			for (int iLED=0;iLED<2;iLED++){
				if(iLED==0) pos_LED = objBox->GetLeftLED();
				else pos_LED = objBox->GetRightLED();
				 
			// boite LED -> plateforme LED deplacee
				FinalPosLED = objBox->GetMatChgtBoxPlan() * pos_LED;
				FinalPosLED += objBox->GetCenter();
				 
			// plateforme LED deplacee -> plateforme LED
				FinalPosLED *= (objBox->GetPlatform())->GetDisplacementMatrix();				
				FinalPosLED += (objBox->GetPlatform())->GetDisplacementVector();
				 
			// plateforme LED -> chambre
				FinalPosLED *= (objBox->GetPlatform())->GetMatChgtPlaneGlob();
				FinalPosLED += (objBox->GetPlatform())->GetCenter();
				 
			// chambre -> plateforme CCD
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
				for(int j =0;j<2;j++)
	    			finalImage[2*iLED+j] = -(ccdBox->GetFocalLength()/TMath::Abs(FinalPosLED[2])) * FinalPosLED[j];
		
			// Take into account the bad orientation of the center of the CCD.
				double IMGX, IMGY;
				finalImage[2*iLED]   += ccdBox->GetFocalLength()*TMath::Tan(ccdBox->GetTx()); 
				finalImage[2*iLED+1] += ccdBox->GetFocalLength()*TMath::Tan(ccdBox->GetTy()); 

				IMGX = finalImage[2*iLED];
				IMGY = finalImage[2*iLED+1];
				
			// Take into account the rotation of the CCD around the optical axis
			finalImage[2*iLED]   =  TMath::Cos(ccdBox->GetPsi())*IMGX + TMath::Sin(ccdBox->GetPsi())*IMGY; 
			finalImage[2*iLED+1] = -TMath::Sin(ccdBox->GetPsi())*IMGX + TMath::Cos(ccdBox->GetPsi())*IMGY; 
			
			// Rotate the axis to have Y axis down if Azimuthal BCAM
				if(TMath::Abs(ccdBox->GetType()) == 1 )
				{
					finalImage[2*iLED]   = -finalImage[2*iLED];  
					finalImage[2*iLED+1] = -finalImage[2*iLED+1];  
				}
			
			// Displace the center of the frame to the top left corner of the CCD 
				finalImage[2*iLED]   += BoxBCAM::CCDWidth/2.; 
				finalImage[2*iLED+1] += BoxBCAM::CCDHeight/2.; 
				
				if(iLED ==1 ) 
				{		
					if(finalImage[0] > finalImage[2])
					{
						for( int ii = 0; ii < 2; ii++ )
						{
							Double_t Temp = finalImage[2 + ii];
							finalImage[2 + ii] = finalImage[ii];
							finalImage[ii] = Temp;
						}		
					}				
				 
				}
				
				// Record to the image object
				finalResult->SetLED(iLED, finalImage[2*iLED], finalImage[2*iLED+1]);
				
				
			// test si les chambres ne se sont pas croisees 
				if (FinalPosLED[2]<=0) cout<<"des chambres se croisent (BCAM) !!!"<<endl;

			} //End loop over LEDs
		
		// calcul Displacements
		currentImage->CalculateImageDisplacement();
		
	} // End loop over optical lines
  }
  
  
void BlocTMS::ReadRefImages(FILE * fp, int PrintFlag){
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
  
void BlocTMS::ReadFinalImages(FILE * fp, int PrintFlag){
	char sdump[20];
	double XR, YR, XL, YL;
	
	for(int iline =0;iline<GetNLines();iline++){	
		for(int iimage =0;iimage<GetNMeasuresPerLine()/4;iimage++){
			fscanf(fp,"%s %lf %lf %lf %lf\n",&sdump,&XL,&YL,&XR,&YR);
			if (PrintFlag>0) 
				printf("Data Bloc : Line %d Image %d -> Left=(%.2f,%.2f) Right=(%.2f,%.2f)    dX=%.2f  dY=%.2f dL=%.2f\n"
					,iline,iimage,XL,YL,XR,YR,(XR-XL),YR-YL,TMath::Sqrt((XR-XL)*(XR-XL)+(YR-YL)*(YR-YL)));
			SetFinalImage(iline,4*iimage,XL/1.e6);
			SetFinalImage(iline,4*iimage+1,YL/1.e6);
			SetFinalImage(iline,4*iimage+2,XR/1.e6);
			SetFinalImage(iline,4*iimage+3,YR/1.e6);
			SetImageDisplacement(iline,4*iimage,GetFinalImage(iline,4*iimage)-GetRefImage(iline,4*iimage));
			SetImageDisplacement(iline,4*iimage+1,GetFinalImage(iline,4*iimage+1)-GetRefImage(iline,4*iimage+1));
			SetImageDisplacement(iline,4*iimage+2,GetFinalImage(iline,4*iimage+2)-GetRefImage(iline,4*iimage+2));
			SetImageDisplacement(iline,4*iimage+3,GetFinalImage(iline,4*iimage+3)-GetRefImage(iline,4*iimage+3));
// 			printf("Diff : Line %d Image %d -> Left=(%.9f) Right=(%.9f)    \n"
// 					,iline,iimage,(GetFinalImage(iline,4*iimage+1)-GetRefImage(iline,4*iimage+1))/74.5e-3,(GetFinalImage(iline,4*iimage+3)-GetRefImage(iline,4*iimage+3))/74.5e-3);
		}
	}
	//CalcNewAngle();
}
  
void BlocTMS::CalcNewAngle(){
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
//				double Diff = GetImageDisplacement(i,4*iBCAM + 2*iLED +1);
//				printf("Line %d BCAM %d LED %d : F= %f    Diff = %.2f  DiffAngle = %.9f \n",i,iBCAM,iLED,F,Diff*1.e6,Diff/F);
//			}
//		}
//	}
}


/// Add the Intrinsic error of the measurement
void BlocTMS::AddIntrinsicError(TRandom3 *rand_obj,Double_t IntResFactor){
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

void BlocTMS::PrintFinalImages(){
	double XR, YR, XL, YL;
	for(int iline =0;iline<GetNLines();iline++){
				
				XL = 1.e6*GetFinalImage(iline,0);
				YL = 1.e6*GetFinalImage(iline,1);
				XR = 1.e6*GetFinalImage(iline,2);
				YR = 1.e6*GetFinalImage(iline,3);
				
 				printf("Final %s : Left=(%.2f,%.2f) Right=(%.2f,%.2f)    dX=%.2f  dY=%.2f dL=%.2f\n",GetImageName(iline).Data()
 								,XL,YL,XR,YR,(XR-XL),YR-YL,TMath::Sqrt((XR-XL)*(XR-XL)+(YR-YL)*(YR-YL)));
		}
}
	
void BlocTMS::PrintRefImages(){
	double XR, YR, XL, YL;
	for(int iline =0;iline<GetNLines();iline++){
				
				XL = 1.e6*GetRefImage(iline,0);
				YL = 1.e6*GetRefImage(iline,1);
				XR = 1.e6*GetRefImage(iline,2);
				YR = 1.e6*GetRefImage(iline,3);
				
 				printf("Ref %s : Left=(%.2f,%.2f) Right=(%.2f,%.2f)    dX=%.2f  dY=%.2f dL=%.2f\n",GetImageName(iline).Data()
 						,XL,YL,XR,YR,(XR-XL),YR-YL,TMath::Sqrt((XR-XL)*(XR-XL)+(YR-YL)*(YR-YL)));
		}
}
		
void BlocTMS::PrintDisplacements(){
	double XR, YR, XL, YL;
	for(int iline =0;iline<GetNLines();iline++){				
				XL = 1.e6*GetImageDisplacement(iline,0);
				YL = 1.e6*GetImageDisplacement(iline,1);
				XR = 1.e6*GetImageDisplacement(iline,2);
				YR = 1.e6*GetImageDisplacement(iline,3);
				
 				printf("Disp %s : Left=(%.2f,%.2f) Right=(%.2f,%.2f)    dX=%.2f  dY=%.2f dL=%.2f\n",GetImageName(iline).Data()
 						,XL,YL,XR,YR,(XR-XL),YR-YL,TMath::Sqrt((XR-XL)*(XR-XL)+(YR-YL)*(YR-YL)));
		}
}

