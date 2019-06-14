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
//-----------------------------------------------------------------------------
/// \class LMSChi2Function
/// Use the class "ROOT::Math::Minimizer" which gives the possibility to swith 
/// between different minimizer (Minuit, Fumili, Minuit2 or Fumili2) and
/// different algorithm (MIGRAD, SIMPLEX, ...)
///
/// \author Raphael Tieulent, Institut de Physique Nucleaire de Lyon
//-----------------------------------------------------------------------------
#include <TError.h>
#include <TString.h>

#include "GMSAnalyser.h"
#include "Spectro.h"
#include "LMSChi2Function.h"

//ClassImp(LMSChi2Function)

//_____________________________________________________________________________
LMSChi2Function::LMSChi2Function(int nPar, const Int_t nPoint, const Int_t nBloc, Int_t * BlocID, Int_t * nLinePerBloc
								 , Int_t * nMeasPerLine) : 
ROOT::Math::FitMethodFunction(nPar,nPoint)
{
	fnBloc = nBloc;
	fBlocID = new Int_t[fnBloc];
	fnLinePerBloc = new Int_t[fnBloc];
	fnMeasPerLine = new Int_t[fnBloc];
	SetBlocID(BlocID);
	SetLinePerBloc(nLinePerBloc);
	SetMeasPerLine(nMeasPerLine);
}

//_____________________________________________________________________________
LMSChi2Function::~LMSChi2Function() {
	delete fnMeasPerLine;
	delete fnLinePerBloc;
	delete fBlocID;
}

//_____________________________________________________________________________
void LMSChi2Function::SetBlocID(Int_t * tab) {
	for (int i=0; i<fnBloc; i++) fBlocID[i] = tab[i];
}
//_____________________________________________________________________________
void LMSChi2Function::SetLinePerBloc(Int_t * tab) {
	for (int i=0; i<fnBloc; i++) fnLinePerBloc[i] = tab[i];
}
//_____________________________________________________________________________
void LMSChi2Function::SetMeasPerLine(Int_t * tab) {
	for (int i=0; i<fnBloc; i++) fnMeasPerLine[i] = tab[i];
}

//_____________________________________________________________________________
double LMSChi2Function::DoEval(const double * par) const { 
	/// Evaluation of the Chi2 
	//Info("LMSChi2Function::DoEval","Entering, NPoints = %d",NPoints());
	int ndata = NPoints(); 
	gSimulatedSpectrometer->InitDepChambers((double*)par);
    gSimulatedSpectrometer->CalcFinalImages();
	double chi2 = 0; 
	for (int i = 0; i <  ndata; ++i) { 
		double res = DataElement( par, i);
		chi2 += res*res; 
	}
	chi2 /= (ndata - fFree);
	if(gGMSAnalyser->GetPrintLevel()>=2){
		UpdateNCalls();
		//Info("LMSChi2Function::DoEval","After %d Calls : Chi2 = %f",NCalls(),chi2);
	}
//    Double_t f_param;
//    Int_t NMeas =0;
//    gSimulatedSpectrometer->InitDepChambers((double*)par);
//    gSimulatedSpectrometer->CalcFinalImages();
//    Double_t chi2 = 0.;
//    for(int i=0;i<gSimulatedSpectrometer->GetNBloc();i++) {
//		if (!gSimulatedSpectrometer->IsBlocEnabled(i)) continue;
//		for(int j=0;j<gSimulatedSpectrometer->GetNLine(i);j++) {
//	  		//if(gSimulatedSpectrometer->IsLineDead(i,j)) continue; // Be sure that this given line is not dead
//	    	for(int k=0;k<gSimulatedSpectrometer->GetNMeasurePerLine(i);k++) {
//				f_param = (gMeasuredSpectrometer->GetImageDisplacement(i,j,k) - gSimulatedSpectrometer->GetImageDisplacement(i,j,k)) 
//				/  gMeasuredSpectrometer->GetSigma(i,j,k);
//				
//				//printf("Line %d Mesure %d   SpectroMes %e   SpectroSim %e  Diff %e\n", j, k,  gMeasuredSpectrometer->GetImageDisplacement(i,j,k) ,
//				//						gSimulatedSpectrometer->GetImageDisplacement(i,j,k),gMeasuredSpectrometer->GetImageDisplacement(i,j,k) - gSimulatedSpectrometer->GetImageDisplacement(i,j,k));
//	      		chi2 += f_param*f_param;
//				NMeas++;
//	    	}
//		}
//	}
	return chi2; 
}

//_____________________________________________________________________________
double LMSChi2Function::DataElement(const double *par, unsigned int iData, double *g ) const { 
	/// Calculates the chi2 residual multiplied by its weight + the derivatives of 
	/// the residual w.r.t the parameters \n
	/// \param par : Parameters array.
	/// \param iData : i-th Data contributing to the Chi2 function.
	/// \param g : Gradient of the data element (optional).
	/// \return The data i-th contribution to the fit objective function
	
	double residual = 0. ;
	int iBloc, iLine, iMeas;
	for ( iBloc=0; iBloc <fnBloc; iBloc++) {
		int nMeasPerBloc = fnLinePerBloc[iBloc] * fnMeasPerLine[iBloc];
		if (iData<nMeasPerBloc){
			iLine = iData / fnMeasPerLine[iBloc];
			iMeas = iData % fnMeasPerLine[iBloc];
			break;
		}
		iData -= nMeasPerBloc;
	}
//	Info("LMSChi2Function::DataElement","Entering : iData=%d Minimizer Type = %s , iBloc=%d BlocID=%d  iLine=%d iMeas=%d"
//		,iData,gGMSAnalyser->GetMinimizerType(),iBloc,fBlocID[iBloc],iLine,iMeas);

	//if(gSimulatedSpectrometer->IsLineDead(iBloc,iLine))  // Be sure that this given line is not dead
	residual = (gMeasuredSpectrometer->GetImageDisplacement(fBlocID[iBloc],iLine,iMeas) 
				   - gSimulatedSpectrometer->GetImageDisplacement(fBlocID[iBloc],iLine,iMeas)) 
						/  gMeasuredSpectrometer->GetSigma(fBlocID[iBloc],iLine,iMeas);
				
	//printf("Line %d Mesure %d   SpectroMes %e   SpectroSim %e  Diff %e\n", j, k,  gMeasuredSpectrometer->GetImageDisplacement(i,j,k) ,
	//						gSimulatedSpectrometer->GetImageDisplacement(i,j,k),gMeasuredSpectrometer->GetImageDisplacement(i,j,k) - gSimulatedSpectrometer->GetImageDisplacement(i,j,k));
	
	
	// calculate the derivatives 
//	if (g != 0)    { 
//		fModelFunc->SetParameters(par);
//		fModelFunc->GradientPar( &fX[i], g,1.E-9);
//		// need to mutiply by -1*weight
//		for (int ipar = 0; ipar < fModelFunc->GetNpar(); ++ipar) { 
//			g[ipar] *= -1 * weight;
//		}
//	}
	return residual;
}

