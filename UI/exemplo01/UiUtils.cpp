#include "stdafx.h"
#include "resource.h"
#include "UiUtils.h"


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

int processSelectEvent(int event_id) {
	for(int i = 0; i < ARRAYSIZE(plants); i++) {
		if(plants[i] == event_id) {
			currentPlant = i;
			return 1;
		}
	}
	return 0;
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