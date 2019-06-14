#include <TTimeStamp.h>
#include <TObjString.h>
#include <TStopwatch.h>
#include <TROOT.h>
#include <AliLog.h>
#include "GMSAmandaClient.h"
#include "GMSDimClient.h"
#include "GMSAnalyser.h"
#include "Spectro.h"
#include "ImageResult.h"

//ClassImp(GMSDimClient)
//-------------------------------------------------------------------------------------------------------------------------------------------

GMSDimClient::GMSDimClient(char *name) : DimInfo(name, -1.0), TObject() {
	FILE * fp;
	if(!(fp = fopen("/mnt/alidcsfs001/Scratch_FED/gms/fwLwdaq_Script.txt","r")))  AliFatal("LWDAQ Script File does not exist");
//	if(!(fp = fopen("/Users/tieulent/GMS/Scripts/fwLwdaq_Script.txt","r")))  AliFatal("LWDAQ Script File does not exist");
	
	char line[256];
	char BCAMElements[4][10];
	strcpy(BCAMElements[0],"/XLEFT,");
	strcpy(BCAMElements[1],"/YLEFT,");
	strcpy(BCAMElements[2],"/XRIGHT,");
	strcpy(BCAMElements[3],"/YRIGHT,");
	char PROXElements[6][10];
	strcpy(PROXElements[0],"/XPOS,");
	strcpy(PROXElements[1],"/YPOS,");
	strcpy(PROXElements[2],"/XMAG,");
	strcpy(PROXElements[3],"/YMAG,");
	strcpy(PROXElements[4],"/ROT,");
	strcpy(PROXElements[5],"/PREC,");
	TString Request;
	int iline = 0;
	
	while ( fgets ( line, sizeof(line), fp ) != NULL ) {
        char cdump[256];
		TString sLine = line;
		
		//  BCAMs case
		if(sLine.Contains("name: BCAM")){
			sLine.Remove(sLine.Last('_'),4);
			sLine.Remove(0,sLine.Last('-')+1);
			TString aliasbase(sLine);

			for(int ielement=0;ielement<4;ielement++){
				TString alias(aliasbase);
				alias+= BCAMElements[ielement];
				Request += alias;
			}
			iline++;
		}
		//  PROXs case
		if(sLine.Contains("name: Rasnik")){
			sLine.Remove(sLine.Last('_'),4);
			sLine.Remove(0,sLine.Last('-')+1);
			TString aliasbase(sLine);
			
			for(int ielement=0;ielement<6;ielement++){
				TString alias(aliasbase);
				alias+= PROXElements[ielement];
				Request += alias;
			}
			iline++;
		}
	}
	Request.Remove(Request.Length()-1,1);
	fRequest = new TString(Request);

	fclose(fp);
	
};
GMSDimClient::~GMSDimClient() {
	delete fRequest;
};
//-------------------------------------------------------------------------------------------------------------------------------------------
void GMSDimClient::infoHandler()
	{
		sleep(1);
		GMSDATA *data = (GMSDATA *)getData();
		LoadData(data);

		AliInfo(" --------------------------------------------------------");
		AliInfo("\t New GMSDimClient :"); 
		AliInfo(" --------------------------------------------------------");
		AliInfo(Form("\tStatus : %d", fStatus));
		AliInfo(Form("\tReference Run Number : %d", fRefRunNumber));
		AliInfo(Form("\t\tStart : %s", fRefStartTime->AsString()));
		AliInfo(Form("\t\tStop  : %s", fRefStopTime->AsString()));
		AliInfo(Form("\tLast Run Number %d ", fLastRunNumber));
		AliInfo(Form("\t\tStart : %s", fLastStartTime->AsString()));
		AliInfo(Form("\t\tStop  : %s", fLastStopTime->AsString()));
		AliInfo(Form("ORACLE Shift : Start %d  End %d  ",fLastStartTime->GetSec(),fLastStopTime->GetSec()) );
		
		
		TStopwatch  * Timer = new TStopwatch();
		Timer->Start();

		GMSAmandaClient * AmandaClient = new GMSAmandaClient("alidcsamanda.cern.ch",1337
			,fRequest ,fLastStartTime->GetSec(), fLastStopTime->GetSec()  );
		
		
		// Retrieve the Data from the ORACLE database
		AmandaClient->GetDCSValues();

		//AliInfo(Form("Setup File Name: %s" ,gGMSAnalyser->GetBlocDBFileName()));
		//AmandaClient->FillDataFile(gGMSAnalyser->GetBlocDBFileName());
		AmandaClient->FillDataMap();
		
		TMap * imageMap = gMeasuredSpectrometer->GetImages();
		TIter iter(imageMap);
		TObjString* lineName;
		while ((lineName = (TObjString*) iter.Next())) {
			((ImageResult *)imageMap->GetValue(lineName))->Print();
		}
		
		
		Timer->Stop();
		Timer->Print("m");
		delete Timer;
	}

//-------------------------------------------------------------------------------------------------------------------------------------------

void GMSDimClient::LoadData(GMSDATA * data)
	{
		fStatus =data->Status	   ; 
		fRefRunNumber = data->RefRunNumber ;
		fRefStartTime = new TTimeStamp(data->RefStartTime,0) ;
 		fRefStopTime = new TTimeStamp(data->RefStopTime,0) ;
  		fLastRunNumber = data->LastRunNumber;
 		fLastStartTime = new TTimeStamp(data->LastStartTime,0);
 		printf("\t\tStart %d  %d\n",data->LastStartTime,fLastStartTime->GetSec()); 
		printf("\t\tStop  %d\n",data->LastStopTime);
 		fLastStopTime = new TTimeStamp(data->LastStopTime,0) ;
	}
