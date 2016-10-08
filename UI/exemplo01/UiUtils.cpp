#include "stdafx.h"
#include "resource.h"
#include "UiUtils.h"



char* retrieveDateTime(HWND hwnd) {
	char date[32] = {0};					
	GetDlgItemTextA(hwnd, F_DATE, date, 30);

	char time[32] = {0};
	GetDlgItemTextA(hwnd, F_TIME, time, 30);

	char* result = (char*) calloc(64, 1);
	sprintf(result, "%s %s", &date, &time);

	return result;
}