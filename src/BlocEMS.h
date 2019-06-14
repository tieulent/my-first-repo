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
#ifndef BLOCEMS_H
#define BLOCEMS_H

#include "Bloc.h"

class Wall;
class Chamber;
class Box;

/** \brief  Class describing a bloc of optical line which is part of the EMS (External monitoring system). 
	
This class describes the optical lines which link one chamber to the wall of the cavern. This monitoring is done using only the BCAM boxes.
	
© Copyright 2006 IPN Lyon - Raphael Tieulent. All Rights Reserved.

	\author Raphael Tieulent IPN Lyon
	\date 2006-10-08
	\version 1.0	\sa
**/


class BlocEMS : public Bloc
{
public:
	BlocEMS(const int nLines);
//	BlocEMS(const Int_t nb_lignes,
//		     Chamber *const ch,Box *const *const BCAMch,
//		     Wall *const wall,Box *const *const BCAMwall);
		virtual~BlocEMS();
/* affiche a l'ecran les adresses enregistrees dans les tableaux de boites*/
		virtual void Display() const; 
/* ATTENTION : rangement des mesures : les 3 premieres sont dx, dy et dd lorsque
la CCD est sur la chambre, et les 3 dernieres sont dx, dy et dd lorsque la CCD
est sur le mur */
virtual void CalcRefImages()	;
virtual void CalcTheoFinalImages();
virtual void CalcFinalImages()  ;
virtual void PrintFinalImages()  ;
virtual void PrintDisplacements()  ;
virtual void PrintRefImages()  ;
virtual void AddIntrinsicError(TRandom3 *rand_obj,Double_t IntResFactor);
virtual void ReadRefImages(FILE *fp , Int_t PrintFlag);
virtual void ReadFinalImages(FILE *fp , Int_t PrintFlag);
	
	void SetChamber(Chamber * ch){fCh = ch;};
	void SetWall(Wall * wall){fWall = wall;};

/// \todo a Refaire pour prendre en compte le fait que la chambre fixe est un seul demi plan et non plus une chambre entiere.


private:
	Chamber *fCh;
	Wall *fWall;
	//Int_t *numWallSup;
	
	//ClassDef(BlocEMS,1) 

};

#endif
