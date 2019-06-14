#include <TROOT.h>
#include <TStopwatch.h>
#include <TObjString.h>
#include <TMap.h>
#include <AliLog.h>
#include <iostream>
using namespace std;

#include "GMSAmandaClient.h"
#include "BCAMImageResult.h"
#include "PROXImageResult.h"
#include "GMSAnalyser.h"
#include "Spectro.h"
#include "BlocTMS.h"
#include "BlocLMSPROX.h"

//ClassImp(GMSAmandaClient)

GMSAmandaClient::GMSAmandaClient(char *HostName, Int_t Port) : AliDCSClient(HostName, Port,1000,20,1) {
	SetServerHost(HostName);
	SetServerPort(Port);
	fValues = new TMap();
};

GMSAmandaClient::GMSAmandaClient(char *HostName, Int_t Port, TString * Request) : AliDCSClient(HostName, Port,1000,20,1){
	SetServerHost(HostName);
	SetServerPort(Port);
	SetRequest(Request);
	fValues = new TMap();
};
GMSAmandaClient::GMSAmandaClient(char *HostName, Int_t Port, TString * Request, UInt_t StartTime, UInt_t EndTime) : AliDCSClient(HostName, Port, 1000,20,1){
	SetServerHost(HostName);
	SetServerPort(Port);
	SetRequest(Request);
	SetStartTime(StartTime);
	SetEndTime(EndTime);
	fValues = new TMap();
};
GMSAmandaClient::~GMSAmandaClient() {
	delete fValues;
};

void GMSAmandaClient::GetDCSValues() {
	Int_t result;

	TMap values;
	fValues->SetOwner(1);


	TString rString(GetRequest()->Data());
	cout<<rString<<endl;

	TObjArray* requests = rString.Tokenize(",");

	AliInfo(Form("Number of values: %d",requests->GetEntries()));

	TStopwatch sw;
	sw.Start();
	
	if(requests->GetEntries() == 0) AliFatal("The request is empty.");

	fValues = GetAliasValues(requests, fStartTime, fEndTime);

	if (!fValues) {
		AliError(Form("Query failed! Result error: %s", GetErrorString(GetResultErrorCode())));
		if(GetResultErrorCode() == AliDCSClient::fgkServerError)	
			AliFatal( Form("Server error: %s",GetServerError().Data()));
	}
	
	sw.Stop();
	AliInfo(Form("Elapsed time: %f",sw.RealTime()));
	AliInfo(Form("Time per alias: %f",sw.RealTime()/requests->GetEntries()));
		
	TIter iter(fValues);
	TObjString* aMeasureName;
	while ((aMeasureName = (TObjString*) iter.Next())) {
		
		TObjArray* valueSet = (TObjArray*) fValues->GetValue(aMeasureName);
		TIter valIter(valueSet);
		AliDCSValue* aValue;
		Float_t val;
		while ((aValue = (AliDCSValue*) valIter.Next())) val = aValue->GetFloat();
		
		cout<<" '"<<aMeasureName->String()<<"' values: "  <<valueSet->GetEntriesFast()<<endl;
		TSeqCollection* MeasureNameSplit = aMeasureName->String().Tokenize("/");
		TObjString * lineName = (TObjString *)MeasureNameSplit->At(0);
		TString measureTag = ((TObjString *)MeasureNameSplit->At(1))->String();
		
		if(measureTag.Contains("XLEFT")){
			BCAMImageResult * imgResult = (BCAMImageResult *) gMeasuredSpectrometer->GetImage(lineName);
			if(imgResult) imgResult->SetXleft(val);
		} else if(measureTag.Contains("YLEFT")){
			BCAMImageResult * imgResult = (BCAMImageResult *) gMeasuredSpectrometer->GetImage(lineName);
			if(imgResult) imgResult->SetYleft(val);
		} else if(measureTag.Contains("XRIGHT")){
			BCAMImageResult * imgResult = (BCAMImageResult *) gMeasuredSpectrometer->GetImage(lineName);
			if(imgResult) imgResult->SetXright(val);
		} else if(measureTag.Contains("YRIGHT")){
			BCAMImageResult * imgResult = (BCAMImageResult *) gMeasuredSpectrometer->GetImage(lineName);
			if(imgResult) imgResult->SetYright(val);
		} else if(measureTag.Contains("XPOS")){
			PROXImageResult * imgResult = (PROXImageResult *) gMeasuredSpectrometer->GetImage(lineName);
			if(imgResult) imgResult->SetXpos(val);
		} else if(measureTag.Contains("YPOS")){
			PROXImageResult * imgResult = (PROXImageResult *) gMeasuredSpectrometer->GetImage(lineName);
			if(imgResult) imgResult->SetYpos(val);
		} else if(measureTag.Contains("XMAG")){
			PROXImageResult * imgResult = (PROXImageResult *) gMeasuredSpectrometer->GetImage(lineName);
			if(imgResult) imgResult->SetXmag(val);
		} else if(measureTag.Contains("YMAG")){
			PROXImageResult * imgResult = (PROXImageResult *) gMeasuredSpectrometer->GetImage(lineName);
			if(imgResult) imgResult->SetYmag(val);
		} else if(measureTag.Contains("ROTATION")){
			PROXImageResult * imgResult = (PROXImageResult *) gMeasuredSpectrometer->GetImage(lineName);
			if(imgResult) imgResult->SetRotation(val);
		}
			
	}
};

//______________________________________________________________________________


Int_t GMSAmandaClient::FindDataBCAM(TString * LineName, Double_t Results[4]) {

	int iResult = 0;
	TObjString * aRequest;
	
	for (int i=0; i<4; i++) {
		Results[i] = 0;
		TString ValName(LineName->Data());
		if(i==0) ValName += "/XLEFT";
		else if(i==1) ValName += "/YLEFT";
		else if(i==2) ValName += "/XRIGHT";
		else if(i==3) ValName += "/YRIGHT";
		
		aRequest = new TObjString(ValName.Data());
		TObjArray* valueSet = (TObjArray*) fValues->GetValue(aRequest);
		TIter valIter(valueSet);
		AliDCSValue* aValue;
		while ((aValue = (AliDCSValue*) valIter.Next())) {
			if((aValue->GetTimeStamp()>fStartTime)&&(aValue->GetTimeStamp()<fEndTime)){
				cout<<aValue->GetFloat()<<endl;
				Results[i] = aValue->GetFloat();
				iResult++;
			}
		} 
	}
	
	
//	int iResult = 0; 
//	for (int i=0; i<4; i++) Results[i] = 0;
//	TIter iter(fValues);
//	TObjString* aRequest;
//	while ((aRequest = (TObjString*) iter.Next())) {
//		TObjArray* valueSet = (TObjArray*) fValues->GetValue(aRequest);
//		TString  ImageName = aRequest->String();
//		
//		if(ImageName.Contains(LineName->Data())) { 
//			//cout<<" '"<<ImageName<<"' values: " <<valueSet->GetEntriesFast()<<" : ";
//
//			TIter valIter(valueSet);
//			AliDCSValue* aValue;
//			while ((aValue = (AliDCSValue*) valIter.Next())) {
//				//if((aValue->GetTimeStamp()>fStartTime)&&(aValue->GetTimeStamp()<fEndTime))
//				//cout<<aValue->GetFloat()<<endl;
//				int idata;
//				if(ImageName.Contains("XLEFT")) idata = 0;
//				else if(ImageName.Contains("YLEFT")) idata = 1;
//				else if(ImageName.Contains("XRIGHT")) idata = 2;
//				else if(ImageName.Contains("YRIGHT")) idata = 3;
//				Results[idata] = aValue->GetFloat();
//			} 
//			iResult++;
//		} 
//	}
	return iResult;
};

//______________________________________________________________________________


Int_t GMSAmandaClient::FindDataPROX(TString * LineName, Double_t Results[5]) {
	
	int iResult = 0;
	TObjString * aRequest;
	
	for (int i=0; i<5; i++) {
		Results[i] = 0;
		TString ValName(LineName->Data());
		if(i==0) ValName += "/XPOS";
		else if(i==1) ValName += "/YPOS";
		else if(i==2) ValName += "/XMAG";
		else if(i==3) ValName += "/YMAG";
		else if(i==4) ValName += "/ROT";
		
		aRequest = new TObjString(ValName.Data());
		TObjArray* valueSet = (TObjArray*) fValues->GetValue(aRequest);
		TIter valIter(valueSet);
		AliDCSValue* aValue;
		while ((aValue = (AliDCSValue*) valIter.Next())) {
			if((aValue->GetTimeStamp()>fStartTime)&&(aValue->GetTimeStamp()<fEndTime)){
				cout<<aValue->GetFloat()<<endl;
				Results[i] = aValue->GetFloat();
				iResult++;
			}
		} 
	}
	return iResult;
}

//______________________________________________________________________________

void GMSAmandaClient::FillDataMap() {
	TMap * imageMap = gMeasuredSpectrometer->GetImages();
	TIter iter(imageMap);
	TObjString* lineName;
	while ((lineName = (TObjString*) iter.Next())) {
		TString slineName = lineName->String();
		
	}
	
}

//______________________________________________________________________________

void GMSAmandaClient::FillDataFile(const char * DBFileName) {
	
    //if(gGMSAnalyser->GetPrintLevel() > 0) 
	AliInfo(Form("Starting Filling. Input Setup File Name: %s",DBFileName));
	int Bloc, Line;

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
    if(!(fp = fopen(DBFileName,"r"))) AliFatal(Form("Enable to open Setup file %s",DBFileName));

    char line[256];
    while ( fgets ( line, sizeof(line), fp ) != NULL ) {
        char cdump;
		char sdump[10];
        switch ( line [0] ) {
        case '#': // Line commented
            break;
        case '>': // Number total of Bloc
            break;
//         case 'B': // BlocLMSBCAM
// 			fNBlocLMS++;
// 	    	sscanf ( line, "%c %d %d", &cdump, &iBloc, &NbLines);
// 	    	for( int i=0;i<NbLines ;i++ ){
// 				fscanf(fp,"%d %d %d %d %s %d %d %d %s "
// 					,&iLine
// 					,&Ch1,&Plat1,&Place1,&BCAMID1
// 					,&Ch2,&Plat2,&Place2,&BCAMID2);
// 				// Create the Boxes
// 				fBox[n+i] = new BoxBCAM(Ch1,Plat1,Place1,BCAMID1);
// 				fBox[n+i]->CreateVolume(GetChamberByID(Ch1),kBlue);
// 				fBox[n+i+NbLines] = new BoxBCAM(Ch2,Plat2,Place2,BCAMID2);
// 				fBox[n+i+NbLines]->CreateVolume(GetChamberByID(Ch2),kBlack);
// 				if(gGMSAnalyser->GetPrintLevel() > 0) Info("Spectro::CreateBlocs","Ligne %d created",i);
// 	    	}
// 			// Create the Bloc
// 			fBloc[m] = new BlocLMSBCAM(NbLines,
//                                     GetChamberByID(Ch1),&(fBox[n]),
//                                     GetChamberByID(Ch2),&(fBox[n+NbLines]));
// 			if(gGMSAnalyser->GetPrintLevel() > 0) Info("Spectro::CreateBlocs","Add blocs[%d] = %x",m,fBloc[m]);
// 			// Move courant pointers 
// 			n += 2*NbLines;
// 			m++;
//             break;
         case 'P': // BlocLMSPROX
 	    	sscanf ( line, "%c %d %d", &cdump, &iBloc, &NbLines);
 	    	for( int i=0;i<NbLines ;i++ ){
 				fscanf(fp,"%d %d %d %d %s %d %d %d %s "
 					,&iLine
 		       		,&Ch1,&Plat1,&Place1,&TubeID
 		       		,&Ch2,&Plat2,&Place2,&MaskID);

				TString * LineName = new TString(Form("LMSCH%02dCH%02dP%d",Ch1,Ch2,Place1/10));
				
				//if(gGMSAnalyser->GetPrintLevel() > 0) 
				AliInfo(Form("Looking for %s",LineName->Data()));
				
				Double_t PROXResults[5];
				if(FindDataPROX(LineName,PROXResults)) {
					AliInfo(Form("Data found for %s",LineName->Data()));
					for( int ires=0;ires<5 ;ires++ ) AliInfo(Form("\t%f",PROXResults[ires]));
					gMeasuredSpectrometer->FindLMSPROXLine(LineName, Bloc,Line);
					AliInfo(Form("%s : %d -> %d", LineName->Data(),Bloc, Line));
					((BlocLMSPROX*)gMeasuredSpectrometer->GetBloc(Bloc))->SetFinalImages(Line,PROXResults);
					((BlocLMSPROX*)gMeasuredSpectrometer->GetBloc(Bloc))->CalculateImageDisplacements(Line);
				}else AliError(Form("No Data found for %s",LineName->Data()));
				
				delete LineName;
			}
             break;
//         case 'W': // BlocEMS
// 			fNBlocEMS++;
// 			sscanf ( line, "%c %d %d", &cdump, &iBloc, &NbLines);
// 	    	for( int i=0;i<NbLines ;i++ ){
// 				fscanf(fp,"%d %d %d %d %s %d %d %d %s "
// 					,&iLine
// 					,&Ch1,&Plat1,&Place1,&BCAMID1 		// On the chamber
// 					,&Ch2,&Plat2,&Place2,&BCAMID2);		// On the Wall
// 				// Create the Boxes
// 				fBox[n+i] = new BoxBCAM(Ch1,Plat1,Place1,BCAMID1);
// 				fBox[n+i]->CreateVolume(GetChamberByID(Ch1),kBlue);
// 				fBox[n+i+NbLines] = new BoxBCAM(Ch2,Plat2,Place2,BCAMID2);
// 				fBox[n+i+NbLines]->CreateVolume(GetWall(Ch2),kBlack);
// 			}
// 				// Create the Bloc
// 			fBloc[m] = new BlocEMS(NbLines,GetChamberByID(Ch1),&(fBox[n]),GetWall(Ch2),&(fBox[n+NbLines]));
// 			// Move courant pointers 
// 			n += 2*NbLines; 
// 			m++;
//             break;
        case 'T': // BlocTMS
			sscanf ( line, "%c %d %d", &cdump, &iBloc, &NbLines);
	    	for( int i=0;i<NbLines ;i++ ){
				fscanf(fp,"%d %d %d %d %s %d %d %d %s \n"
					,&iLine
					,&Ch1,&Plat1,&Place1,&BCAMID1 		// On the chamber
					,&Ch2,&Plat2,&Place2,&BCAMID2);		// On the Wall
				
				TString * LineName = new TString(Form("TMSCH%02dP%dP%d",Ch1,Place1/10,Place2/10));
				
				//if(gGMSAnalyser->GetPrintLevel() > 0) 
					AliInfo(Form("Looking for %s",LineName->Data()));
				
				
				Double_t BCAMResults[4];
				if(FindDataBCAM(LineName,BCAMResults)) {
					AliInfo(Form("Data found for %s",LineName->Data()));
					for( int ires=0;ires<4 ;ires++ ) AliInfo(Form("\t%f",BCAMResults[ires]));
					gMeasuredSpectrometer->FindTMSLine(LineName, Bloc,Line);
					AliInfo(Form("%s : %d -> %d", LineName->Data(),Bloc, Line));
					((BlocTMS*)gMeasuredSpectrometer->GetBloc(Bloc))->SetFinalImages(Line,BCAMResults);
					((BlocTMS*)gMeasuredSpectrometer->GetBloc(Bloc))->CalculateImageDisplacements(Line);
				}else AliError(Form("NO Data found for %s",LineName->Data()));
				
				delete LineName;
			}
            break;

        }
    }
    fclose ( fp );

  
}

