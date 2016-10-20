#pragma once

#define LOG_ID 99

#define LM35 1
#define LDR 2

#define PROT_SIZE 6
#define PROT_WRITE 27
#define PROT_READ  72

#define PROT_T_TEMP     1
#define PROT_T_LUX      2
#define PROT_T_PLANT    4
#define PROT_T_STATUS   8
#define PROT_T_TIME     16
#define PROT_T_HISTORY  64

typedef unsigned char byte;
typedef unsigned short word;

typedef struct ProtocolData {
  BYTE header;
  BYTE type;
  WORD size;
  BYTE* data;
  WORD checksum;
} ProtocolData;

typedef struct IncidentLog {
  byte plant;
  byte h;
  byte m;
  byte sensor;
  byte bound;    
} IncidentLog;

HANDLE openPort(char* port);
BOOLEAN isPortAvailable(char* port);
WORD toWord(BYTE b1, BYTE b2);
word protocolReadVar(HANDLE port, byte var, word value);
void protocolWriteVar(HANDLE port, byte var, word value);
IncidentLog* protocolReadLogs(HANDLE port, WORD* size);