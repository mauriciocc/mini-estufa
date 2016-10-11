#include "stdafx.h"
#include "Protocol.h"

HANDLE openPort(char* port) {

  HANDLE hComm = CreateFileA(
					  port, 
					  GENERIC_READ | GENERIC_WRITE, //Read
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
	dcb.StopBits = ONESTOPBIT;

	SetCommState(hComm, &dcb);	

	COMMTIMEOUTS cto;
	GetCommTimeouts(hComm, &cto);
	cto.ReadIntervalTimeout = 0;
	cto.ReadTotalTimeoutConstant = 0; 
	cto.ReadTotalTimeoutMultiplier = 0;
	cto.WriteTotalTimeoutConstant = 0;
	cto.WriteTotalTimeoutMultiplier = 0;
	SetCommTimeouts(hComm, &cto);

	/*size_t dataSize = sizeof(BalanceData);

	BalanceData* data = (BalanceData*) calloc(1, dataSize);
	DWORD NoBytesRead;	
	ReadFile(hComm, data, dataSize, &NoBytesRead, NULL);	*/

	return hComm;

	

  //return data;
}


unsigned short calculateChecksum(ProtocolData* data) {
	unsigned short sum = 0;
	sum += data->header;
	sum += data->type;
	sum += data->size;
	char* content = data->data;
	for(int i = 0; i < data->size; i++) {
		sum += content[i];
	}
	return sum;
}

int protocolReadTemp(char* port) {
		
	size_t size = sizeof(ProtocolData);

	ProtocolData* data = (ProtocolData*) calloc(1, size);
	data->header = PROT_READ;
	data->type = PROT_T_TEMP;
	data->data = "Hello World!";
	data->size = strlen(data->data);	
	data->checksum = calculateChecksum(data);

	HANDLE hnd = openPort(port);
	DWORD NoBytesRead;
	WriteFile(hnd, data, size, &NoBytesRead, NULL);	

	

	//Wait for response


	//ReadFile(hnd, &data, 6, &NoBytesRead, NULL);

	CloseHandle(hnd);
	free(data);



	return -1;
}