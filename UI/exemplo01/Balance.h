#pragma once

typedef struct BalanceData {
	char ignore1[18];
	char peso[6];
	char ignore2[6];	
	char precoKg[7];
	char ignore3[6];
	char vlrTotal[7];
	char ignore4[6];	 
} BalanceData;

BalanceData* readBalance();
