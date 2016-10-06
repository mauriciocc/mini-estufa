#include "stdafx.h"
#include "Protocol.h"

HANDLE openPort(char* port) {

  HANDLE hComm = CreateFile(
					(LPCTSTR) port, 
					  GENERIC_READ | GENERIC_WRITE, //Read
                      0,                            // No Sharing
                      NULL,                         // No Security
                      OPEN_EXISTING,// Open existing port only
                      NULL,            // Non Overlapped I/O
                      NULL);        // Null for Comm Devices

  if (hComm == INVALID_HANDLE_VALUE){
      //return NULL;
  }

	DCB dcb;		
	dcb.DCBlength = sizeof(DCB);

	GetCommState(hComm, &dcb);	

	dcb.BaudRate = CBR_9600;
	dcb.ByteSize = 8;         
	dcb.Parity   = NOPARITY; 
	dcb.StopBits = TWOSTOPBITS;

	SetCommState(hComm, &dcb);	

	COMMTIMEOUTS cto;
	GetCommTimeouts(hComm, &cto);
	cto.ReadIntervalTimeout = 10;
	cto.ReadTotalTimeoutConstant = 10; 
	cto.ReadTotalTimeoutMultiplier = 10;
	SetCommTimeouts(hComm, &cto);

	/*size_t dataSize = sizeof(BalanceData);

	BalanceData* data = (BalanceData*) calloc(1, dataSize);
	DWORD NoBytesRead;	
	ReadFile(hComm, data, dataSize, &NoBytesRead, NULL);	*/

	return hComm;

	

  //return data;
}



int protocolReadTemp(char* port) {
	HANDLE hnd = openPort(port);

	ProtocolData data;
	DWORD NoBytesRead;		
	ReadFile(hnd, &data, 6, &NoBytesRead, NULL);

	CloseHandle(hnd);
	return -1;
}