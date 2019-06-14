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

#include "Wall.h"
#include "op_mat.h"

//ClassImp(Wall)
 const Double_t Wall::DEP_ANG_MAX = 0.001;//!
 const Double_t Wall::DEP_POSXZ_MAX = 0.001;//!
 const Double_t Wall::DEP_POSY_MAX = 0.001;//!

/// The class Constructor
Wall::Wall(Int_t ID, const Double_t pos_th[3], const Double_t ang_th[3])
  {
   fID = ID;
   SetCenterTheo(pos_th);
   SetRotationAngles(ang_th);

    for(Int_t i=0;i<2;i++) {
		Double_t pos[3], ang[3];
		this->LoadPlatformGeometry(i,pos,ang);
		(this->GetPlatform(i))->SetCenterTheo(pos);
		(this->GetPlatform(i))->SetRotationAngles(ang);
    }

  }


/// destructor
Wall::~Wall()
  {
    if (nb_support != 0)
      {
    	delete [] fMatrixDeformation;
    	delete [] fMatrixDeformationTransposed;
    	delete [] fVectorDeformation;
      }
  }
void Wall::CreateVolume(){
}

/// Load the position and orientation of the platform iPlatform
void Wall::LoadPlatformGeometry(Int_t iPlatform, Double_t *pos, Double_t *ang)
  {
    FILE * fp;
    if(!(fp = fopen("data/Platforms.db","r"))){
	cout<<"Enable to open chamber database file data/Platforms.db"<<endl;
	exit(1);
    }
    Int_t PlatformID, ChamberID;
    char line[256];
    while ( fgets ( line, sizeof(line), fp ) != NULL )
    {
	switch ( line [0] ){
	    case '#': // Line commented
	        break;	
	    default:
		// Decode your values with a single scanf.
		sscanf ( line, " %d %lf %lf %lf %lf %lf %lf ", &PlatformID, &pos[0], &pos[1], &pos[2], &ang[0], &ang[1], &ang[2] );
		break;
	}	
	ChamberID = PlatformID/10;
	PlatformID = PlatformID%10;
	if((PlatformID == iPlatform)&&(ChamberID==fID)) {
	    fclose ( fp );	   
	    return;
	}    
    }	
    fclose ( fp );
    cout<<"Enable to find Position and Orientation of Platform "<<iPlatform<<" of Chamber "<<fID<<endl;
    exit(1);
  }


/// Declaration of the deformation Matrix
void Wall::SetSupport(const Int_t nb_support)
  {
    this->nb_support = nb_support;
    fMatrixDeformation = new Double_t[nb_support][3][3];
    fMatrixDeformationTransposed = new Double_t[nb_support][3][3];
    fVectorDeformation = new Double_t[nb_support][3];
    for(Int_t i=0;i<nb_support;i++)
      {
        for(Int_t j=0;j<3;j++)
	  {
	    for(Int_t k=0;k<3;k++)
	      {
	        fMatrixDeformation[i][j][k] = 0.;
		fMatrixDeformationTransposed[i][j][k] = 0.;
	      }
	    fVectorDeformation[i][j] = 0.;
	    fMatrixDeformation[i][j][j] = 1.;
	    fMatrixDeformationTransposed[i][j][j] = 1.;
	  }
      }
  }


/// Random initialization of the wall deformation at each position of the platforms
void Wall::SetRandomWallDeformation(TRandom3 *rand_obj)
  {
    Double_t ang;
    Double_t m[3][3][3];
    Double_t temp[3][3];
    for (Int_t i=0;i<nb_support;i++)
      {
        ang = rand_obj->Gaus(0.,DEP_ANG_MAX);
	/* matrices de rotation active d'angle phi_X */ 
	m[0][0][0] = 1;
	m[0][0][1] = 0;
	m[0][0][2] = 0;
	m[0][1][0] = 0;
	m[0][1][1] = TMath::Cos(ang);
	m[0][1][2] = -TMath::Sin(ang);
	m[0][2][0] = 0;
	m[0][2][1] = TMath::Sin(ang);
	m[0][2][2] = TMath::Cos(ang);
	
	ang = rand_obj->Gaus(0.,DEP_ANG_MAX);
	/* matrices de rotation active d'angle phi_Y */
	m[1][0][0] = TMath::Cos(ang);
	m[1][0][1] = 0;
	m[1][0][2] = TMath::Sin(ang);
	m[1][1][0] = 0;
	m[1][1][1] = 1;
	m[1][1][2] = 0;
	m[1][2][0] = -TMath::Sin(ang);
	m[1][2][1] = 0;
	m[1][2][2] = TMath::Cos(ang);
	
    	ang = rand_obj->Gaus(0.,DEP_ANG_MAX);
	/* matrices de rotation active d'angle phi_Z */
    	m[2][0][0] = TMath::Cos(ang);
    	m[2][0][1] = -TMath::Sin(ang);
    	m[2][0][2] = 0;
    	m[2][1][0] = TMath::Sin(ang);
    	m[2][1][1] = TMath::Cos(ang);
    	m[2][1][2] = 0;
    	m[2][2][0] = 0;
    	m[2][2][1] = 0;
    	m[2][2][2] = 1;
    	
    	/* [R(phi_X)R(phi_Y)R(phi_Z)] */
    	prod_mat(m[0],m[1],temp);
    	prod_mat(temp,m[2],fMatrixDeformation[i]);
    	
    	transp_mat(fMatrixDeformation[i],fMatrixDeformationTransposed[i]);
    	
    	/* vecteur de translation de la chambre i */
    	fVectorDeformation[i][0] = rand_obj->Gaus(0.,DEP_POSXZ_MAX);
    	fVectorDeformation[i][1] = rand_obj->Gaus(0.,DEP_POSY_MAX);
    	fVectorDeformation[i][2] = rand_obj->Gaus(0.,DEP_POSXZ_MAX);
      }
  }

