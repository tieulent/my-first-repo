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
#ifndef BLOCTMS_H
#define BLOCTMS_H

#include <TRandom3.h>
#include <Bloc.h>

class Chamber;
class Box;

/**
@author Philippe Pillot
*/

//!  Class describing a bloc of optical line which is part of the TMS (Transverse monitoring system). 
/*!
This class describes the optical lines which are on a given chamber to monitor the deformation of the support plans. This monitoring is done using only the BCAM boxes.
*/
class BlocTMS : public Bloc{

public:
	BlocTMS(const int nb_lignes);

BlocTMS(const int nb_lignes,
	Chamber *const ch,
	Box *const *const BCAM1,
	Box *const *const BCAM2);
virtual~BlocTMS();
/* affiche a l'ecran les adresses enregistrees dans les tableaux de boites*/
virtual void Display() const;
/* ATTENTION : rangement des mesures : les 3 premieres sont dx, dy et dd lorsque
la CCD est sur la chambre 1, et les 3 dernieres sont dx, dy et dd lorsque la CCD
est sur la chambre 2 */
virtual void CalcRefImages()	;
virtual void CalcTheoFinalImages();
virtual void CalcFinalImages()  ;
virtual void PrintFinalImages()  ;
virtual void PrintRefImages()  ;
virtual void PrintDisplacements()  ;
void PrintImagesDisplacement();
virtual void AddIntrinsicError(TRandom3 *rand_obj,double IntResFactor);
virtual void ReadRefImages(FILE *fp , int PrintFlag);
virtual void ReadFinalImages(FILE *fp , int PrintFlag);
virtual void CalcNewAngle();
Chamber * GetChamber() const{return fCh;};

private:
	Chamber * fCh;
	
	//ClassDef(BlocTMS,1) 

};
#endif
