#ifndef UTILS_H
#define UTILS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "const.h"

#define DIM_BITS 4
#define MAX_DIM 8


class gcError
{
public: 
 gcError(const char *new_msg,const char *p1=NULL);
 char *msg;
};

int gc_FileExists(const char *filename);


int gc_feoln(FILE *f);
int gc_fopen(FILE **fil, const char *filename, const char *mode);
int gc_unlink(char *filename);

int gc_itoa(int val,char *buf,int radix);

int gc_strcat(char *dest, int len, char *src);
int gc_strncat(char *dest, int dest_len, char *src, int src_len);
int gc_strcpy(char *dest,int len,char *src);
int gc_strncpy(char *dest,int dest_len,char *src,int src_len);

void CenterStr(char *str, int width);

//-----------------------------------------------------------------
class ErrorClass
{
public:
	int type;
	void *wparam;
	ErrorClass(int,void *);
	void PrintMessage();
};

#endif
