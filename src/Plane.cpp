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

#include <stdio.h>
#include <TClass.h>
#include <TMath.h>

#include "GMSAnalyser.h"
#include "Plane.h"

//ClassImp(Plane)

const Double_t Plane::MAXDEP = 0.0005;
 
Plane::Plane():TObject(),
fName(""),fPlaneID(0)
  {
  	Plane::Class()->IgnoreTObjectStreamer();
	for(int i=0;i<6;i++) 
		fDisplacementParameters[i] = fDisplacementParametersErrors[i] =fSimulatedDisplacementParameters[i] = 0.;
	
	fInitialPos = new TGeoHMatrix();
	fAliROOTDisplacementMatrix = NULL;
	fVolume = NULL;
	fVolCombiTrans = NULL;
	fVolRotation = NULL;
	fVolGlobal = NULL;
	fGlobalCombiTrans = NULL;
	fGlobalRotation = NULL;
  }


/* constructeur 1 */
Plane::Plane(const Double_t pos[3]):TObject(),
fName(""),fPlaneID(0)
  {
  	Plane::Class()->IgnoreTObjectStreamer();
	for(int i=0;i<6;i++) 
		fDisplacementParameters[i] = fDisplacementParametersErrors[i] = 0.;
	fInitialPos = new TGeoHMatrix();
	fAliROOTDisplacementMatrix = NULL;
	fVolume = NULL;
	fVolCombiTrans = NULL;
	fVolRotation = NULL;
	fVolGlobal = NULL;
	fGlobalCombiTrans = NULL;
	fGlobalRotation = NULL;

	SetCenter(pos);
	SetCenterTheo(pos);
  }


/* constructeur 2 */
Plane::Plane(const Double_t pos[3],const Double_t ang[3], const Double_t Size[3], const char * Name):TObject(),
fName(Name),fPlaneID(0)
  {
  	Plane::Class()->IgnoreTObjectStreamer();
	for(int i=0;i<6;i++) 
		fDisplacementParameters[i] = fDisplacementParametersErrors[i] = 0.;
 	fInitialPos = new TGeoHMatrix();
	fAliROOTDisplacementMatrix = NULL;
	fVolume = NULL;
	fVolCombiTrans = NULL;
	fVolRotation = NULL;
	fVolGlobal = NULL;
	fGlobalCombiTrans = NULL;
	fGlobalRotation = NULL;

	SetCenter(pos);
	SetCenterTheo(pos);	
	SetRotationAngles(ang);
	CalculateMatrixChgtGlobPlane(ang);
	SetVolumeSize(Size);

}

Plane::~Plane()  {
	//printf("Start Destruction of Plane %s at Add %X\n",this->fName.Data(), this); 
	//printf("fVolume address %X \n",fVolume); 
	//printf("fVolGlobal address %X  Nb of node %d\n",fVolGlobal
	//	,fVolGlobal->GetNtotal()); 
	if(fVolGlobal!=NULL) 
		for(int i=0;i<fVolGlobal->GetNtotal();i++) 
			fVolGlobal->RemoveNode(fVolGlobal->GetNode(i));
	if(fVolume!=NULL) {
		delete fVolume;
		//printf("fVolume deleted\n");
	}
	if(fGlobalCombiTrans!=NULL) {
		delete fGlobalCombiTrans;
		//printf("fGlobalCombiTrans deleted\n");
	}
	if(fGlobalRotation!=NULL) {
		delete fGlobalRotation;
		//printf("fGlobalRotation deleted\n");
	}
	if(fVolCombiTrans!=NULL) delete fVolCombiTrans;
	if(fVolRotation!=NULL) delete fVolRotation;
	
	if(fVolGlobal!=NULL) delete fVolGlobal;

	
	if(fInitialPos!=NULL) {
		delete fInitialPos;
		//printf("Fin Destruc Plane\n");
	}
}
//__________________________________________________________________

/// Translate the plane 
void Plane::Translate(const Double_t shift[3]){
	for(int i=0;i<3;i++) fCenter[i] = fCenterTheo[i] + shift[i];
}
//__________________________________________________________________

/// Set the size of the TGeoVolume
void Plane::SetVolumeSize(const Double_t size[3])
  {
    for(int i=0;i<3;i++) fVolSize[i] = size[i]/2.;
  }
//__________________________________________________________________

/// Set the Center of the plane
void Plane::SetCenter(const Double_t pos[3])
  {
    for(int i=0;i<3;i++) fCenter[i] = pos[i];
  }
//__________________________________________________________________

/// Set the theoritical center of the plane
void Plane::SetCenterTheo(const Double_t pos[3])
  {
    for(int i=0;i<3;i++) fCenterTheo[i] = pos[i];
  }
//__________________________________________________________________

/// Set the Rotation Angles of a given Plane
void Plane::SetRotationAngles(const Double_t * ang)
  {
    for(int i=0;i<3;i++) fAnglesGlobalPlane[i] = ang[i];
  }

//__________________________________________________________________
/// Calculate the matrix of displacement used by GMSAnalyser
void Plane::CalculateDisplacementMatrix(const Double_t param[6]){
	
	fMatDisplacement.SetToIdentity();

	fMatDisplacement.RotateZ(param[2]);
	fMatDisplacement.RotateY(param[1]);	
	fMatDisplacement.RotateX(param[0]);	

	fMatDisplacementInverse = fMatDisplacement.Inverse();	
	fVectorDisplacement.SetXYZ(param[3],param[4],param[5]);	
}

//__________________________________________________________________
/// Calculate the matrix of displacement used by GMSAnalyser
void Plane::CalculateDisplacementMatrix(){
	
	fMatDisplacement.SetToIdentity();

	fMatDisplacement.RotateZ(fDisplacementParameters[2]);
	fMatDisplacement.RotateY(fDisplacementParameters[1]);	
	fMatDisplacement.RotateX(fDisplacementParameters[0]);	

	fMatDisplacementInverse = fMatDisplacement.Inverse();	
	fVectorDisplacement.SetXYZ(fDisplacementParameters[3],fDisplacementParameters[4],fDisplacementParameters[5]);
	
}
//__________________________________________________________________
/// Fill in the Displacement matrix use for output to AliROOT

void Plane::FillDisplacementMatrix(){
	Double_t matrix[9];
	Double_t vect[3];
	matrix[0] = fMatDisplacement.XX();
	matrix[1] = fMatDisplacement.XY();
	matrix[2] = fMatDisplacement.XZ();
	matrix[3] = fMatDisplacement.YX();
	matrix[4] = fMatDisplacement.YY();
	matrix[5] = fMatDisplacement.YZ();
	matrix[6] = fMatDisplacement.ZX();
	matrix[7] = fMatDisplacement.ZY();
	matrix[8] = fMatDisplacement.ZZ();
	fAliROOTDisplacementMatrix->SetRotation(matrix);
	
	vect[0] = fVectorDisplacement.X();
	vect[1] = fVectorDisplacement.Y();
	vect[2] = fVectorDisplacement.Z();
	fAliROOTDisplacementMatrix->SetTranslation(vect);
}
//__________________________________________________________________
/// Write out the Displacement matrix use for output to AliROOT

void Plane::WriteDisplacementMatrix(){
	
	fAliROOTDisplacementMatrix->Print();
	PrintDisplacementMatrix();
    PrintDisplacementParameters();
//	fAliROOTDisplacementMatrix->Write(fName);
}

//__________________________________________________________________
/// Calculate the matrix of frame transformation 
void Plane::CalculateMatrixChgtGlobPlane(const Double_t ang[3])
  {
    SetRotationAngles(ang);
    fMatChgtGlobPlane.SetToIdentity();
	fMatChgtGlobPlane.RotateZ(ang[2]);
	fMatChgtGlobPlane.RotateY(ang[1]);	
 	fMatChgtGlobPlane.RotateX(ang[0]);   
	fMatChgtPlaneGlob = fMatChgtGlobPlane.Inverse();
	if(gGMSAnalyser->GetPrintLevel()>1){
		printf("\n ChgtGlobPlane Matrix of %s\n",fName.Data());
		printf("|%+e %+e %+e|\n",fMatChgtGlobPlane.XX(),fMatChgtGlobPlane.XY(),fMatChgtGlobPlane.XZ());
		printf("|%+e %+e %+e|\n",fMatChgtGlobPlane.YX(),fMatChgtGlobPlane.YY(),fMatChgtGlobPlane.YZ());
		printf("|%+e %+e %+e|\n", fMatChgtGlobPlane.ZX(),fMatChgtGlobPlane.ZY(),fMatChgtGlobPlane.ZZ());	
	}
  }
  
//__________________________________________________________________
/// Print to the standard output the Matrix of frame transformation 
void Plane::PrintMatChgtGlobPlane()
  {
	  if(gGMSAnalyser->GetPrintLevel()>1){
		printf("\nFrame transformation Global -> Plane Matrix of %s\n",fName.Data());
		printf("|%+e %+e %+e|\n",fMatChgtGlobPlane.XX(),fMatChgtGlobPlane.XY(),fMatChgtGlobPlane.XZ());
		printf("|%+e %+e %+e|\n",fMatChgtGlobPlane.YX(),fMatChgtGlobPlane.YY(),fMatChgtGlobPlane.YZ());
		printf("|%+e %+e %+e|\n", fMatChgtGlobPlane.ZX(),fMatChgtGlobPlane.ZY(),fMatChgtGlobPlane.ZZ());
	  }
  }

//__________________________________________________________________
/// Calculate the matrix of frame transformation 
void Plane::CalculateMatrixChgtGlobPlane_v2(const Double_t ang[3])
  {
	fInitialPos->SetRotation(kIdentityMatrix);
	fInitialPos->RotateZ(ang[2]*TMath::RadToDeg());
	fInitialPos->RotateY(ang[1]*TMath::RadToDeg());
	fInitialPos->RotateX(ang[0]*TMath::RadToDeg());
	fInitialPos->Print();
	
  }

//__________________________________________________________________
/// Print to the standard output the Matrix of displacement of the Plane
void Plane::PrintDisplacementMatrix()
  {
	printf("\nDisplacement Matrix of %s\n",fName.Data());
	printf("|%+22.16le %+22.16le %+22.16le|\n",fMatDisplacement.XX(),fMatDisplacement.XY(),fMatDisplacement.XZ());
	printf("|%+22.16le %+22.16le %+22.16le|\n",fMatDisplacement.YX(),fMatDisplacement.YY(),fMatDisplacement.YZ());
	printf("|%+22.16le %+22.16le %+22.16le|\n", fMatDisplacement.ZX(),fMatDisplacement.ZY(),fMatDisplacement.ZZ());	
  }

//__________________________________________________________________
/// Set the Displacement Parameters
void Plane::SetDisplacementParameters(const Double_t * param, Int_t start)
  {
	for(int i=0; i<6;i++) fDisplacementParameters[i] = param[i+start];
  }

//__________________________________________________________________
/// Set the Simulated Displacement Parameters
void Plane::SetSimulatedDisplacementParameters( const Double_t * param, Int_t start)
  {
	for(int i=0; i<6;i++) fSimulatedDisplacementParameters[i] = param[i+start];
  }
//__________________________________________________________________
/// Set the Displacement Parameters
void Plane::SetDisplacementParameters(const Double_t * param)
{
	for(int i=0; i<6;i++) fDisplacementParameters[i] = param[i];
}

//__________________________________________________________________
/// Set the Simulated Displacement Parameters
void Plane::SetSimulatedDisplacementParameters( const Double_t * param)
{
	for(int i=0; i<6;i++) fSimulatedDisplacementParameters[i] = param[i];
}

//__________________________________________________________________
/// Print to the standard output the 6 parameters of displacement of the Plane
void Plane::PrintDisplacementParameters()
  {
	if(gGMSAnalyser->GetPrintLevel()>1){
		printf("\nDisplacement Parameters of %s\n",fName.Data());
		printf("Theta X = %+e   Theta Y = %+e   Theta Z = %+e\n", 	
			fDisplacementParameters[0],fDisplacementParameters[1],fDisplacementParameters[2]);
		printf("      X = %+e         Y = %+e         Z = %+e\n", 	
			fDisplacementParameters[3],fDisplacementParameters[4],fDisplacementParameters[5]);
	}
  }

//__________________________________________________________________
/// Print to the standard output the 6 SIMULATED parameters of displacement of the Plane
void Plane::PrintSimulatedDisplacementParameters()
  {
		printf("\nSimulated Displacement Parameters of %s\n",fName.Data());
		printf("Theta X = %+e   Theta Y = %+e   Theta Z = %+e\n", 	
			fSimulatedDisplacementParameters[0],fSimulatedDisplacementParameters[1],fSimulatedDisplacementParameters[2]);
		printf("      X = %+e         Y = %+e         Z = %+e\n", 	
			fSimulatedDisplacementParameters[3],fSimulatedDisplacementParameters[4],fSimulatedDisplacementParameters[5]);
  }
