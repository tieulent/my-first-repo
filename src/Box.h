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
#ifndef BOX_H
#define BOX_H

#include <TObject.h>
#include <TRandom3.h>
#include <TVector3.h>
#include <TRotation.h>
#include <TGeoVolume.h>
#include <TGeoMatrix.h>
#include <TString.h>

class Chamber;
class Plane;
class Wall;

//!  Base class describing an optical box (BCAM or PROX). 
/*!
  A Box is an optical apparatus which can be a BCAM box, a PROX tube or a PROX mask.
  This class box describes the basic attributs and methods of the optical elements. 
*/
class Box : public TObject{

public:
	static const double CCDWidth  ; ///< unit is meter
	static const double CCDHeight ; ///< unit is meter

//	Box(int ChID, int PlatformID, int LocationID, int BCAMID );
	Box(int ChID, int PlatformID, int LocationID, const char * BCAMID );
	virtual ~Box();

	virtual void random_box(TRandom3 *rand_obj,const Box* boite_th) = 0;
	virtual void CreateVolume(Chamber * Ch, int Color) = 0;
	virtual void CreateVolume(Wall * wall, int Color) = 0;
	virtual void LoadCalibrationParameters() =0;
	void ComputeTransformationMatrix(); 

	inline TVector3 GetCenter() const {return fCenter;};
	inline double 	GetCenter(int i)const {return fCenter(i);};
	inline TVector3 GetAngles() const {return fAngles;};
	inline double 	GetAngle(int i) const {return fAngles(i);};
	inline TRotation GetMatChgtPlanBox() const {return fMatChgtPlaneBox;};
	inline TRotation GetMatChgtBoxPlan() const {return fMatChgtBoxPlane;};
//	inline int GetID() const {return fID;};
	inline const char * GetID() {return fID.Data();};
	inline int GetChamberID() const {return fChID;};
	inline int GetPlatformID() const {return fPlatformID;};
	inline int GetLocationID() const {return fLocationID;};

	void SetAngles(const double ang[3]);
	void SetAngle(int i, const double ang);
	void SetCenter(const double pos[3]);
	void PrintMatChgtPlaneBox();

	Chamber * GetChamber(){return fCh;};
	Plane * GetPlatform(){return fPlatform;};
	void SetChamber(Chamber * ch){fCh = ch;};
	void SetPlatform(Plane * plt){fPlatform = plt;};
	
	
protected:
	TVector3  fVolSize;		///< Size dX, dY, dZ of the TGeoVolume
	TVector3  fCenter; 		///< Position of the Box in the frame of the platform
	TVector3  fAngles;		///< Orientation of the Box in the Frame of the platform

	TRotation  fMatChgtPlaneBox; ///< Matrix of frame transformation to go from the platform frame to the box frame 
	TRotation  fMatChgtBoxPlane; ///< Matrix of frame transformation to go from the box frame  to the platform frame
	TRotation  fMatCalibBox; 	 ///< Matrix of frame transformation to apply the Calibration constant of the Box (Change the optical axis)
	TGeoVolume 		*fBoxVolume;
	TGeoVolume 		*fOpticalAxis;
	TGeoCombiTrans 	*fBoxCombiTrans;
	TGeoRotation 	*fBoxRotation;


//	int fID; 			///< Identification number of the box (used for the calibration constants)
	TString fID; 			///< Identification number of the box (used for the calibration constants)
	int fChID;			///< Identification of the chamber on which sits the box
	int fPlatformID;	///< Identification of the Platform on which sits the box
	int fLocationID;	///< Place on the platform (define its position and orientation)

	TString fName;		///< Name of the TGeoVolume
	
	Chamber * fCh;
	Plane * fPlatform;
	
	
	void LoadPositionOrientation();
	
//	ClassDef(Box,1) 

};

#endif
