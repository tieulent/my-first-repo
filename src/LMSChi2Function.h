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
#ifndef LMSCHI2FUNCTION_H
#define LMSCHI2FUNCTION_H


/// \ingroup "Geometry Monitoring System"
/// \class LMSChi2Function
/// \brief Class describing the Chi2 function used to minimize the LMS part of the system.
///
//  Raphael Tieulent, Institut de Physique Nucleaire de Lyon

#include <Math/FitMethodFunction.h>


class LMSChi2Function : public ROOT::Math::FitMethodFunction { 
	
public:
	
	// needed for AClic
	typedef ROOT::Math::BasicFitMethodFunction<ROOT::Math::IMultiGenFunction>::Type_t  Type_t;
	
	LMSChi2Function(int nPar, const Int_t npoint, const Int_t nBloc, Int_t * BlocID, Int_t * nLinePerBloc
					, Int_t * nMeasPerLine);	
	~LMSChi2Function();	
	/// Returns the type of the fit method 
	inline Type_t Type() const { return ROOT::Math::FitMethodFunction::kLeastSquare; }
	
	/// Clone the Chi2 function
	inline ROOT::Math::IMultiGenFunction * Clone() const { return new LMSChi2Function(NDim(), NPoints(),fnBloc,fBlocID,fnLinePerBloc,fnMeasPerLine); }
	
	double DataElement(const double *par, unsigned int i, double *g  = 0) const; 
	
	inline void SetnBlocIDs(Int_t n){fnBloc = n;};
	void SetBlocID(Int_t * tab);
	void SetLinePerBloc(Int_t * tab);
	void SetMeasPerLine(Int_t * tab);
	inline void NFree(Int_t n){fFree = n ;};
	inline Int_t NFree(){return fFree;};
	
private: 
	
	Double_t DoEval(const double * par) const ;
	Int_t fnBloc;
	Int_t *fBlocID;
	Int_t *fnLinePerBloc;
	Int_t *fnMeasPerLine;
	Int_t fFree;
	
	//ClassDef(LMSChi2Function,1) 

};

#endif
