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
#ifndef BLOCLMSPROX_H
#define BLOCLMSPROX_H

#include "Bloc.h"


/**
@author Raphael Tieulent
*/
class BlocLMSPROX : public Bloc
{
public:
/* ATTENTION a l'ordre des elements : CCD puis Masque */
	BlocLMSPROX(const int nb_lignes);
	BlocLMSPROX(const int nb_lignes,
	      Chamber *const chMask,Box *const *const PROXMask,
	      Chamber *const chCCD,Box *const *const PROXCCD);
    ~BlocLMSPROX();
/* affiche a l'ecran les adresses enregistrees dans les tableaux de boites*/
	virtual void Display() const;
/* ATTENTION : rangement des mesures : dx, dy, dt puis da */
	virtual void CalcRefImages()	;
	virtual void CalcTheoFinalImages();
	virtual void CalcFinalImages()  ;
	virtual void PrintFinalImages()  ;
	virtual void PrintRefImages()  ;
	virtual void PrintDisplacements()  ;
	virtual void AddIntrinsicError(TRandom3 *rand_obj,double IntResFactor);
	virtual void ReadRefImages(FILE *fp , int PrintFlag);
	virtual void ReadFinalImages(FILE *fp , int PrintFlag);
	
	
private:
//	Chamber *fChCCD;
//	Chamber *fChMask;
//	Box **fPROXCCD;
//	Box **fPROXMask;
///* utile pour faire les calculs d'incertitude */
//double *M0;
//double *Mf;
	
//	ClassDef(BlocLMSPROX,1) 

};

#endif
