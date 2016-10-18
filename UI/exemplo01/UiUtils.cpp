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
			return "TEMPERATURA";
		case LDR:
			return "LUMINOSIDADE";
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