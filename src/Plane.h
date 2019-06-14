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
#ifndef PLANE_H
#define PLANE_H
#include <TObject.h>
#include <TRandom3.h>
#include <TVector3.h>
#include <TRotation.h>
#include <TGeoVolume.h>
#include <TGeoMatrix.h>


/**
@author Raphael Tieulent
*/

//!  Base class which describs a Plane
/*!
*/
class Plane : public TObject{

private:

	TVector3  fAnglesGlobalPlane; //!< Angles of rotation between the Plane and its mother volume     	  
	TRotation fMatChgtGlobPlane;  //!< Matrix of frame transformation (Mother to Plane)  		     	  
	TRotation fMatChgtPlaneGlob;  //!< Matrix of frame transformation (Plane to Mother)  		     	  

protected:

	TString fName;						//!< Name of the Plane
	Int_t   fPlaneID;					//!< Id of the Plane
	
	TVector3 fVolSize; 					//!< Volume size

	TRotation fMatDisplacement;			//!< Matrix of Displacement
	TRotation fMatDisplacementInverse;	//!< Inverse of the Matrix of Displacement
	TVector3 fCenter;					//!< Center of the Plane
	TVector3 fCenterTheo;				//!< Theoritical center of the Plane
	TVector3 fVectorDisplacement;		//!< Vector of Displacement

	Double_t fDisplacementParameters[6];//!< Displacement parameters
	Double_t fDisplacementParametersErrors[6];//!< Errors on the Displacement parameters
	Double_t fSimulatedDisplacementParameters[6];

	TGeoHMatrix *fInitialPos;
	TGeoHMatrix *fAliROOTDisplacementMatrix; //!< Displacement matrix (output for AliROOT)
	

public:
	Plane();
	virtual ~Plane();
	Plane(const Double_t pos[3]);
	Plane(const Double_t pos[3],const Double_t ang[3], const Double_t Size[3], const char * Name);
	void SetCenter(const Double_t pos[3]);
	void SetCenterTheo(const Double_t pos[3]);
	void CalculateMatrixChgtGlobPlane(const Double_t ang[3]);
	void CalculateMatrixChgtGlobPlane_v2(const Double_t ang[3]);
	void Translate(const Double_t shift[3]);
	void CalculateDisplacementMatrix(const Double_t param[6]);
	void CalculateDisplacementMatrix();
	void FillDisplacementMatrix();
	void WriteDisplacementMatrix();

	void SetRotationAngles(const Double_t *ang);
	void SetVolumeSize(const Double_t size[3]);
	void PrintDisplacementMatrix();
	
	inline Int_t GetID(){return fPlaneID;};
	inline void SetID(Int_t ID){fPlaneID = ID;};
	
	inline void SetDisplacementParameter(Int_t ipar, Double_t val) { fDisplacementParameters[ipar] = val;};
	inline Double_t GetDisplacementParameter(Int_t ipar)const {return fDisplacementParameters[ipar];};
	inline void SetDisplacementParameterError(Int_t ipar, Double_t val) { fDisplacementParametersErrors[ipar] = val;};
	inline Double_t GetDisplacementParameterError(Int_t ipar)const {return fDisplacementParametersErrors[ipar];};
	void SetDisplacementParameters(const Double_t * param, Int_t start);
	void SetDisplacementParameters(const Double_t * param);
	inline const Double_t * GetDisplacementParameters()const {return fDisplacementParameters;};
	void PrintDisplacementParameters();
	void PrintMatChgtGlobPlane();
	
	inline Double_t GetSimulatedDisplacementParameter(Int_t ipar)const {return fSimulatedDisplacementParameters[ipar];};
	inline void SetSimulatedDisplacementParameter(Int_t ipar, Double_t val) { fSimulatedDisplacementParameters[ipar] = val;};
	void SetSimulatedDisplacementParameters(const Double_t * param, Int_t start);
	void SetSimulatedDisplacementParameters(const Double_t * param);
	inline const Double_t * GetSimulatedDisplacementParameters()const {return fSimulatedDisplacementParameters;};
	void PrintSimulatedDisplacementParameters();
	
	inline void SetName(TString name){fName = name;};
	inline const char * GetPlaneName()const {return fName.Data();};
	inline TRotation GetMatChgtPlaneGlob()const{return fMatChgtPlaneGlob;};///< Return the matrix of frame transformation from the Plane frame to the mother volume frame 
	inline TRotation GetMatChgtGlobPlane()const{return fMatChgtGlobPlane;};///< Return the matrix of frame transformation from the mother volume frame to the Plane frame 
	inline TVector3 GetDisplacementVector()const{return fVectorDisplacement;};///< Return the vector of displacement
	inline TRotation GetDisplacementMatrix()const{return fMatDisplacement;};///< Return the matrix of displacement 
	inline TRotation GetDisplacementMatrixTransposed()const{return fMatDisplacementInverse;}; ///< Return the Transposed matrix of displacement 
	inline TVector3 GetCenter()const{return fCenter;};///< Return the position of the center of the Plane
	inline TVector3 GetCenterTheo()const{return fCenterTheo;}; ///< Return the position of the theoritical center of the Plane
	inline Double_t GetCenterTheo(int i)const{return fCenterTheo[i];}; ///< Return the coordinate "i" of the position of the theoritical center of the Plane
	inline Double_t GetCenter(int i)const{return fCenter[i];}; ///< Return the coordinate "i" of the position of the center of the Plane 
	inline Double_t GetAngle(int i)const{return fAnglesGlobalPlane[i];}; ///< Return the angle of rotation of the Plane with respect to its mother volume
	inline Double_t GetVolSize(int i)const{return fVolSize[i];}; ///< Return the Volume size

	TGeoVolume *fVolume;//! 
	TGeoCombiTrans *fVolCombiTrans;//! 
	TGeoRotation *fVolRotation;//! 

	TGeoVolume *fVolGlobal;//! 
	TGeoCombiTrans *fGlobalCombiTrans;//! 
	TGeoRotation *fGlobalRotation;//! 


	static const Double_t MAXDEP; //!

 //	ClassDef(Plane,1) 


};

#endif
