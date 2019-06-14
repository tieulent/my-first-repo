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
#ifndef CHAMBER_H
#define CHAMBER_H
#include <TObject.h>

#include "Plane.h"

/// \ingroup "GMSAnalyser"
/// \class Chamber
/// \brief Base class which describs a Chamber
///
//  Author Raphael Tieulent, IPN-Lyon

class Chamber : public Plane {

private:

	Int_t fID;   		///< Id of the chamber
	Plane *fPlatforms; 	///< Array of Platforms of the chamber
	Int_t fNPlatforms;  ///< Number of platform on that chamber

public:

	Chamber();
	Chamber(Int_t ID, const Double_t pos_th[3], const Double_t ang[3], const Double_t size[3], const Int_t *platforms, const Int_t NPlatforms);
	~Chamber();
	
	/// Return the platform "iPlatform" 
	inline Plane * GetPlatform(Int_t iPlatform)   {return ((iPlatform < fNPlatforms) ? (fPlatforms+iPlatform):0 );}; 
	/// Return the array of platforms
	inline Plane * GetPlatforms()  {return (fPlatforms);}; 
	/// Return the ID of the Chamber
	inline Int_t GetID(){return fID;}; 
	/// Set the number of platforms of the Chamber
	inline void SetNPlatforms(Int_t NPlatforms) {fNPlatforms = NPlatforms;};
	/// Get the number of platforms of the Chamber
	inline Int_t GetNPlatforms() {return fNPlatforms;};
	
	Plane * GetPlatformByID(Int_t PlatformID); 
	void LoadPlatformPosition(Int_t platformsID, Double_t *pos, Double_t *ang, Double_t *size);
	void CreateVolume(const Int_t  * platforms);


 	// ClassDef(Chamber,1)
};

#endif
