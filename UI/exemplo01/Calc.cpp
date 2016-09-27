#include "stdafx.h"
#include "resource.h"
#include "Calc.h"

Calc* createCalc() {
	return calcClearState(
		(Calc*) calloc(1, sizeof(Calc))
	);
}

Calc* calcClearState(Calc* calcState){
	calcState->currentVal = CALC_NO_VAL;		
	calcState->last_op = 0;
	calcState->op_queue = 0;
	calcState->isNew = FALSE;
	return calcState;
}

int calcDoOp(Calc* calcState, double value, int op) {
	double currentValue = calcState->currentVal;
	int opResult = 0;
	switch(op) {
		case B_ADD:
			calcState->currentVal = currentValue + value;
			break;
		case B_SUB:
			calcState->currentVal = currentValue - value;
			break;
		case B_MUL:
			calcState->currentVal = currentValue * value;
			break;
		case B_EQ:
			if(calcState->last_op == 0) {
				calcState->currentVal = value;
			} else {				
				calcDoOp(calcState, value, calcState->last_op);			
			}
			break;
		case B_DIV:
			if(value != 0) {
				calcState->currentVal = currentValue / value;
			} else {
				opResult = -1;
			}
			break;
		case B_PERC:			
			calcState->currentVal = currentValue * (value/100);
			break;
		case B_DESC: {				
				calcState->currentVal -= value;
			}
			break;
	}

	return opResult;
}