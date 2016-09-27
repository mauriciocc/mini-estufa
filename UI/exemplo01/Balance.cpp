#include "stdafx.h"
#include "Balance.h"

BalanceData* readBalance() {

  HANDLE hComm = CreateFile(_T("COM5"), 
					  GENERIC_READ, //Read
                      0,                            // No Sharing
                      NULL,                         // No Security
                      OPEN_EXISTING,// Open existing port only
                      NULL,            // Non Overlapped I/O
                      NULL);        // Null for Comm Devices

  if (hComm == INVALID_HANDLE_VALUE){
      return NULL;
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
	cto.ReadTotalTimeoutConstant = 0;
	cto.ReadTotalTimeoutMultiplier = 0;
	SetCommTimeouts(hComm, &cto);

	size_t dataSize = sizeof(BalanceData);

	BalanceData* data = (BalanceData*) calloc(1, dataSize);
	DWORD NoBytesRead;	
	ReadFile(hComm, data, dataSize, &NoBytesRead, NULL);	

	CloseHandle(hComm);

  return data;
}