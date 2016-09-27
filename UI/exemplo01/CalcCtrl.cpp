#include "stdafx.h"
#include "resource.h"
#include "Calc.h"
#include "CalcCtrl.h"
#include "StrUtils.h"
#include "Nf.h"
#include "Balance.h"

#define INPUT_BUFFER_SIZE 256
#define NF_BUFFER_SIZE 65536
#define NEW_LINE "\r\n"
#define NF_HEADER ,0
#define VAL_FORMAT "%.2f"

char* opChar(int op) {
	switch (op)
	{
	case NULL:
		return " ";
	case B_ADD:
		return " +";
	case B_DIV:
		return " /";
	case B_MUL:
		return " *";
	case B_SUB:
		return " -";
	case B_PERC:
		return " %";
	case B_DESC:
		return " Desconto";
	case B_EQ:
		return " =";
	default:
		return "??? ";
	}	
}

void appendOpToNf(HWND hDlg, int op, char* value) {
	char* strBuffer = (char*) malloc(NF_BUFFER_SIZE);	

	GetDlgItemTextA(hDlg, F_NF, strBuffer, NF_BUFFER_SIZE);
	if (strlen(strBuffer) > 0) {
		strcat(strBuffer, NEW_LINE);
	}	
	
	strcat(strBuffer, value);
	strcat(strBuffer, opChar(op));

	SetDlgItemTextA(hDlg, F_NF, strBuffer);	

	free(strBuffer);
}

void emitNfAction(HWND hDlg, Calc* calcState) {
	char* strBuffer = (char*) calloc(1, NF_BUFFER_SIZE);
	GetDlgItemTextA(hDlg, F_NF, strBuffer, NF_BUFFER_SIZE);
	strBuffer = (char*) realloc(strBuffer, NF_BUFFER_SIZE + 4096);
	char* header = 
		"WALVATES IND E COM DE SOFTWARE S/A\n"
		"Rua Sem Nome, 123, Bairro Teste\n"
		"CNPJ: 74.410.743/0001-76\n"
		"---------------------------------\n\0";
	prepend(header, strBuffer);
	sprintf(strBuffer+strlen(strBuffer), "\n%sTOTAL: R$ %.2f", "---------------------------------\n", calcState->currentVal);	
	emitNf(strBuffer);
	free(strBuffer);
}

// Return NULL if dont match any value
char* buttonOp(int event_id, char* strBuffer, BOOL clear) {
	char* result = "";

	switch(event_id) {

	case B_NUM_0:
		result = "0";		
		break;
	case B_NUM_1:
		result = "1";
		break;	
	case B_NUM_2:
		result = "2";
		break;
	case B_NUM_3:
		result = "3";
		break;
	case B_NUM_4:
		result = "4";
		break;
	case B_NUM_5:
		result = "5";
		break;
	case B_NUM_6:
		result = "6";
		break;
	case B_NUM_7:
		result = "7";
		break;
	case B_NUM_8:
		result = "8";
		break;
	case B_NUM_9:
		result = "9";
		break;
	case B_COMMA:		
		if(strlen(strBuffer) > 0){
			result = ".";		
		}
		break;
	}

	if(clear || (strcmp(strBuffer, "0") == 0 && strchr(result, '.') == NULL)) {
			memset(strBuffer, 0, INPUT_BUFFER_SIZE);
	}
		
	if(strchr(strBuffer, '.') == NULL || strchr(result, '.') == NULL) { 		
		strcat_s(strBuffer, INPUT_BUFFER_SIZE, result);
	}
	
	return result;

}

double castToDouble(char* val) {
	int len = strlen(val);
	for(int i = 0; i < len; i++) {
		if(val[i] == ',') {
			val[i] = '.';
		}
	}
	return atof(val);
}

void calcCtrlHandleEvent(HWND hDlg, int event_id, Calc* calcState) {
	if(event_id == B_EMIT_NF) {
		emitNfAction(hDlg, calcState);
		event_id = B_CLR;		
	} else if(event_id == B_CE) {
		SetDlgItemTextA(hDlg, F_VALUE_1, "0");
		return;
	} 
	
	
	double value;
	char* strBuffer = (char*) malloc(INPUT_BUFFER_SIZE);
	char fieldVal[256];

	if(event_id == B_BALANCE_READ) {
		BalanceData* data = readBalance();
		double peso = castToDouble(data->peso);
		double precoKg = castToDouble(data->precoKg);
		value = castToDouble(data->vlrTotal);

		char appendText[512];
		sprintf_s(appendText, "Peso: %.2f, Preço/Kg: %.2f", peso, precoKg);		
		appendOpToNf(hDlg, NULL, appendText);

		sprintf(fieldVal, VAL_FORMAT, value);
		SetDlgItemTextA(hDlg, F_VALUE_1, fieldVal);		
		free(data);
		return;
	} else {
				
		GetDlgItemTextA(hDlg, F_VALUE_1, strBuffer, INPUT_BUFFER_SIZE);

		if(event_id != B_MS && strlen(buttonOp(event_id, strBuffer, calcState->isNew)) > 0) {
			SetDlgItemTextA(hDlg, F_VALUE_1, strBuffer);
			free(strBuffer);
			calcState->isNew = FALSE;
			return;
		}	
		
		value = atof(strBuffer);
	}

	switch(event_id) {
		case B_MC:
			calcState->memory = 0;
			SetDlgItemTextA(hDlg, F_MEMORY, NULL);
		break;
						
		case B_MR:
			sprintf(fieldVal, VAL_FORMAT, calcState->memory);			
			SetDlgItemTextA(hDlg, F_VALUE_1, fieldVal);
		break;
						
		case B_MS:
			calcState->memory = value;
			sprintf(fieldVal, VAL_FORMAT, value);
			SetDlgItemTextA(hDlg, F_MEMORY, fieldVal);
		break;

		case B_CLR:
			calcClearState(calcState);
			SetDlgItemTextA(hDlg, F_NF, NULL);			
			SetDlgItemTextA(hDlg, F_RESULT, NULL);
			SetDlgItemTextA(hDlg, F_VALUE_1, "0");
		break;

		default:						
	
			if(strlen(strBuffer) == 0 || calcState->isNew) {
				free(strBuffer);
				return;
			}

			if(calcState->currentVal == CALC_NO_VAL) {			
				calcState->currentVal = value;				
				calcState->op_queue = event_id;				
			} else {
				int op = calcState->op_queue;
				calcState->op_queue = event_id;
				calcDoOp(calcState, value, op);
			}

			calcState->isNew = TRUE;
						
			sprintf(fieldVal, VAL_FORMAT, calcState->currentVal);
			SetDlgItemTextA(hDlg, F_VALUE_1, fieldVal);
			
			sprintf(strBuffer, "%.2f", value);		

			int register_id = event_id;
			if(register_id == B_EQ && calcState->last_op != 0) {
				register_id = calcState->last_op;
			}
			appendOpToNf(hDlg, register_id, strBuffer);
			sprintf(strBuffer, "%.2f", calcState->currentVal);				
			SetDlgItemTextA(hDlg, F_RESULT, strBuffer);			
	}

	if(event_id != B_EQ) {
		calcState->last_op = event_id;
	}

	free(strBuffer);						
}

void initializeView(HWND hDlg, Calc* state) {
	calcCtrlHandleEvent(hDlg, B_CLR, state);
}