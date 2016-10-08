#include "stdafx.h"
#include "DateUtils.h"

using namespace std;

tm* parseDate(char* dateTimeString) {			

	tm* timeStruct = (tm*) calloc(1, sizeof(tm));
	sscanf(
		dateTimeString, 
		"%d/%d/%d %d:%d:%d", 
		&timeStruct->tm_mday, 
		&timeStruct->tm_mon, 
		&timeStruct->tm_year, 
		&timeStruct->tm_hour, 
		&timeStruct->tm_min, 
		&timeStruct->tm_sec
	);
	return timeStruct;
}
