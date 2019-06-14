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

#include <iostream>
#include <TMath.h>
#include <AliLog.h>
#include <TBranch.h>
#include <TTree.h>
#include <TObjArray.h>

#include "Bloc.h"
#include "Chamber.h"
#include "Wall.h"
#include "Box.h"
#include "GMSImage.h"
#include "BoxBCAM.h"
#include "BoxPROX.h"
#include "BlocEMS.h"
#include "BlocTMS.h"
#include "BlocLMSBCAM.h"
#include "BlocLMSPROX.h"
#include "ImageResult.h"
#include "BCAMImageResult.h"
#include "PROXImageResult.h"
#include "Spectro.h"
#include "GMSAnalyser.h"
using namespace std;

//ClassImp(Spectro)

//______________________________________________________________________________
/** \brief Spectrometer constructor
	
	The construction of the spectrometer need two input files 
	
	\author Raphael Tieulent IPN Lyon
	\date 2006-10-08
	\param  ChamberFileName : Describes the Chamber position and orientation
	\param  BlocFileName : Describes the optical lines
	\param  Type : Type of the spectrometer (MEasured or Simulated)
**/
Spectro::Spectro(const char * ChamberFileName, const char * BlocFileName, ESpectroType Type) : TObject(), fType(Type),fNbChamber(0),fNbWall(0),fNbBloc(0),fNbBox(0){
	
	fImages = new TMap;
	fImages->SetOwner(1);
	
	fReferenceImages = new TMap;
	fReferenceImages->SetOwner(1);
	
	fBloc = new TObjArray();
	fBloc->SetOwner(1);

	fBox = new TObjArray();
	fBox->SetOwner(1);

	
	
	if(gGMSAnalyser->GetPrintLevel() > 1) AliInfo("Start Spectrometer creation");
	if(gGMSAnalyser->GetPrintLevel() > 1) AliInfo("Start Chambers creation");
	CreateChambers(ChamberFileName);
	
	if(gGMSAnalyser->GetPrintLevel() > 1) AliInfo("Start Blocs creation");
	CreateBlocs(BlocFileName);    
	
	
	// initialization of dead lines
    LineDead = new Bool_t*[fNbBloc];
    for (int i=0;i<fNbBloc;i++) {
        LineDead[i] = new Bool_t[GetBloc(i)->GetNLines()];
        for(int j=0;j<GetBloc(i)->GetNLines();j++)
            LineDead[i][j] = kFALSE;
    }

    // bloc to be used :
    fBlocEnabled = new Bool_t[fNbBloc];
    for (int i=0;i<fNbBloc;i++)
        fBlocEnabled[i] = kTRUE;
}


//______________________________________________________________________________
Spectro::~Spectro() {

	delete fBox;
	delete fBloc;
	delete fImages;
	
	for(int i=fNbChamber-1;i>=0;i--) delete fChamber[i];
    delete[] fChamber;

   if(fNbWall>0) {
   	for(int i=0;i<fNbWall;i++) delete fWall[i];
   	delete[] fWall;
   }
   
    for(int i=0;i<fNbBloc;i++) delete[] LineDead[i];
    delete[] LineDead;
	
    delete[] fBlocEnabled; fBlocEnabled = NULL;
}

//______________________________________________________________________________
/** \brief Create the Chambers contained in the Spectrometer
	
	\author Raphael Tieulent IPN Lyon
	\date 2006-10-08
	\param  FileName : Name of the file containing the definition of the chambers
**/

void Spectro::CreateChambers(const char * FileName) {
    FILE * fp;
    if(!(fp = fopen(FileName,"r"))) 
		Fatal("Spectro::CreateChambers","Enable to open chamber database file %s",FileName);
    
    int PlatformID, ChamberID, WallID;
    char line[256];
    int iChamber = 0, iWall =0;
    Double_t pos[3], ang[3], size[3];
	Int_t * platforms = new Int_t[4];
    while ( fgets ( line, sizeof(line), fp ) != NULL ) {
        char cdump;
        switch ( line [0] ) {
        	case '#': // Line commented
            	break;
        	case '>': // First step finish
				fChamber  = new Chamber*[fNbChamber];
				if(fNbWall>0) fWall = new Wall*[fNbWall];
				break;
			case 'C': // Number of Chambers
				sscanf ( line, "%c %d", &cdump, &(this->fNbChamber));
				break;
			case 'W': // Number of Wall
				sscanf ( line, "%c %d", &cdump, &(this->fNbWall));
				break;
			case 'c': 
				//Create New Chamber
				{
				sscanf ( line, "%c %d %lf %lf %lf %lf %lf %lf  %lf %lf %lf %d %d %d %d"
						, &cdump, &ChamberID, &pos[0], &pos[1], &pos[2], &ang[0], &ang[1], &ang[2], &size[0], &size[1], &size[2], &platforms[0], &platforms[1], &platforms[2], &platforms[3]);	    
				for(int i=0;i<3;i++) ang[i] *= -1.; // Because Vanik definition of rotations is opposite
				fChamber[iChamber] = new Chamber(ChamberID,pos,ang,size, platforms,4);
				//printf("CREATE CHAMBER AT ADDRESS : %x \n",fChamber[iChamber]);
				iChamber++;
				break;
				}
			case 'w': //Create New Wall
				sscanf ( line, "%c %d %lf %lf %lf %lf %lf %lf "
						, &cdump, &WallID, &pos[0], &pos[1], &pos[2], &ang[0], &ang[1], &ang[2] );	    
				fWall[iWall] = new Wall(WallID,pos,ang);
				iWall++;
				break;
        }
    }
    fclose ( fp );
    if(iChamber!=fNbChamber) 
		Error("Spectro::CreateChambers","Number of declared chamber is not consistent. Check file %s",FileName);     
   
    if(iWall!=fNbWall) 
		Error("Spectro::CreateChambers","Number of declared Wall is not consistent. Check file %s",FileName);     	

	return;
}
//______________________________________________________________________________
/** \brief Collect the number of boxes to create from the optical line definition file
	
	
	\author Raphael Tieulent IPN Lyon
	\date 2006-10-08
	\param  FileName : Name of the file containing the definition of the optical lines
	\return Returns the number of boxes to be created
**/

int Spectro::GetNbBoxes(const char * FileName) {
    FILE * fp;
    if(!(fp = fopen(FileName,"r"))) 
		Fatal("Spectro::GetNbBoxes","Enable to open chamber database file %s",FileName);
	
    char line[256];
    int iBoxs = 0;
    while ( fgets ( line, sizeof(line), fp ) != NULL ) {
		if ( (line[0] == ' ') || (line[0] == '\t')) iBoxs +=2;
    }
    fclose ( fp );

    return iBoxs;
}

//______________________________________________________________________________
/** \brief Collect the number of optical lines from the bloc information
	
	
	\author Raphael Tieulent IPN Lyon
	\date 2008-03-12
	\return Returns the number of optical lines
**/

int Spectro::GetNbLines() {
	int NbLine = 0;
	for(int ibloc=0;ibloc<fNbBloc;ibloc++)
	 	NbLine += GetBloc(ibloc)->GetNLines();
	
    return NbLine;
}

//______________________________________________________________________________

/// Create the blocs of optical lines contained in the Spectrometer
void Spectro::CreateBlocs(const char * FileName) {
	/// FileName is the name of the file which contains the optical lines information.
	/// This name file can be set in the settings GUI.
	
    this->fNbBox = GetNbBoxes(FileName);

    if(gGMSAnalyser->GetPrintLevel() > 0) AliInfo(Form("%d Boxes will be created",fNbBox));
    int iBloc;
    int NbLines;
    char BCAMID1[6],BCAMID2[6];
    int iLine,Ch1,Plat1,Place1,Ch2,Plat2,Place2;
//    int TubeID, MaskID;
    char TubeID[6], MaskID[6];
    /* ATTENTION : bien ranger les elements les uns derriere les autre, et ne
    pas en ranger 2 au meme endroit */
    /* ATTENTION a passer les bon pointeurs */
    int n = 0; // = pointeur courant sur **boites
    int m = 0; // = pointeur courant sur **fBloc    
    
    FILE * fp;
    if(!(fp = fopen(FileName,"r"))) 
		AliFatal(Form("Enable to open Bloc database file %s",FileName));

    char line[256];
    while ( fgets ( line, sizeof(line), fp ) != NULL ) {
        char cdump;
		char sdump[10];
        switch ( line [0] ) {
        case '#': // Line commented
            break;
        case '>': // Number total of Bloc
            sscanf ( line, "%s %s %s %s %d", &sdump,&sdump,&sdump,&sdump, &(this->fNbBloc));
    		if(gGMSAnalyser->GetPrintLevel() > 0) AliInfo(Form("%d Blocs will be created",fNbBloc));			
            break;
        case 'B': // BlocLMSBCAM
			fNBlocLMS++;
	    	sscanf ( line, "%c %d %d", &cdump, &iBloc, &NbLines);
			
			// Create the Bloc
			fBloc->Add(new BlocLMSBCAM(NbLines));

			for( int i=0;i<NbLines ;i++ ){
				fscanf(fp,"%d %d %d %d %s %d %d %d %s "
					,&iLine
					,&Ch1,&Plat1,&Place1,&BCAMID1
					,&Ch2,&Plat2,&Place2,&BCAMID2);
				// Create the Boxes
				fBox->Add((TObject*)new BoxBCAM(Ch1,Plat1,Place1,BCAMID1));
				BoxBCAM * objBox = (BoxBCAM *) fBox->Last();
				objBox->CreateVolume(GetChamberByID(Ch1),((objBox->GetType()==1) ? kBlack : kBlue));
				
				fBox->Add((TObject*)new BoxBCAM(Ch2,Plat2,Place2,BCAMID2));
				BoxBCAM * ccdBox = (BoxBCAM *) fBox->Last();
				ccdBox->CreateVolume(GetChamberByID(Ch2),((ccdBox->GetType()==1) ? kBlack : kBlue));
				
				TObjString * imgName = new TObjString(Form("LMSCH%02dCH%02dP%d",Ch1,Ch2,Place1/10));
				TObjString * refImgName = new TObjString(Form("RefLMSCH%02dCH%02dP%d",Ch1,Ch2,Place1/10));
				BCAMImageResult * imgResult = new BCAMImageResult(imgName->String().Data());
				BCAMImageResult * imgReference = new BCAMImageResult(refImgName->String().Data());
				
				AddImage(imgName, imgResult);
				AddReferenceImage(refImgName, imgReference);

				((BlocLMSBCAM *)fBloc->Last())->AddImage(new GMSImage(imgName,GMSImage::kBCAM,objBox,ccdBox,imgResult,imgReference));
				
				if(gGMSAnalyser->GetPrintLevel() > 0) AliInfo(Form("Line %d created",i));
	    	}
			if(gGMSAnalyser->GetPrintLevel() > 0) AliInfo(Form("Add LMS BCAM bloc[%d] = %x",m,fBloc->Last()));
			// Move courant pointers 
			n += 2*NbLines;
			m++;
            break;
        case 'P': // BlocLMSPROX
			fNBlocLMS++;
	    	sscanf ( line, "%c %d %d", &cdump, &iBloc, &NbLines);
				
			// Create the Bloc
			fBloc->Add(new BlocLMSPROX(NbLines));
				
			for( int i=0;i<NbLines ;i++ ){
				fscanf(fp,"%d %d %d %d %s %d %d %d %s "
					,&iLine
		       		,&Ch1,&Plat1,&Place1,&MaskID
		       		,&Ch2,&Plat2,&Place2,&TubeID);
				// Create the Boxes
				fBox->Add( (TObject *)  new BoxPROX(Ch1,Plat1,Place1,MaskID,BoxPROX::MaskType));
				BoxPROX * objBox = (BoxPROX *) fBox->Last();
				objBox->CreateVolume(GetChamberByID(Ch1),0);
				
				fBox->Add( (TObject *)  new BoxPROX(Ch2,Plat2,Place2,TubeID,BoxPROX::TubeType));
				BoxPROX * ccdBox = (BoxPROX *) fBox->Last();
				ccdBox->CreateVolume(GetChamberByID(Ch2),0);
				
				TObjString * imgName = new TObjString(Form("LMSCH%02dCH%02dP%d",Ch1,Ch2,Place1/10));
				TObjString * refImgName = new TObjString(Form("RefLMSCH%02dCH%02dP%d",Ch1,Ch2,Place1/10));
				PROXImageResult * imgResult = new PROXImageResult(imgName->String().Data());
				PROXImageResult * imgReference = new PROXImageResult(refImgName->String().Data());
				
				AddImage(imgName, imgResult);
				AddReferenceImage(refImgName, imgReference);
				
				((BlocLMSPROX *)fBloc->Last())->AddImage(new GMSImage(imgName,GMSImage::kPROX,objBox,ccdBox,imgResult,imgReference));
				
			}
			// Move courant pointers 
			n += 2*NbLines; 
			m++;
            break;
        case 'W': // BlocEMS
			fNBlocEMS++;
			sscanf ( line, "%c %d %d", &cdump, &iBloc, &NbLines);
				
			// Create the Bloc
			fBloc->Add(new BlocEMS(NbLines));
				
			for( int i=0;i<NbLines ;i++ ){
				fscanf(fp,"%d %d %d %d %s %d %d %d %s\n"
					,&iLine
					,&Ch1,&Plat1,&Place1,&BCAMID1 		// On the chamber
					,&Ch2,&Plat2,&Place2,&BCAMID2);		// On the Wall
				// Create the Boxes
				fBox->Add((TObject*)new BoxBCAM(Ch1,Plat1,Place1,BCAMID1));
				BoxBCAM * objBox = (BoxBCAM *) fBox->Last();
				objBox->CreateVolume(GetChamberByID(Ch1),((objBox->GetType()==1) ? kBlack : kBlue));
				
				fBox->Add((TObject*)new BoxBCAM(Ch2,Plat2,Place2,BCAMID2));
				BoxBCAM * ccdBox = (BoxBCAM *) fBox->Last();
				ccdBox->CreateVolume(GetChamberByID(Ch2),((ccdBox->GetType()==1) ? kBlack : kBlue));
				
				TObjString * imgName = new TObjString(Form("EMSCH%02dCH%02dP%d",Ch1,Ch2,Place1/10));
				TObjString * refImgName = new TObjString(Form("RefEMSCH%02dCH%02dP%d",Ch1,Ch2,Place1/10));
				BCAMImageResult * imgResult = new BCAMImageResult(imgName->String().Data());
				BCAMImageResult * imgReference = new BCAMImageResult(refImgName->String().Data());
				
				AddImage(imgName, imgResult);
				AddReferenceImage(refImgName, imgReference);
				
				((BlocEMS *)fBloc->Last())->AddImage(new GMSImage(imgName,GMSImage::kBCAM,objBox,ccdBox,imgResult,imgReference));
				
				if(gGMSAnalyser->GetPrintLevel() > 0) AliInfo(Form("Line %d created",i));
				
			}
			// Move courant pointers 
			n += 2*NbLines; 
			m++;
            break;
        case 'T': // BlocTMS
			fNBlocTMS++;
			sscanf ( line, "%c %d %d", &cdump, &iBloc, &NbLines);
			
			// Create the Bloc
			fBloc->Add(new BlocTMS(NbLines));
	    	
			for( int i=0;i<NbLines ;i++ ){
				
				fscanf(fp,"%d %d %d %d %s %d %d %d %s\n" ,&iLine ,&Ch1,&Plat1,&Place1,&BCAMID1  ,&Ch2,&Plat2,&Place2,&BCAMID2);		
				if(Ch1 != Ch2)  AliError("Two BCAM on different chambers in the creation of a BlocTMS");
				
				// Create the Boxes
				fBox->Add((TObject*)new BoxBCAM(Ch1,Plat1,Place1,BCAMID1));
				BoxBCAM * objBox = (BoxBCAM *) fBox->Last();
				objBox->CreateVolume(GetChamberByID(Ch1),((objBox->GetType()==1) ? kBlack : kBlue));
				
				fBox->Add((TObject*)new BoxBCAM(Ch2,Plat2,Place2,BCAMID2));
				BoxBCAM * ccdBox = (BoxBCAM *) fBox->Last();
				ccdBox->CreateVolume(GetChamberByID(Ch2),((ccdBox->GetType()==1) ? kBlack : kBlue));
								
				TObjString * imgName = new TObjString(Form("TMSCH%02dP%dP%d",Ch1,Place1/10,Place2/10));
				TObjString * refImgName = new TObjString(Form("RefTMSCH%02dP%dP%d",Ch1,Place1/10,Place2/10));
				BCAMImageResult * imgResult = new BCAMImageResult(imgName->String().Data());
				BCAMImageResult * imgReference = new BCAMImageResult(refImgName->String().Data());
				
				AddImage(imgName, imgResult);
				AddReferenceImage(refImgName, imgReference);
				
				((BlocTMS *)fBloc->Last())->AddImage(new GMSImage(imgName,GMSImage::kBCAM,objBox,ccdBox,imgResult,imgReference));
				
				if(gGMSAnalyser->GetPrintLevel() > 0) AliInfo(Form("Ligne %d created",i));
			}
			
			// Move current pointers 
			n += 2*NbLines; 
			m++;
            break;

        }
    }
    fclose ( fp );
}


//______________________________________________________________________________

/// Print out to thefile "display_spectro.out" the memory addresses of the tables of boxes, chambers and blocs
void Spectro::Print() const {
//    FILE *test_spectro = fopen("display_spectro.out","w+");
//    fprintf(test_spectro,"boites :\n");
//    for(int i=0;i<fNbBox;i++) {
//    fprintf(test_spectro,"%x %x\n",&(fBox[i]),fBox[i]);
//    }
//    fprintf(test_spectro,"chambres :\n");
//    for(int i=0;i<(fNbChamber);i++) {
//    fprintf(test_spectro,"%x %x\n",&(fChamber[i]),fChamber[i]);
//    }
//    fprintf(test_spectro,"walls :\n");
//    for(int i=0;i<fNbWall;i++) {
//    fprintf(test_spectro,"%x\n",&(fWall[i]));
//    }
//    fclose(test_spectro);
//
//    FILE *test_bloc = fopen("display_bloc.out","w+");
//    fclose(test_bloc);
//    for(int i=0;i<fNbBloc;i++) fBloc[i]->Display();
}


// ______________________________________________________________________________
// 
// / Initialization of the chamber positions
// void Spectro::InitChamberPosition(Chamber * Ch, Double_t shift[3], Double_t ang[3]) {
// 	/// The chambers are positionned from their theoritical positions by
// 	/// applying a translation (table "shift") and a rotation (table "ang").
// 	
//     /* Le referentiel theorique est le meme que le referentiel global -> on peut
//     utiliser la fonction "calc_mat_chgt_glob_plan" pour determiner les matrices
//     de changement de base faisant passer du referentiel global au referentiel de
//     la chambre dans sa position reelle. */
//      Ch->Translate(shift);
//      Ch->CalculateMatrixChgtGlobPlane(ang);
//     
// }



//______________________________________________________________________________

/// Initialization  of the displacements of the chambers 
void Spectro::InitDepChambers(Double_t *parametres) {
	for(int i=0;i<fNbChamber;i++){ 	
		fChamber[i]->SetDisplacementParameters(&(parametres[6*i]));
		fChamber[i]->CalculateDisplacementMatrix(&(parametres[6*i]));
	}
}


//______________________________________________________________________________

/// Initialization  of the displacements of the chambers and of the paltforms
void Spectro::InitGlobalDisplacements(Double_t *parametres) {
	for(int iCh=0;iCh<fNbChamber;iCh++){ 	
		fChamber[iCh]->SetDisplacementParameters(&(parametres[30*iCh]));
		fChamber[iCh]->CalculateDisplacementMatrix(&(parametres[30*iCh]));
		for(int iPl=0;iPl<4;iPl++){
			fChamber[iCh]->GetPlatform(iPl)->SetDisplacementParameters(&(parametres[30*iCh+6*(iPl+1)]));
			fChamber[iCh]->GetPlatform(iPl)->CalculateDisplacementMatrix(&(parametres[30*iCh+6*(iPl+1)]));
		}
	}
}

//______________________________________________________________________________

/// Initialization of the  displacements of the platforms
void Spectro::InitDepPlatforms(Chamber * Ch, Double_t *parametres) {
	for(int i=0;i<4;i++)
		Ch->GetPlatform(i)->CalculateDisplacementMatrix(&(parametres[6*i]));
}


//______________________________________________________________________________

/* initialisation des deplacements de la chambre supportant les liens
exterieurs */
void Spectro::init_dep_chambre_ext(Double_t *parametres) {
    if(fExternalChamberID == -1) 
        Fatal("Spectro::init_dep_chambre_ext","le numero de la chambre exterieur doit etre initialise");

    fChamber[fExternalChamberID]->CalculateDisplacementMatrix(parametres);
}


//______________________________________________________________________________

/* initialisation des deplacements de la chambre supportant les liens
exterieurs, version sans deformation */
void Spectro::init_dep_chambre_ext2(Double_t *parametres) {
    if(fExternalChamberID == -1) 
         Fatal("Spectro::init_dep_chambre_ext2","le numero de la chambre exterieur doit etre initialise");       
    
    fChamber[fExternalChamberID]->CalculateDisplacementMatrix(parametres);
}


//______________________________________________________________________________

/// Initialisation des deplacements des chambres supportant les liens
/// interieurs : deformation seule de la chambre fExternalChamberID, et deplacements et
/// deformation des autres chambres 
void Spectro::init_dep_chambres_int(Double_t *parametres) {
	/// parametres is a table of 6 doubles
	if(fExternalChamberID == -1) 
		Fatal("Spectro::init_dep_chambre_int","le numero de la chambre exterieur doit etre initialise");       
	for(int i=0;i<fNbChamber;i++) {
		if (i==fExternalChamberID) continue;
		fChamber[i]->CalculateDisplacementMatrix(&(parametres[6*i]));
	}

}

//______________________________________________________________________________

/// Random initialization of the position and orientation of the optical boxes.
/// Used in simulation mode to mimic the installation error of the boxes  
void Spectro::InitRandomBoxPosition(TRandom3 *rand_obj, const Spectro *SimulSpectro) {
	/// The boxes are randomly positionned only for the MeasuredSpectrometer into simulation mode.
	/// Therefore we use the position and orientation of the boxes in the SimulatedSpectrometer as a reference. 
	for(int i=0;i<fNbBox;i++)
        GetBox(i)->random_box(rand_obj,SimulSpectro->GetBox(i));
}


//______________________________________________________________________________

/// Random initialization of the deformation of the walls.
void Spectro::InitRandomWallDeformation(TRandom3 *rand_obj) {
	if(fNbWall>0)
		for(int i=0;i<fNbWall;i++)
			fWall[i]->SetRandomWallDeformation(rand_obj);
}


//______________________________________________________________________________

/// Calculate the Reference Images for ALL blocs
void Spectro::CalcRefImages() {
    for(int i=0;i<fNbBloc;i++) GetBloc(i)->CalcRefImages();
}

//______________________________________________________________________________

/// Calculate the Reference Images for the bloc iBloc
void Spectro::CalcRefImages(Int_t iBloc) {
	GetBloc(iBloc)->CalcRefImages();
}

//______________________________________________________________________________

/* calcul des deplacements des images de tous les blocs a partir de la position
theorique des chambres */
void Spectro::CalcTheoFinalImages() {
    for(int i=0;i<fNbBloc;i++) GetBloc(i)->CalcTheoFinalImages();
}


//______________________________________________________________________________

/// calcul des deplacements des images de tous les blocs a partir de la position
/// theorique des chambres pour les lignes exterieures et a partir des positions
/// reelles et mesurees des chambres pour les lignes interieures */
void Spectro::calc_imagef_extTh() {
    /// @todo Translate in english
	 int i;
    for(i=0;i<fNbExternalBloc;i++) GetBloc(i)->CalcTheoFinalImages();
    for(i=fNbExternalBloc;i<fNbBloc;i++) GetBloc(i)->CalcFinalImages();
}


//______________________________________________________________________________

/// calcul des deplacements des images de tous les blocs (flagges comme etant
/// presents) a partir de la position reelle ou mesuree des chambres */
void Spectro::CalcFinalImages() {
    for(int i=0;i<fNbBloc;i++) 
        if (fBlocEnabled[i]) GetBloc(i)->CalcFinalImages();
    
}

//______________________________________________________________________________

/// Calculate the final images of Bloc "iBloc" 
void Spectro::CalcFinalImages(Int_t iBloc) {
    if (fBlocEnabled[iBloc]) GetBloc(iBloc)->CalcFinalImages();

}


//______________________________________________________________________________

/// Prints out the final images of Bloc "iBloc" 
void Spectro::PrintFinalImages(Int_t iBloc) {
    if (fBlocEnabled[iBloc]) GetBloc(iBloc)->PrintFinalImages();

}
//______________________________________________________________________________

/// Prints out the displacements images of Bloc "iBloc" 
void Spectro::PrintDisplacements(Int_t iBloc) {
    if (fBlocEnabled[iBloc]) GetBloc(iBloc)->PrintDisplacements();

}

//______________________________________________________________________________

/* calcul des deplacements des images de tous les blocs exterieurs a partir de
la position reelle ou mesuree des chambres */
void Spectro::calc_imagef_ext() {
    for(int i=0;i<fNbExternalBloc;i++) GetBloc(i)->CalcFinalImages();
}


//______________________________________________________________________________

/* calcul des deplacements des images de tous les blocs interieurs a partir de
la position reelle ou mesuree des chambres */
void Spectro::calc_imagef_int() {
    for(int i=fNbExternalBloc;i<fNbBloc;i++) GetBloc(i)->CalcFinalImages();
}


//______________________________________________________________________________

/// Add the Intrinsic error of the measurement
void Spectro::AddIntrinsicError(TRandom3 *rand_obj,Double_t IntResFactor) {
    for(int i=0;i<fNbBloc;i++) GetBloc(i)->AddIntrinsicError(rand_obj,IntResFactor);
}


//______________________________________________________________________________

/// Read the uncertancie of each measurement from file "sig"
void Spectro::ReadSigmas(FILE *sig) {
    Double_t x;

    for(int i=0;i<fNbBloc;i++) {
        for(int j=0;j<GetBloc(i)->GetNLines();j++) {
            for(int k=0;k<GetBloc(i)->GetNMeasuresPerLine();k++) {
                fscanf(sig,"%le",&x);
                GetBloc(i)->SetSigma(j,k,x);
				
            }
        }
    }
}

//______________________________________________________________________________

/// Write the uncertancie of each measurement to file "sig"
void Spectro::WriteSigmas(FILE *sig) {
    for(int i=0;i<fNbBloc;i++) {
        for(int j=0;j<GetBloc(i)->GetNLines();j++) {
            for(int k=0;k<GetBloc(i)->GetNMeasuresPerLine();k++) {
 				fprintf(sig,"%le\n",GetBloc(i)->GetSigma(j,k));
            }
        }
    }
}

//______________________________________________________________________________

/// Read the references images from a data file
void Spectro::ReadRefImages(FILE *fp, int PrintDataFlag){
	int idump; float fdump; char sdump[20];
	
	int hour,min,sec,day,month,year;

 	fscanf(fp,"%4d%2d%2d%2d%2d%2d \n",&year,&month,&day,&hour,&min,&sec);
 	
	if (PrintDataFlag) 
		printf("Ref IMAGE taken at : %2dh%2d:%2d on %2d-%2d-%4d \n",hour,min,sec,day,month,year);
 	
	// for(int ibloc=0;ibloc<fNbBloc;ibloc++)
	// 	fBloc[ibloc]->ReadRefImages(fp,PrintDataFlag);
	
	TString Tmp;
	double ValBCAM[4]; // XL, YL, XR, YR	
	double ValPROX[5]; // X, Y, MX, MY, Phi
	int Bloc, Line;
	
	for(int iLine = 0; iLine < GetNbLines(); iLine++){
		fscanf(fp,"%s ",Tmp.Data());
		cout<<"DATA : "<<Tmp.Data()<<endl;
		
		if(strstr(Tmp.Data(),"TMS")) { // TMS
			TString LineName = strstr(Tmp.Data(),"TMS");
			LineName.Resize(11);
			//cout<<"TMS: "<<LineName.Data()<<endl;
			fscanf(fp,"%lf %lf %lf %lf\n ",ValBCAM,ValBCAM+1,ValBCAM+2,ValBCAM+3);
			// printf("%s : %.2f %.2f %.2f %.2f dX=%.2f  dY=%.2f dL=%.2f\n ",LineName.Data(),ValBCAM[0],ValBCAM[1],ValBCAM[2],ValBCAM[3]
			// 	,(ValBCAM[2]-ValBCAM[0]),ValBCAM[3]-ValBCAM[1],TMath::Sqrt((ValBCAM[2]-ValBCAM[0])*(ValBCAM[2]-ValBCAM[0])+(ValBCAM[3]-ValBCAM[1])*(ValBCAM[3]-ValBCAM[1])));
			for(int i = 0; i < 4; i++) ValBCAM[i] /= 1.e6;
			
			FindTMSLine(LineName, Bloc,Line);
			//cout<<LineName.Data()<<" "<<Bloc<< " "<< Line <<endl;
			((BlocTMS*)GetBloc(Bloc))->SetRefImages(Line,ValBCAM);
 			
		} else if(strstr(Tmp.Data(),"BCAM")) { // LMS BCAM
			TString LineName = strstr(Tmp.Data(),"LMS");
			LineName.Resize(13);
			cout<<"LMS BCAM: "<<LineName.Data()<<endl;
			fscanf(fp,"%lf %lf %lf %lf\n ",ValBCAM,ValBCAM+1,ValBCAM+2,ValBCAM+3);
		} else { //LMS RasNik
			TString LineName = strstr(Tmp.Data(),"LMS");
			LineName.Resize(13);
			cout<<"LMS RasNik: "<<LineName.Data()<<endl;
			fscanf(fp,"%lf %lf %lf %lf %lf\n",ValPROX,ValPROX+1,ValPROX+2,ValPROX+3,ValPROX+4);
			ValPROX[0] /= 1.e6;
			ValPROX[1] /= 1.e6;
			ValPROX[4] /= 1.e3;
			FindLMSPROXLine(LineName, Bloc,Line);
			((BlocLMSPROX*)GetBloc(Bloc))->SetRefImages(Line,ValPROX);
			
		}
		
		
	}

	

}

//______________________________________________________________________________

/// Read the current images from a data file 
void Spectro::ReadFinalImages(FILE *fp, int PrintDataFlag){
	int idump; float fdump; char sdump[20];
	 		
	TString Tmp;
	double ValBCAM[4]; // XL, YL, XR, YR	
	double ValPROX[5]; // X, Y, MX, MY, Phi
	int Bloc, Line;
	// cout<<"Nb Lines "<<GetNbLines()<<endl;
	
	for(int iLine = 0; iLine < GetNbLines(); iLine++){
		fscanf(fp,"%s ",Tmp.Data());
		if(strstr(Tmp.Data(),"TMS")) { // TMS
			TString LineName = strstr(Tmp.Data(),"TMS");
			LineName.Resize(11);
			fscanf(fp,"%lf %lf %lf %lf\n ",ValBCAM,ValBCAM+1,ValBCAM+2,ValBCAM+3);
			for(int i = 0; i < 4; i++) ValBCAM[i] /= 1.e6;
			
			FindTMSLine(LineName, Bloc,Line);
			// cout<<LineName.Data()<<" "<<Bloc<< " "<< Line <<endl;
			((BlocTMS*)GetBloc(Bloc))->SetFinalImages(Line,ValBCAM);
			((BlocTMS*)GetBloc(Bloc))->CalculateImageDisplacements(Line);
 			
		} else if(strstr(Tmp.Data(),"BCAM")) { // LMS BCAM
			TString LineName = strstr(Tmp.Data(),"LMS");
			LineName.Resize(13);
			cout<<"LMS BCAM: "<<LineName.Data()<<endl;
			fscanf(fp,"%lf %lf %lf %lf\n ",ValBCAM,ValBCAM+1,ValBCAM+2,ValBCAM+3);
		} else { //LMS RasNik
			TString LineName = strstr(Tmp.Data(),"LMS");
			LineName.Resize(13);
			fscanf(fp,"%lf %lf %lf %lf %lf\n",ValPROX,ValPROX+1,ValPROX+2,ValPROX+3,ValPROX+4);
			ValPROX[0] /= 1.e6;
			ValPROX[1] /= 1.e6;
			ValPROX[4] /= 1.e3;
			FindLMSPROXLine(LineName, Bloc,Line);
			// cout<<LineName.Data()<<" "<<Bloc<< " "<< Line <<endl;
			
			((BlocLMSPROX*)GetBloc(Bloc))->SetFinalImages(Line,ValPROX);
			((BlocLMSPROX*)GetBloc(Bloc))->CalculateImageDisplacements(Line);
		}
		
		
	}
 	
 }

//______________________________________________________________________________

/// Return a pointer to the chamber "chid".
Chamber * Spectro::GetChamberByID(Int_t chid) const {
	for(int i=0;i<fNbChamber;i++){
		//printf("Checking Chamber element %d with ID %d with the address %x \n",i,fChamber[i]->GetID(),fChamber[i]);
		if(fChamber[i]->GetID() == chid) return fChamber[i];
	}
	Fatal("Spectro::GetChamberByID","No Chamber found with the id %d",chid);
}


//______________________________________________________________________________

/// Return a pointer to the wall "wallid".
Wall * Spectro::GetWall(Int_t wallid) const {
	for(int i=0;i<fNbWall;i++)
		if(fWall[i]->GetID() == wallid) return fWall[i];
	Fatal("Spectro::GetWall","No Wall found with the id %d",wallid);
}

//______________________________________________________________________________
void Spectro::FindTMSLine(TString LineName, int &Bloc, int &Line){
	for(int iBloc = 0; iBloc < GetNBloc(); iBloc++){
		if(GetBloc(iBloc)->GetBlocType() != Bloc::kBlocTMS) continue;
		for(int iLine = 0; iLine < GetBloc(iBloc)->GetNLines(); iLine++){
			if((((BlocTMS*)GetBloc(iBloc))->GetImageName(iLine)).CompareTo(LineName) == 0 ){
				Bloc = iBloc;
				Line = iLine;
				return;
			}
		}
	}
	Fatal("Spectro::FindTMSLine","Did not find line %s",LineName.Data());
}
//______________________________________________________________________________
void Spectro::FindTMSLine(TString * LineName, int &Bloc, int &Line){
	TString lineName(LineName->Data());
	FindTMSLine(lineName,Bloc,Line);
}

//______________________________________________________________________________
void Spectro::FindLMSPROXLine(TString LineName, int &Bloc, int &Line){
	for(int iBloc = 0; iBloc < GetNBloc(); iBloc++){
		if(GetBloc(iBloc)->GetBlocType() != Bloc::kBlocLMS) continue;
		for(int iLine = 0; iLine < GetBloc(iBloc)->GetNLines(); iLine++){
			if((((BlocLMSPROX*)GetBloc(iBloc))->GetImageName(iLine)).CompareTo(LineName) == 0 ){
				Bloc = iBloc;
				Line = iLine;
				return;
			}
		}
	}
	Fatal("Spectro::FindLMSPROXLine","Did not find line %s",LineName.Data());
	
}
//______________________________________________________________________________
void Spectro::FindLMSPROXLine(TString * LineName, int &Bloc, int &Line){
	TString lineName(LineName->Data());
	FindLMSPROXLine(lineName,Bloc,Line);
}
