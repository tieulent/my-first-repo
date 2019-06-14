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
#include "Box.h"
#include <TRandom3.h>
#include <TVector3.h>
#include <TRotation.h>
#include "GMSAnalyser.h"
#include "AliLog.h"

//ClassImp(Box)

const double Box::CCDWidth  = 3.440e-3; 
const double Box::CCDHeight = 2.440e-3; 

Box::~Box()
{
//  	delete fBoxRotation;
//	delete fBoxCombiTrans;
	delete fOpticalAxis;
	delete fBoxVolume;

}

Box::Box(int ChID, int PlatformID, int LocationID, const char * BCAMID )
:TObject(), fChID(ChID),fPlatformID(PlatformID),fLocationID(LocationID),fID(BCAMID)
{
	AliDebug(2,Form("Creating Box %s  on Ch %d on Plat %02d at Loc %d ",fID.Data(),fChID,fPlatformID,fLocationID));

	LoadPositionOrientation();
	ComputeTransformationMatrix();
}

/// Set the Angle i of the box
void Box::SetAngle(int i, const double ang)
  {
    fAngles[i] = ang;
  }

/// Set the Angles of the box
void Box::SetAngles(const double ang[3])
  {
    for(int i=0;i<3;i++) fAngles[i] = ang[i];
  }

/* saisie du centre de la boite */
void Box::SetCenter(const double pos[3])
  {
    for(int i=0;i<3;i++) fCenter[i] = pos[i];
  }

/// Compute the transformation Matrixes between the reference frame of the platform and the one of the box   
void Box::ComputeTransformationMatrix(){
	fMatChgtPlaneBox.SetToIdentity();
 	fMatChgtPlaneBox.RotateX(-fAngles(2));  
	fMatChgtPlaneBox.RotateY(-fAngles(1));  
	fMatChgtPlaneBox.RotateZ(-fAngles(0));
	fMatChgtBoxPlane = fMatChgtPlaneBox.Inverse();
	AliDebug(2,"--------------------- Mat Plane Box --------------------------");
	AliDebug(2,Form("%.4e %.4e %.4e ",fMatChgtPlaneBox.XX(),fMatChgtPlaneBox.XY(),fMatChgtPlaneBox.XZ()));
	AliDebug(2,Form("%.4e %.4e %.4e ",fMatChgtPlaneBox.YX(),fMatChgtPlaneBox.YY(),fMatChgtPlaneBox.YZ()));
	AliDebug(2,Form("%.4e %.4e %.4e ",fMatChgtPlaneBox.ZX(),fMatChgtPlaneBox.ZY(),fMatChgtPlaneBox.ZZ()));
	AliDebug(2,"-------------------------------------------------------------");
}

void Box::PrintMatChgtPlaneBox(){
	AliInfo("-------------- Mat Plane Box -----------------------");
	AliInfo(Form("%.4f %.4f %.4f ",fMatChgtPlaneBox.XX(),fMatChgtPlaneBox.XY(),fMatChgtPlaneBox.XZ()));
	AliInfo(Form("%.4f %.4f %.4f ",fMatChgtPlaneBox.YX(),fMatChgtPlaneBox.YY(),fMatChgtPlaneBox.YZ()));
	AliInfo(Form("%.4f %.4f %.4f ",fMatChgtPlaneBox.ZX(),fMatChgtPlaneBox.ZY(),fMatChgtPlaneBox.ZZ()));
	AliInfo("----------------------------------------------------");
}

/// Load the position and orientation of the Emplacement platform iPlatform
void Box::LoadPositionOrientation()
  {
    FILE * fp;
	TString PlatformsLocationFileName(gGMSAnalyser->GetGMSAnalyserRootDir());
	PlatformsLocationFileName += "/data/CurrentGeom/PlatformsLocations.db";
    if(!(fp = fopen(PlatformsLocationFileName.Data(),"r")))
		AliFatal(Form("Enable to open platform Location database file %s",PlatformsLocationFileName.Data()));
	double pos[3], ang[3];
    char line[256];
	int ChamberID,  PlatformID,  LocationID;
    while ( fgets ( line, sizeof(line), fp ) != NULL ){
		switch ( line [0] ){
			case '#': // Line commented
				break;	
			default:
			// Decode your values with a single scanf.
			sscanf ( line, " %d %d %d %lf %lf %lf %lf %lf %lf ", &ChamberID,&PlatformID, &LocationID
					, &fCenter[0], &fCenter[1], &fCenter[2], &fAngles[0], &fAngles[1], &fAngles[2] );
			if((PlatformID == fPlatformID)
				&&(ChamberID==fChID)
				&&(LocationID == fLocationID)) {
				fclose ( fp );	
				for(int i = 0; i < 3; i++) {
					fAngles[i] *= -1.;// Because the rotation definition of Vanik is Opposite
					fCenter[i] *= -1.;// Because Vanik gave me (Global to Local) and not (Local to Global)
				}
				AliDebug(2,Form("Find position and orientation of box %s on Platform %d of Chamber %d at Loc %d",fID.Data(),fPlatformID,fChID,fLocationID)); 
				AliDebug(2,Form("X = %.3f  Y = %.3f  Z = %.3f",fCenter[0],fCenter[1],fCenter[2]));  
				AliDebug(2,Form("thZ = %.3f  thY = %.3f  thX = %.3f",fAngles[0],fAngles[1],fAngles[2]));  
				return;
			}    
			break;
		}	

    }	
    fclose ( fp );
    AliError(Form("Enable to find Position and Orientation of Box %s on Platform %d of Chamber %d at Location %d",fID.Data(),fPlatformID,fChID,fLocationID));

  }


