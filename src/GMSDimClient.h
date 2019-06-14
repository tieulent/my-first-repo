#ifndef GMS_DIM_CLIENT_H
#define GMS_DIM_CLIENT_H

#include <dic.hxx>
#include <TObject.h>
#include <TTimeStamp.h>
typedef struct{
	Int_t Status;
	Int_t RefRunNumber;
	Int_t LastRunNumber;
	time_t RefStartTime;
	time_t RefStopTime;
	time_t LastStartTime;
	time_t LastStopTime;
}GMSDATA;

/**
@author Raphael Tieulent
*/
//!  Implement a DIM Client
/*!
This class implements an DIM Client which is used to trig the download of the 
data is at the end of a DCS Run. 
*/
class GMSDimClient: public DimInfo, public TObject {
public :
	GMSDimClient(char *name);
	~GMSDimClient();
private :
	virtual void infoHandler();
	void LoadData(GMSDATA * data);
	
	TString * fRequest;
	Int_t fStatus;
	Int_t fRefRunNumber;
	Int_t fLastRunNumber;
	TTimeStamp * fRefStartTime;
	TTimeStamp * fRefStopTime;
	TTimeStamp * fLastStartTime;
	TTimeStamp * fLastStopTime;

//	ClassDef(GMSDimClient, 0);

};

#endif
