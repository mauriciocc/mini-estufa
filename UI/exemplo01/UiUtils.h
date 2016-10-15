#pragma once

void syncPlant(WORD selectedPlant, HWND dlg);
char* plantName(WORD plantId);
char* retrieveDateTime(HWND hwnd);
int processSelectEvent(int event_id);