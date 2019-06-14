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
#ifndef Wall_H
#define Wall_H
#include <TObject.h>

#include <TRandom3.h>
#include <TVector3.h>
#include <TRotation.h>
#include "Plane.h"


/**
@author Raphael Tieulent
*/

//!  Base class which describs a Wall
/*!
*/
class Wall : public Plane{

private:
	/* deformation du Wall localise sur la position reelle du BCAM i */
	static const Double_t DEP_ANG_MAX ;//!
	static const Double_t DEP_POSXZ_MAX ;//!
	static const Double_t DEP_POSY_MAX ;//!
	Int_t nb_support; //!< Nb of Supports
	Double_t (*fMatrixDeformation)[3][3]; //!< Matrix of deformation
	Double_t (*fMatrixDeformationTransposed)[3][3]; //!< Matrix of deformation Transposed
	Double_t (*fVectorDeformation)[3]; //!< Vector of deformation

	// TRotation fMatrixDeformation; //!< Matrix of deformation
	// TRotation fMatrixDeformationTransposed; //!< Matrix of deformation Transposed
	// TVector3  fVectorDeformation; //!< Vector of deformation

	Int_t fID;   //!< Id of the Wall
	Plane fPlatforms[2];//! Array of platforms

public:

	Wall(Int_t ID, const Double_t pos_th[3], const Double_t ang[3]);
	~Wall();
	
	void SetSupport(const Int_t nb_support);
	void SetRandomWallDeformation(TRandom3 *rand_obj);

	inline  Plane * GetPlatform(Int_t iPlatform)  {return ((iPlatform < 2) ? (fPlatforms+iPlatform):0 );}; //!< Returns a pointer to Platform iPlatform
	inline  Plane * GetPlatforms()  {return (fPlatforms);}; //!< Returns a pointer to the table of Platforms
	void LoadPlatformGeometry(Int_t iPlatform,Double_t *pos, Double_t *ang);
	inline Int_t GetID() const {return fID;}; //!< Returns the ID of the Wall
	void CreateVolume();

//	ClassDef(Wall,1)

};


#endif
