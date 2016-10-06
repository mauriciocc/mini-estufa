#pragma once

#define PROT_WRITE 27
#define PROT_READ  72

#define PROT_T_TEMP     1
#define PROT_T_LUX      2
#define PROT_T_PLANT    4
#define PROT_T_STATUS   8
#define PROT_T_TIME     16
#define PROT_T_HISTORY  64

typedef struct ProtocolData {
	char header;
	char type;
	short size;
	char* data;
	short checksum;
} ProtocolData;

typedef struct History {
	char plant;
	unsigned long ocurrence_time;
	short size;
	char* message;
} History;

int protocolReadTemp(char* port);
int protocolReadLux(char* port);
int protocolReadPlant(char* port);
int protocolWritePlant(char* port, char plant);
unsigned long protocolReadTime(char* port);
void protocolWriteTime(char* port, unsigned long newTime);
History* readHistorical(char* port);