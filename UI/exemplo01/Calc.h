#pragma once

#define CALC_NO_VAL -9999

typedef struct Calc {
	double currentVal;
	double lastVal;
	double memory;
	int op_queue;
	int last_op;	
	BOOL isNew;
} Calc;

Calc* createCalc();

Calc* calcClearState(Calc* calcState);
int calcDoOp(Calc* calcState, double value, int op);