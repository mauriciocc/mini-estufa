#include "stdafx.h"
#include "StrUtils.h"

void replace(char* str, char oldChar, char newChar) {
}



char* concat(char *s1, char *s2)
{
    char *result = (char*)malloc(strlen(s1)+strlen(s2)+1);//+1 for the zero-terminator
    //in real code you would check for errors in malloc here
    strcpy(result, s1);
    strcat(result, s2);
    return result;
}

void prepend(char* prepend, char* target) {

	
  int N = strlen(prepend) + strlen(target) ;
  char* temp = (char *) calloc(1, N+1) ;
  strcpy(temp, prepend);
  strcat(temp , target) ;
  strcpy(target, temp) ;
  free( temp ) ; 

	/*int size = strlen(prepend);
    memmove(target + size,target, size);
	memmove(target, prepend, size);*/
}