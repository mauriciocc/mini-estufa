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
      return hComm;
  }

	DCB dcb;		
	dcb.DCBlength = sizeof(DCB);

	GetCommState(hComm, &dcb);	

	dcb.BaudRate = CBR_115200;
	dcb.ByteSize = 8;         
	dcb.Parity   = NOPARITY; 
	dcb.StopBits = ONESTOPBIT;

	SetCommState(hComm, &dcb);	

	COMMTIMEOUTS cto;
	GetCommTimeouts(hComm, &cto);
	cto.ReadIntervalTimeout = 2500;
	cto.ReadTotalTimeoutConstant = 500; 
	cto.ReadTotalTimeoutMultiplier = 200;
	cto.WriteTotalTimeoutConstant = 500;
	cto.WriteTotalTimeoutMultiplier = 200;
	SetCommTimeouts(hComm, &cto);

	return hComm;
}

word msgSize(ProtocolData* msg) {
	return PROT_SIZE + msg->size;
}

WORD toWord(BYTE b1, BYTE b2) {
  return (b2 << 8) | b1;
}

ProtocolData* parseMsg(HANDLE hnd) {
  BYTE bytes[PROT_SIZE]; 	

  DWORD NoBytesRead;
  ReadFile(hnd, &bytes, 4, &NoBytesRead, NULL);		 
  
  ProtocolData* data = (ProtocolData*) calloc(1, sizeof(ProtocolData));
  
  data->header = bytes[0];
  data->type = bytes[1];
  data->size = toWord(bytes[2], bytes[3]);
  
  WORD ptr = 4;
  if(data->size > 0) {    
    data->data = (BYTE*) calloc(1, data->size);
	ReadFile(hnd, data->data, data->size, &NoBytesRead, NULL); 	
  }  

  ReadFile(hnd, &bytes[ptr], 2, &NoBytesRead, NULL);  
  
  data->checksum = toWord(bytes[ptr], bytes[ptr+1]);

  return data;  
}

word receiveWord(HANDLE h) {
	byte msg[2];
	DWORD r;
	ReadFile(h, &msg, 2, &r, NULL); 	
	return toWord(msg[0], msg[1]);
}

void freeMsg(ProtocolData* pd) {
  if(pd->size > 0) {
    free(pd->data);   
  }
  free(pd);
}

BYTE* toBytes(ProtocolData* data) {
  WORD allocation = msgSize(data);
  BYTE* ser = (BYTE*) malloc(allocation);
  ser[0] = data->header;
  ser[1] = data->type;  
  ser[2] = LOBYTE(data->size);
  ser[3] = HIBYTE(data->size);
  int ptr = 4;
  // Corpo da msg
  int i = 0;
  if(data->size > 0) {
    for(; i < data->size; i++) {
      ser[i+ptr] = data->data[i];
    }
    ptr += data->size;
  }
  ser[ptr] = LOBYTE(data->checksum);
  ser[ptr+1] = HIBYTE(data->checksum);
  return ser;
}

void sendMsg(ProtocolData* data, HANDLE hnd) {
	BYTE* bytes = toBytes(data);  		

	DWORD NoBytesRead;	
	WriteFile(hnd, bytes, msgSize(data), &NoBytesRead, NULL);
		
	free(bytes);  
}


WORD calculateChecksum(ProtocolData* data) {
	WORD sum = 0;
	sum += data->header;
	sum += data->type;
	sum += data->size;
	BYTE* content = data->data;
	for(int i = 0; i < data->size; i++) {
		sum += content[i];
	}
	return sum;
}

ProtocolData* createMsg(BYTE header, BYTE type, char* msg, short size) {
	ProtocolData* data = (ProtocolData*) calloc(1, sizeof(ProtocolData));
	data->header = header;
	data->type = type;
	data->data = (BYTE*) msg;
	data->size = size;	
	data->checksum = calculateChecksum(data);
	return data;
}

word protocolReadVar(char* port, byte var, word value) {

	HANDLE h = openPort(port);

	char vals[1] = {NULL};
	short size = 0;
	if(value != NULL) {
		size = 1;
		vals[0] = value;
	}
	
	ProtocolData* data = createMsg(PROT_READ, var, vals, size);
	sendMsg(data, h);
	free(data);

	word val = receiveWord(h);	

	CloseHandle(h);

	return val;
}

void protocolWriteVar(char* port, byte var, word value) {
	HANDLE h = openPort(port);
		
	char vals[] = {LOBYTE(value), HIBYTE(value)};
	ProtocolData* data = createMsg(PROT_WRITE, var,  vals, 2);
	sendMsg(data, h);
	free(data);

	CloseHandle(h);
}

IncidentLog* protocolReadLogs(char* port, WORD* size) {
	HANDLE h = openPort(port);

	ProtocolData* data = createMsg(PROT_READ, PROT_T_HISTORY, NULL, 0);
	sendMsg(data, h);
	free(data);

	word rSize = receiveWord(h);	
	*size = rSize/sizeof(IncidentLog);
	
	IncidentLog* values = NULL;
	if(rSize > 0) {
		values = (IncidentLog*) malloc(rSize);	
		DWORD NoBytesRead;	
		ReadFile(h, values, rSize, &NoBytesRead, NULL);  
	}

	CloseHandle(h);	

	return values;
}

BOOLEAN isPortAvailable(char* port) {
	HANDLE h = openPort(port);
	BOOLEAN valid = h != INVALID_HANDLE_VALUE;
	if(valid) {
		CloseHandle(h);
	}
	return valid;
}