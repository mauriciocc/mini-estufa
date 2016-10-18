#pragma once

void syncPlant(WORD selectedPlant, HWND dlg);
char* plantName(WORD plantId);
char* retrieveDateTime(HWND hwnd);
int processSelectEvent(int event_id);
void setPwmStatus(HWND dlg, WORD status, WORD element);
void setTimeInfo(HWND dlg, WORD time);
char* sensorName(char sensor) ;