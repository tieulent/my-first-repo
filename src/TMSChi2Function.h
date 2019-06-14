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

#ifndef TMSCHI2FUNCTION_H
#define TMSCHI2FUNCTION_H

/// \ingroup "Geometry Monitoring System"
/// \class TMSChi2Function
/// \brief Class describing the Chi2 function used to minimize the TMS part of the system.
///
//  Raphael Tieulent, Institut de Physique Nucleaire de Lyon

#include <Math/FitMethodFunction.h>
class TMSChi2Function : public ROOT::Math::FitMethodFunction { 
	
public:
	
	// needed for AClic
	typedef ROOT::Math::BasicFitMethodFunction<ROOT::Math::IMultiGenFunction>::Type_t  Type_t;
	
	TMSChi2Function(int nPar, const Int_t nPoint, Int_t BlocID, Int_t nLinePerBloc, Int_t nMeasPerLine);	
	~TMSChi2Function();	
	/// Returns the type of the fit method 
	inline Type_t Type() const { return ROOT::Math::FitMethodFunction::kLeastSquare; }
	
	/// Clone the Chi2 function
	inline ROOT::Math::IMultiGenFunction * Clone() const { return new TMSChi2Function(NDim(), NPoints(),fBlocID,fnLinePerBloc,fnMeasPerLine); }
	
	double DataElement(const double *par, unsigned int i, double *g  = 0) const; 
	
	inline void SetBlocID(Int_t Id){fBlocID=Id;};
	inline void SetLinePerBloc(Int_t n){fnLinePerBloc=n;};
	inline void NFree(Int_t n){fFree = n ;};
	inline Int_t NFree(){return fFree;};
	
private: 
	
	Double_t DoEval(const double * par) const ;
	Int_t fBlocID;
	Int_t fnLinePerBloc;
	Int_t fnMeasPerLine;
	Int_t fFree;
	
	//ClassDef(TMSChi2Function,1) 

};
#endif
