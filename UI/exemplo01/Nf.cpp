#include "stdafx.h"
#include "Nf.h"

#define NF_FILE "nf_output.txt"

int emitNf(char* content) {
	FILE* file;	
	fopen_s(&file, NF_FILE, "w+");	
	if(file == NULL) {
		return 1;
	}
	fwrite(content, 1, strlen(content), file);
	fclose(file);
	return 0;
}