#include "stdafx.h"
#include "resource.h"
#include "UiUtils.h"
#include "Protocol.h"


char currentPlant = 0;
int plants[] = {
	RB_ASPLENIO, 
	RB_AZALEIA,
	RB_BROMELIA,
	RB_CAMEDOREA,
	RB_CAMELIA,
	RB_DRACENAS,
	RB_SAMAMBAIA
};

char* plantName(WORD plantId) {
	switch(plantId) {
	case 0: return "Asplenio";
	case 1: return "Azaleia";
	case 2: return "Bromelia";
	case 3: return "Camedorea";
	case 4: return "Camelia";
	case 5: return "Dracenas";
	case 6: return "Samambaia";
	}
}

char* sensorName(char sensor) {
	switch(sensor) {
		case LM35:
			return "Temperatura";
		case LDR:
			return "Luminosidade";
		default:
			return "-";
	}
}

void syncPlant(WORD selectedPlant, HWND dlg) {
	currentPlant = selectedPlant;
	for(int i = 0; i < ARRAYSIZE(plants); i++) {
		BOOLEAN val = 0;
		if(i == selectedPlant) {
			val = 1;
		}
		CheckDlgButton(dlg, plants[i], val);
	}
	
}

int processSelectEvent(int event_id) {
	for(int i = 0; i < ARRAYSIZE(plants); i++) {
		if(plants[i] == event_id) {
			currentPlant = i;
			return i;
		}
	}
	return -1;
}

char* retrieveDateTime(HWND hwnd) {
	char date[32] = {0};					
	GetDlgItemTextA(hwnd, F_DATE, date, 30);

	char time[32] = {0};
	GetDlgItemTextA(hwnd, F_TIME, time, 30);

	char* result = (char*) calloc(64, 1);
	sprintf(result, "%s %s", &date, &time);

	return result;
}

void setPwmStatus(HWND dlg, WORD status, WORD element) {	
	double steps = HIBYTE(status) - 1;
	double currentStep = LOBYTE(status);
	double perc = (currentStep/steps)*100;
	char buf[128];
	sprintf(buf, "%0.2f%%", perc);
	SetDlgItemTextA(dlg, element, buf);
}

void setTimeInfo(HWND dlg, WORD time) {
	char buf[128];
	sprintf(buf, "%.2d:%.2d", HIBYTE(time), LOBYTE(time));
	SetDlgItemTextA(dlg, F_ARD_TIME, buf);

}

void readAndUpdateScreen(HWND hwnd, char* port){
	SetDlgItemInt(hwnd, F_TEMP, protocolReadVar(port, PROT_T_TEMP, NULL), FALSE);
	SetDlgItemInt(hwnd, F_LUX, protocolReadVar(port, PROT_T_LUX, NULL), FALSE);

	word currentPlant = protocolReadVar(port, PROT_T_PLANT, NULL);
	SetDlgItemTextA(hwnd, F_DISPLAY_PLANT, plantName(currentPlant));					
	syncPlant(currentPlant, hwnd);

	setTimeInfo(hwnd, protocolReadVar(port, PROT_T_TIME, NULL));
										
	setPwmStatus(hwnd, protocolReadVar(port, PROT_T_STATUS, LM35), F_FAN);
	setPwmStatus(hwnd, protocolReadVar(port, PROT_T_STATUS, LDR), F_LED);

	word result = protocolReadVar(port, PROT_T_STATUS, LOG_ID);

	// LOGS
	WORD size = 0;
	IncidentLog* logs = protocolReadLogs(port, &size);
	if(size > 0) {
		char* message = (char*)malloc(size*128);
		strcpy(message, "");
		char buf[1024];
		for(int i = size - 1; i >= 0; i--) {
			sprintf(buf, "%.3d - %.2d:%.2d - %s - %s - %s \r\n", 
				i,
				logs[i].h, 
				logs[i].m, 
				plantName(logs[i].plant),
				sensorName(logs[i].sensor), 
				logs[i].bound ? "Acima do limite" : "Abaixo do limite");
			strcat(message, buf);
		}
		SetDlgItemTextA(hwnd, F_LOG_OUTPUT, message);
		free(message);
		free(logs);					
	} else {
		SetDlgItemTextA(hwnd, F_LOG_OUTPUT, "");
	}
}