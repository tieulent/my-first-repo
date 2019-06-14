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

#include "GMSImage.h"
#include "PROXImageResult.h"
#include "Bloc.h"
#include "BoxPROX.h"
#include "Chamber.h"
#include "BlocLMSPROX.h"

//ClassImp(BlocLMSPROX)

BlocLMSPROX::BlocLMSPROX(const int nLines) : Bloc(nLines,BoxPROX::NB_MES){
  	SetBlocType(Bloc::kBlocLMS);
}

//BlocLMSPROX::BlocLMSPROX(const Int_t nb_lignes,
//	    	  Chamber *const chMask,Box *const *const PROXMask,
//	    	  Chamber *const chCCD,Box *const *const PROXCCD)
//		 : Bloc(nb_lignes,BoxPROX::NB_MES)
//  {
//  	SetBlocType(Bloc::kBlocLMS);
//	fChCCD  = chCCD;
//	fChMask = chMask;
//    fLineNames = new TString[nb_lignes];
//	
//	fPROXCCD =new Box*[nb_lignes];
//	fPROXMask =new Box*[nb_lignes];
//	for(int i=0;i<nb_lignes;i++){
//		fPROXCCD[i] = PROXCCD[i];
//		fPROXMask[i] = PROXMask[i];
//		fLineNames[i] = Form("LMSCH%02dCH%02dP%d",PROXMask[i]->GetChamberID(),PROXCCD[i]->GetChamberID(),PROXCCD[i]->GetLocationID()/10);
//	}
//	
//    M0 = new Double_t[nb_lignes];
//    Mf = new Double_t[nb_lignes];
//    for(int i=0;i<nb_lignes;i++){
//		M0[i] = 1.;
//		Mf[i] = 1.;
//	}
//  }

BlocLMSPROX::~BlocLMSPROX()
{
//	delete [] fPROXCCD;
//	delete [] fPROXMask;
//	delete [] M0;
//	delete [] Mf;
}

/// Printout to stdout the addresses of the objects records in the tables fPROXCCD and fPROXMask 
void BlocLMSPROX::Display() const
  {
//    FILE *test_bloc = fopen("display_bloc.out","a");
//    fprintf(test_bloc,"display Bloc2LMSPROX :\n");
//    fprintf(test_bloc,"	boites :\n");
//    for(int i=0;i<fNLines;i++)
//        fprintf(test_bloc,"%x %x   %x %x\n",&(fPROXCCD[i]),fPROXCCD[i],&(fPROXMask[i]),fPROXMask[i]);
//    fprintf(test_bloc,"	chambres :\n");
//    fprintf(test_bloc,"%x %x\n",fChCCD,fChMask);
//    fclose(test_bloc);
  }
  
/// Read from data file the reference images of the Proximity Bloc
void BlocLMSPROX::ReadRefImages(FILE * fp, Int_t PrintFlag){
	char sdump[20];
	Double_t X, Y, MX, MY, Phi;		
	
	for(int iline =0;iline<GetNLines();iline++){

				fscanf(fp,"%s %lf %lf %lf %lf %lf\n",&sdump,&X,&Y,&MX,&MY,&Phi);
 				if (PrintFlag) printf("Img0 Prox  Line %d -> Pos=(%.2f,%.2f) Mag=(%.2f,%.2f) Rot=%.2f\n"
 									,iline,X, Y, MX, MY, Phi);
 				SetRefImage(iline,0,X/1.e6);
				SetRefImage(iline,1,Y/1.e6);
				SetRefImage(iline,2,MX);
				SetRefImage(iline,3,MY);
				SetRefImage(iline,4,Phi/1.e3);
		}

}
  
/// Read from data file the final images of the Proximity Bloc
void BlocLMSPROX::ReadFinalImages(FILE * fp, Int_t PrintFlag){
	char sdump[20];
	Double_t X, Y, MX, MY, Phi;		
	
	for(int iline =0;iline<GetNLines();iline++){

				fscanf(fp,"%s %lf %lf %lf %lf %lf\n",&sdump,&X,&Y,&MX,&MY,&Phi);
 				if (PrintFlag) printf("Data Prox  Line %d -> Pos=(%.2f,%.2f) Mag=(%.2f,%.2f) Rot=%.2f\n"
 									,iline,X, Y, MX, MY, Phi);
 				SetFinalImage(iline,0,X/1.e6);
				SetFinalImage(iline,1,Y/1.e6);
				SetFinalImage(iline,2,MX);
				SetFinalImage(iline,3,MY);
				SetFinalImage(iline,4,Phi/1.e3);
		}

}
  
  
/// calcul de la position (sur le masque) de l'image renvoyee par la CCD 
void BlocLMSPROX::CalcRefImages()
  {
    TVector3 n01 ;	// vecteur directeur de (OM1)
	n01[2] = 1.;
	
    TVector3 n02;	// vecteur directeur de (OM2)
	n02[1] = BoxPROX::size_CCD;
	n02[2] = BoxPROX::d_CCD_lens;
	n02 = n02.Unit();
	
			  
    TVector3 n;
    TVector3 OmOl; // vecteur O'O
    TVector3 OM2; // vecteur O'M2

    TVector3 pos_lentille;
    
	TArrayF refImage(5);
	  
    for(int i=0;i<fNLines;i++){
		GMSImage * currentImage =  GetImage(i);
		PROXImageResult * referenceResult = (PROXImageResult *) currentImage->GetReference();
		BoxPROX * objBox = (BoxPROX*)currentImage->GetObjectBox();
		BoxPROX * ccdBox = (BoxPROX*)currentImage->GetCCDBox();
		
	//=======================================================================================================	
	// Calcul du vecteur O'O : position du centre de la lentille dans le referentiel du masque
	//=======================================================================================================	
	// PROX CCD -> Platform CCD 
		pos_lentille[2] = ccdBox->GetLensZ();
		OmOl =  ccdBox->GetMatChgtBoxPlan() * pos_lentille;
		OmOl += ccdBox->GetCenter();
	
	// Platform CCD -> Chamber CCD
		OmOl *= (ccdBox->GetPlatform())->GetMatChgtPlaneGlob();
		OmOl += (ccdBox->GetPlatform())->GetCenter();

	// Chamber CCD -> Global
		OmOl *= (ccdBox->GetChamber())->GetMatChgtPlaneGlob();
		OmOl += (ccdBox->GetChamber())->GetCenter();
		
	// Global -> Chamber Mask
		OmOl -= (objBox->GetChamber())->GetCenter();
		OmOl *= (objBox->GetChamber())->GetMatChgtGlobPlane();
		
	// Chamber Mask -> Platform Mask
		OmOl -= (objBox->GetPlatform())->GetCenter();
		OmOl *= (objBox->GetPlatform())->GetMatChgtGlobPlane();
		
	// Platform Mask -> Mask
		OmOl -= objBox->GetCenter();
		OmOl *= objBox->GetMatChgtPlanBox() ;
		
	//=============================================================	
	// Calcul du vecteur directeur n01 dans le referentiel du masque 
	//=============================================================	
	
	// PROX CCD -> Platform CCD 
		n = ccdBox->GetMatChgtBoxPlan() * n01;
	// Platform CCD -> Chamber CCD	
		n *= (ccdBox->GetPlatform())->GetMatChgtPlaneGlob();
	// Chamber CCD -> Global	
		n *= (ccdBox->GetChamber())->GetMatChgtPlaneGlob();
	// Global -> Chamber Mask	
		n *= (objBox->GetChamber())->GetMatChgtGlobPlane();
	// Chamber Mask -> Platform Mask	
		n *= (objBox->GetPlatform())->GetMatChgtGlobPlane();
	// Platform Mask -> Mask
		n *= objBox->GetMatChgtPlanBox();

	//=============================================================				
	// Calcul de la position de M1 sur le masque (vecteur O'M1) 
	//=============================================================	
	
	refImage[0] = -(OmOl[2]*n[0]/n[2]) + OmOl[0];
	refImage[1] = -(OmOl[2]*n[1]/n[2]) + OmOl[1];
	
	//=============================================================	
	// Calcul du grossissement 
	//=============================================================	

	// M0[i] = -n[2]*BoxPROX::d_CCD_lens/OmOl[2];
	refImage[2] = -n[2]*BoxPROX::d_CCD_lens/OmOl[2];
	refImage[3] = -n[2]*BoxPROX::d_CCD_lens/OmOl[2];
	/// @todo Calculer Magnification en X et Y
	
	/* taille de l'image filmee sur le masque */
	//fRefImage[i][2] = BoxPROX::size_CCD/M0[i];
	
	
	//=============================================================	
	// Calcul du vecteur directeur n02 dans le referentiel du masque
	//=============================================================	

	// PROX CCD -> Platform CCD 
		n = ccdBox->GetMatChgtBoxPlan() * n02;
	// Platform CCD -> Chamber CCD	
		n *= ccdBox->GetPlatform()->GetMatChgtPlaneGlob();
	// Chamber CCD -> Global	
		n *= ccdBox->GetChamber()->GetMatChgtPlaneGlob();
	// Global -> Chamber Mask	
		n *= objBox->GetChamber()->GetMatChgtGlobPlane();
	// Chamber Mask -> Platform Mask	
		n *= objBox->GetPlatform()->GetMatChgtGlobPlane();
	// Platform Mask -> Mask
		n *= objBox->GetMatChgtPlanBox();	
		
	//=============================================================	
	// Calcul de la position de M2 sur le masque (vecteur O'M2)
	//=============================================================	
		
	OM2[0] = -(OmOl[2]*n[0]/n[2]) + OmOl[0];
	OM2[1] = -(OmOl[2]*n[1]/n[2]) + OmOl[1];
	
	
	//=============================================================	
	// Calcul de l'orientation de l'image sur le masque
	//=============================================================	

	refImage[4] = TMath::ATan((OM2[0]-refImage[0])/(OM2[1]-refImage[1]));
//	printf("X %.2f  Y %.2f MX %.5f   MY %.5f   Rot  %.2f  \n", 1e3*fRefImage[i][0],1e3*fRefImage[i][1],fRefImage[i][2],fRefImage[i][3],fRefImage[i][4]);

		// Record in the image object
		referenceResult->SetData(refImage);
	} // End Loop over the optical lines
    
  }


/// Calcul de la position (sur le masque) de l'image deplacee renvoyee par la
/// CCD, apres avoir deplacer les chambres a partir de leur position theorique.
void BlocLMSPROX::CalcTheoFinalImages()
  {
    TVector3 n01 ;	// vecteur directeur de (OM1)
	n01[2] = 1.;
	
    TVector3 n02;	// vecteur directeur de (OM2)
	n02[1] = BoxPROX::size_CCD / 
    			TMath::Sqrt(BoxPROX::size_CCD*BoxPROX::size_CCD + 
			     BoxPROX::d_CCD_lens*BoxPROX::d_CCD_lens);
	n02[2] = BoxPROX::d_CCD_lens / 
    		     TMath::Sqrt(BoxPROX::size_CCD*BoxPROX::size_CCD + 
		  	  BoxPROX::d_CCD_lens*BoxPROX::d_CCD_lens);
			  
    TVector3 n;
    TVector3 OmOl; // vecteur O'O
    TVector3 OM2; // vecteur O'M2

    TVector3 pos_lentille;
	    

    for(int i=0;i<fNLines;i++){
		
		GMSImage * currentImage =  GetImage(i);
		PROXImageResult * finalResult = (PROXImageResult *) currentImage->GetResult();
		BoxPROX * objBox = (BoxPROX*)currentImage->GetObjectBox();
		BoxPROX * ccdBox = (BoxPROX*)currentImage->GetCCDBox();
		
		TArrayF finalImage(5);
		
		//=======================================================================================================	
	// Calcul du vecteur O'O : position du centre de la lentille dans le referentiel du masque
	//=======================================================================================================	
	// PROX CCD -> Platform CCD Deplacee
		pos_lentille[2] = ccdBox->GetLensZ();
		OmOl = ccdBox->GetMatChgtBoxPlan() * pos_lentille;
		OmOl += ccdBox->GetCenter();
	
	// Platform CCD deplacee -> Platform CCD
		OmOl *= (ccdBox->GetPlatform())->GetDisplacementMatrix();
		OmOl += (ccdBox->GetPlatform())->GetDisplacementVector();
	
	// Platform CCD -> Chamber CCD deplacee
		OmOl *= (ccdBox->GetPlatform())->GetMatChgtPlaneGlob();
		OmOl += (ccdBox->GetPlatform())->GetCenter();
	
	// chambre CCD deplacee -> chambre CCD theorique deplacee
		OmOl *= (ccdBox->GetChamber())->GetMatChgtPlaneGlob();
		OmOl += (ccdBox->GetChamber())->GetCenter();
		OmOl -= (ccdBox->GetChamber())->GetCenterTheo();	
				
	// chambre CCD theorique deplacee -> chambre CCD theorique
		OmOl *= (ccdBox->GetChamber())->GetDisplacementMatrix();
		OmOl += (ccdBox->GetChamber())->GetDisplacementVector();	
	
	// Chamber CCD theorique -> Global
		OmOl += (ccdBox->GetChamber())->GetCenterTheo();
		
	//  Global -> Chamber Mask theorique
		OmOl -= (objBox->GetChamber())->GetCenterTheo();
		
	// Chamber Mask theorique -> Chamber Mask theorique deplacee
		OmOl -= (objBox->GetChamber())->GetDisplacementVector();
		OmOl *= (objBox->GetChamber())->GetDisplacementMatrixTransposed();
	
	// Chambre Mask theorique deplacee -> chambre Mask deplacee
		OmOl += (objBox->GetChamber())->GetCenterTheo();
		OmOl -= (objBox->GetChamber())->GetCenter();
		OmOl *= (objBox->GetChamber())->GetMatChgtGlobPlane();
		
	// Chambre Mask deplacee -> Platform Mask
		OmOl -= (objBox->GetPlatform())->GetCenter();
		OmOl *= (objBox->GetPlatform())->GetMatChgtGlobPlane();
	
	// Platform Mask -> Platform Mask deplacee
		OmOl -= (objBox->GetPlatform())->GetDisplacementVector();
		OmOl *= (objBox->GetPlatform())->GetDisplacementMatrixTransposed();
		
	// Platform Mask deplacee -> PROX Mask
		OmOl -= objBox->GetCenter();
		OmOl *= objBox->GetMatChgtPlanBox();
		
	//=============================================================	
	// Calcul du vecteur directeur n01 dans le referentiel du masque 
	//=============================================================	
	
	// PROX CCD -> Platform CCD Deplacee
		n = ccdBox->GetMatChgtBoxPlan() * n01;
	// Platform CCD deplacee -> Platform CCD
		n *= (ccdBox->GetPlatform())->GetDisplacementMatrix();
	// Platform CCD -> Chamber CCD	Deplacee
		n *= (ccdBox->GetPlatform())->GetMatChgtPlaneGlob();
	// Chamber CCD Deplacee -> Chamber CCD theorique deplacee
		n *= (ccdBox->GetChamber())->GetMatChgtPlaneGlob();
	// Chamber CCD theorique deplacee -> chambre CCD theorique
		n *= (ccdBox->GetChamber())->GetDisplacementMatrix();
	// Chamber Mask theorique -> Chamber Mask theorique deplacee
		n *= (objBox->GetChamber())->GetDisplacementMatrixTransposed();
	// Chambre Mask theorique deplacee -> chambre Mask deplacee					
		n *= (objBox->GetChamber())->GetMatChgtGlobPlane();
	// Chambre Mask deplacee -> Platform Mask
		n *= (objBox->GetPlatform())->GetMatChgtGlobPlane();
	// Platform Mask -> Platform Mask deplacee			
		n *= (objBox->GetPlatform())->GetDisplacementMatrixTransposed();
	// Platform Mask deplacee -> PROX Mask		
		n *= objBox->GetMatChgtPlanBox();								

	//=============================================================				
	// Calcul de la position de M1 sur le masque (vecteur O'M1) 
	//=============================================================	
	
		finalImage[0] = -(OmOl[2]*n[0]/n[2]) + OmOl[0];
		finalImage[1] = -(OmOl[2]*n[1]/n[2]) + OmOl[1];
	
	//=============================================================	
	// Calcul du grossissement 
	//=============================================================	
	
		//Mf[i] = -n[2]*BoxPROX::d_CCD_lens/OmOl[2];
		finalImage[2] = -n[2]*BoxPROX::d_CCD_lens/OmOl[2];
		finalImage[3] = -n[2]*BoxPROX::d_CCD_lens/OmOl[2];
	
	//=============================================================	
	// Calcul du vecteur directeur n02 dans le referentiel du masque 
	//=============================================================	
	
	// PROX CCD -> Platform CCD Deplacee
		n = ccdBox->GetMatChgtBoxPlan() * n02;
	// Platform CCD deplacee -> Platform CCD
		n *= (ccdBox->GetPlatform())->GetDisplacementMatrix();
	// Platform CCD -> Chamber CCD	Deplacee
		n *= (ccdBox->GetPlatform())->GetMatChgtPlaneGlob();
	// Chamber CCD Deplacee -> Chamber CCD theorique deplacee
		n *= (ccdBox->GetChamber())->GetMatChgtPlaneGlob();
	// Chamber CCD theorique deplacee -> chambre CCD theorique
		n *= (ccdBox->GetChamber())->GetDisplacementMatrix();
	// Chamber Mask theorique -> Chamber Mask theorique deplacee
		n *= (objBox->GetChamber())->GetDisplacementMatrixTransposed();
	// Chambre Mask theorique deplacee -> chambre Mask deplacee					
		n *= (objBox->GetChamber())->GetMatChgtGlobPlane();
	// Chambre Mask deplacee -> Platform Mask
		n *= (objBox->GetPlatform())->GetMatChgtGlobPlane();
	// Platform Mask -> Platform Mask deplacee			
		n *= (objBox->GetPlatform())->GetDisplacementMatrixTransposed();
	// Platform Mask deplacee -> PROX Mask		
		n *= objBox->GetMatChgtPlanBox();								

	//=============================================================				
	// Calcul de la position de M2 sur le masque (vecteur O'M2) 
	//=============================================================	
	
		OM2[0] = -(OmOl[2]*n[0]/n[2]) + OmOl[0];
		OM2[1] = -(OmOl[2]*n[1]/n[2]) + OmOl[1];
		
	//=============================================================	
	// Calcul de l'orientation de l'image sur le masque
	//=============================================================	

		finalImage[4] = TMath::ATan((OM2[0]-finalImage[0])/(OM2[1]-finalImage[1]));
	// Record in the image object
		finalResult->SetData(finalImage);
	
	// calcul Displacements
		currentImage->CalculateImageDisplacement();
	
	    
	//printf("X %.2f  Y %.2f MX %.2f   MY %.2f   Rot  %.2f  \n", fFinalImage[i][0],fFinalImage[i][1],fFinalImage[i][2],fFinalImage[i][3],fFinalImage[i][4]);
		
	/* test si les chambres ne se sont pas croisees */
		if (OmOl[2]<=0) {
	    	cout<<"des chambres se croisent (PROX) !!!"<<endl;
	/*	    fFinalImage[i][0] *= 1000.;
			fFinalImage[i][1] *= 1000.;
			fFinalImage[i][2] *= 1000.;
			fFinalImage[i][3] *= 1000.;
			fImageDisplacement[i][0] *= 1000.;
			fImageDisplacement[i][1] *= 1000.;
			fImageDisplacement[i][2] *= 1000.;
			fImageDisplacement[i][3] *= 1000.;
	*/	}
    } // End loop over optical lines
    
  }

/// Calcul de la position (sur le masque) de l'image deplacee renvoyee par la
/// CCD, apres avoir deplacer les chambres a partir de leur position reelle ou
/// mesuree. 
void BlocLMSPROX::CalcFinalImages()
  {
   	TVector3 n01 ;	// vecteur directeur de (OM1)
	n01[2] = 1.;
	
    TVector3 n02;	// vecteur directeur de (OM2)
	n02[1] = Box::CCDWidth / 
    			TMath::Sqrt(Box::CCDWidth*Box::CCDWidth + 
			     BoxPROX::d_CCD_lens*BoxPROX::d_CCD_lens);
	n02[2] = BoxPROX::d_CCD_lens / 
    		     TMath::Sqrt(Box::CCDWidth*Box::CCDWidth + 
		  	  BoxPROX::d_CCD_lens*BoxPROX::d_CCD_lens);
			  
    TVector3 n;
    TVector3 OmOl; // vecteur O'O
    TVector3 OM2; // vecteur O'M2

    TVector3 pos_lentille;
  
    for(int i=0;i<fNLines;i++){
		
		GMSImage * currentImage =  GetImage(i);
		PROXImageResult * finalResult = (PROXImageResult *) currentImage->GetResult();
		BoxPROX * objBox = (BoxPROX*)currentImage->GetObjectBox();
		BoxPROX * ccdBox = (BoxPROX*)currentImage->GetCCDBox();
		
		TArrayF finalImage(5);
		
	//=======================================================================================================	
	// Calcul du vecteur O'O : position du centre de la lentille dans le referentiel du masque
	//=======================================================================================================	
	// PROX CCD -> Platform CCD Deplacee
		pos_lentille[2] = ccdBox->GetLensZ();
		OmOl = ccdBox->GetMatChgtBoxPlan() * pos_lentille;
		OmOl += ccdBox->GetCenter();
	
	// Platform CCD deplacee -> Platform CCD
		OmOl *= (ccdBox->GetPlatform())->GetDisplacementMatrix();
		OmOl += (ccdBox->GetPlatform())->GetDisplacementVector();
	
	// Platform CCD -> Chamber CCD deplacee
		OmOl *= (ccdBox->GetPlatform())->GetMatChgtPlaneGlob();
		OmOl += (ccdBox->GetPlatform())->GetCenter();
	
	// chambre CCD deplacee -> chambre CCD 
		OmOl *= (ccdBox->GetChamber())->GetDisplacementMatrix();
		OmOl += (ccdBox->GetChamber())->GetDisplacementVector();	
	
	// Chamber CCD -> Global
		OmOl *= (ccdBox->GetChamber())->GetMatChgtPlaneGlob();
		OmOl += (ccdBox->GetChamber())->GetCenter();		
		
	//  Global -> Chamber Mask 
		OmOl -= (objBox->GetChamber())->GetCenter();
		OmOl *= (objBox->GetChamber())->GetMatChgtGlobPlane();
		
	// Chamber Mask -> Chamber Mask deplacee
		OmOl -= (objBox->GetChamber())->GetDisplacementVector();
		OmOl *= (objBox->GetChamber())->GetDisplacementMatrixTransposed();
			
	// Chambre Mask deplacee -> Platform Mask
		OmOl -= (objBox->GetPlatform())->GetCenter();
		OmOl *= (objBox->GetPlatform())->GetMatChgtGlobPlane();
	
	// Platform Mask -> Platform Mask deplacee
		OmOl -= (objBox->GetPlatform())->GetDisplacementVector();
		OmOl *= (objBox->GetPlatform())->GetDisplacementMatrixTransposed();
		
	// Platform Mask deplacee -> PROX Mask
		OmOl -= objBox->GetCenter();
		OmOl *= objBox->GetMatChgtPlanBox();
	
	//=============================================================	
	// Calcul du vecteur directeur n01 dans le referentiel du masque 
	//=============================================================		

	// PROX CCD -> Platform CCD Deplacee
		n = ccdBox->GetMatChgtBoxPlan() * n01;
	// Platform CCD deplacee -> Platform CCD
		n *= (ccdBox->GetPlatform())->GetDisplacementMatrix() ;
	// Platform CCD -> Chamber CCD	Deplacee
		n *= (ccdBox->GetPlatform())->GetMatChgtPlaneGlob() ;
	// Chamber CCD Deplacee -> Chamber CCD  
		n *= (ccdBox->GetChamber())->GetDisplacementMatrix() ;
	// Chamber CCD -> Global
		n *= (ccdBox->GetChamber())->GetMatChgtPlaneGlob() ;
	//  Global -> Chamber Mask 
		n *= (objBox->GetChamber())->GetMatChgtGlobPlane() ;
	// Chamber Mask -> Chamber Mask deplacee
		n *= (objBox->GetChamber())->GetDisplacementMatrixTransposed() ;
	// Chambre Mask deplacee -> Platform Mask
		n *= (objBox->GetPlatform())->GetMatChgtGlobPlane() ;
	// Platform Mask -> Platform Mask deplacee
		n *= (objBox->GetPlatform())->GetDisplacementMatrixTransposed() ;
	// Platform Mask deplacee -> PROX Mask
		n *= objBox->GetMatChgtPlanBox() ;																
	
	//=============================================================				
	// Calcul de la position de M1 sur le masque (vecteur O'M1) 
	//=============================================================	

		finalImage[0] = -(OmOl[2]*n[0]/n[2]) + OmOl[0];
		finalImage[1] = -(OmOl[2]*n[1]/n[2]) + OmOl[1];
	
	//=============================================================	
	// Calcul du grossissement 
	//=============================================================	

		//Mf[i] = -n[2]*BoxPROX::d_CCD_lens/OmOl[2];
		finalImage[2] = -n[2]*BoxPROX::d_CCD_lens/OmOl[2];
		finalImage[3] = -n[2]*BoxPROX::d_CCD_lens/OmOl[2];
	
	//=============================================================	
	// Calcul du vecteur directeur n02 dans le referentiel du masque 
	//=============================================================		

	// PROX CCD -> Platform CCD Deplacee
		n = ccdBox->GetMatChgtBoxPlan() * n02;
	// Platform CCD deplacee -> Platform CCD
		n *= (ccdBox->GetPlatform())->GetDisplacementMatrix() ;
	// Platform CCD -> Chamber CCD	Deplacee
		n *= (ccdBox->GetPlatform())->GetMatChgtPlaneGlob() ;
	// Chamber CCD Deplacee -> Chamber CCD  
		n *= (ccdBox->GetChamber())->GetDisplacementMatrix() ;
	// Chamber CCD -> Global
		n *= (ccdBox->GetChamber())->GetMatChgtPlaneGlob() ;
	//  Global -> Chamber Mask 
		n *= (objBox->GetChamber())->GetMatChgtGlobPlane() ;
	// Chamber Mask -> Chamber Mask deplacee
		n *= (objBox->GetChamber())->GetDisplacementMatrixTransposed() ;
	// Chambre Mask deplacee -> Platform Mask
		n *= (objBox->GetPlatform())->GetMatChgtGlobPlane() ;
	// Platform Mask -> Platform Mask deplacee
		n *= (objBox->GetPlatform())->GetDisplacementMatrixTransposed() ;
	// Platform Mask deplacee -> PROX Mask
		n *= objBox->GetMatChgtPlanBox() ;																
	
	//=============================================================				
	// Calcul de la position de M2 sur le masque (vecteur O'M2) 
	//=============================================================	
	
		OM2[0] = -(OmOl[2]*n[0]/n[2]) + OmOl[0];
		OM2[1] = -(OmOl[2]*n[1]/n[2]) + OmOl[1];
	
	//=============================================================	
	// Calcul de l'orientation de l'image sur le masque
	//=============================================================	
	
		finalImage[4] = TMath::ATan((OM2[0]-finalImage[0])/(OM2[1]-finalImage[1]));
		
	// Record in the image object
		finalResult->SetData(finalImage);
		
	// calcul Displacements
		currentImage->CalculateImageDisplacement();
		

//	printf("X %.2f  Y %.2f MX %.5f   MY %.5f   Rot  %.2e  \n", 1e3*fRefImage[i][0],1e3*fRefImage[i][1],fRefImage[i][2],fRefImage[i][3],fRefImage[i][4]);
//	printf("X %.2f  Y %.2f MX %.5f   MY %.5f   Rot  %.2e  \n", 1e3*fFinalImage[i][0],1e3*fFinalImage[i][1],fFinalImage[i][2],fFinalImage[i][3],fFinalImage[i][4]);
//	printf("Disp X %.2f  Y %.5f MX %.5f   MY %.5f   Rot  %.2e  \n", 1e6*fImageDisplacement[i][0],1e6*fImageDisplacement[i][1],fImageDisplacement[i][2],fImageDisplacement[i][3],fImageDisplacement[i][4]);

		/* test si les chambres ne se sont pas croisees */
		if (OmOl[2]<=0) {
			cout<<"des chambres se croisent (PROX) !!!"<<endl;
// 		    fFinalImage[i][0] *= 1000.;
// 			fFinalImage[i][1] *= 1000.;
// 			fFinalImage[i][2] *= 1000.;
// 			fFinalImage[i][3] *= 1000.;
// 			fFinalImage[i][4] *= 1000.;
// 			fImageDisplacement[i][0] *= 1000.;
// 			fImageDisplacement[i][1] *= 1000.;
// 			fImageDisplacement[i][2] *= 1000.;
// 			fImageDisplacement[i][3] *= 1000.;
// 			fImageDisplacement[i][4] *= 1000.;
		}
	} // End loop over optical lines
    
  }


///  Add the Intrinsic error of the measurement
void BlocLMSPROX::AddIntrinsicError(TRandom3 *rand_obj,Double_t IntResFactor)
  {
    for (int i=0;i<fNLines;i++) {
		GMSImage * currentImage =  GetImage(i);
		PROXImageResult * referenceResult = (PROXImageResult *) currentImage->GetReference();
		PROXImageResult * finalResult = (PROXImageResult *) currentImage->GetResult();
		  
		referenceResult->SetXpos(rand_obj->Gaus(referenceResult->GetXpos(),IntResFactor*BoxPROX::SIGMA_X_PROX));
		referenceResult->SetYpos(rand_obj->Gaus(referenceResult->GetYpos(),IntResFactor*BoxPROX::SIGMA_Y_PROX));
		referenceResult->SetXmag(rand_obj->Gaus(referenceResult->GetXmag(),IntResFactor*BoxPROX::SIGMA_M_PROX));
		referenceResult->SetYmag(rand_obj->Gaus(referenceResult->GetYmag(),IntResFactor*BoxPROX::SIGMA_M_PROX));
		referenceResult->SetRotation(rand_obj->Gaus(referenceResult->GetRotation(),IntResFactor*BoxPROX::SIGMA_A_PROX));
		
		finalResult->SetXpos(rand_obj->Gaus(finalResult->GetXpos(),IntResFactor*BoxPROX::SIGMA_X_PROX));
		finalResult->SetYpos(rand_obj->Gaus(finalResult->GetYpos(),IntResFactor*BoxPROX::SIGMA_Y_PROX));
		finalResult->SetXmag(rand_obj->Gaus(finalResult->GetXmag(),IntResFactor*BoxPROX::SIGMA_M_PROX));
		finalResult->SetYmag(rand_obj->Gaus(finalResult->GetYmag(),IntResFactor*BoxPROX::SIGMA_M_PROX));
		finalResult->SetRotation(rand_obj->Gaus(finalResult->GetRotation(),IntResFactor*BoxPROX::SIGMA_A_PROX));

		// calcul Displacements
		currentImage->CalculateImageDisplacement();

      }
  }
void BlocLMSPROX::PrintFinalImages(){
	double X, Y, MX, MY,Phi;
	for(int iline =0;iline<GetNLines();iline++){
				
				X = 1.e6*GetFinalImage(iline,0);
				Y = 1.e6*GetFinalImage(iline,1);
				MX = GetFinalImage(iline,2);
				MY = GetFinalImage(iline,3);
				Phi = 1.e3*GetFinalImage(iline,4);
				
 				printf("Final %s : Pos=(%.2f,%.2f) Mag=(%.2f,%.2f)  Phi=%.2f  \n",GetImageName(iline).Data()
 								,X,Y,MX,MY,Phi);
		}
	
}
void BlocLMSPROX::PrintDisplacements(){
	double X, Y, MX, MY,Phi;
	for(int iline =0;iline<GetNLines();iline++){				
		X = 1.e6*GetImageDisplacement(iline,0);
		Y = 1.e6*GetImageDisplacement(iline,1);
		MX = GetImageDisplacement(iline,2);
		MY = GetImageDisplacement(iline,3);
		Phi = 1.e3*GetImageDisplacement(iline,4);
 				printf("Disp %s : Pos=(%.2f,%.2f) Mag=(%.1e,%.1e)  Phi=%.1e \n",GetImageName(iline).Data()
 						,X,Y,MX,MY,Phi);
		}
}


void BlocLMSPROX::PrintRefImages(){}
