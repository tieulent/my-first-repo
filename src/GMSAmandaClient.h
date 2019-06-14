#ifndef GMS_AMANDA_CLIENT_H
#define GMS_AMANDA_CLIENT_H


#include <TMap.h>
#include <TString.h>
#include "AliDCSClient.h"

/**
@author Raphael Tieulent
*/
//!  Implement an AMANDA Client
/*!
This class implements an AMANDA Client which is used to retrieve GMS Raw Data 
from the ALICE DCS Data Base.
The download of the data is triggered at the end of a DCS Run using a DIM 
Server-Client connection. The DIM Client is implement in the Class GMSDimClient.
*/
class GMSAmandaClient: public AliDCSClient {
public :
	GMSAmandaClient(char *HostName, Int_t Port);
	GMSAmandaClient(char *HostName, Int_t Port, TString * Request);
	GMSAmandaClient(char *HostName, Int_t Port, TString * Request, UInt_t StartTime, UInt_t EndTime);
	~GMSAmandaClient();
	inline char * GetServerHost(){return fServerHost;};
	inline Int_t  GetServerPort(){return fServerPort;};
	inline TString * GetRequest()   {return fRequest;};
	inline UInt_t GetStartTime(){return fStartTime;};
	inline UInt_t GetEndTime()  {return fEndTime;};
	inline void SetServerHost(char *ServerHost) {fServerHost=ServerHost;};
	inline void SetServerPort(Int_t ServerPort) {fServerPort=ServerPort;};
	inline void SetRequest	 (TString *Request) 	{fRequest = Request;};
	inline void SetStartTime(UInt_t StartTime)	{fStartTime =StartTime;};
	inline void SetEndTime  (UInt_t EndTime) 	{fEndTime   =EndTime;};

	void GetDCSValues();
	void FillDataFile(const char * DBFileName);
	void FillDataMap();
	Int_t FindDataBCAM(TString * LineName, Double_t Results[4]);
	Int_t FindDataPROX(TString * LineName, Double_t Results[5]);
		
private :
	
	char * fServerHost;
	Int_t fServerPort;
	TString * fRequest;
	UInt_t fStartTime;
	UInt_t fEndTime;
	TMap * fValues;
	
//	ClassDef(GMSAmandaClient, 0);

	
};
#endif

