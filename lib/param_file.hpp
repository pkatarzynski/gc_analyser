#ifndef param_file_HPP
#define param_file_HPP

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "utils.h"
//-----------------------------------------------------------------
//Settings
//-----------------------------------------------------------------

#define BUFFER_LEN 200
#define VERSION_NUMBER 20100923
#define MAX_ENTRIES 200

//-----------------------------------------------------------------
//Definitions of parameter types
//-----------------------------------------------------------------

#define ptUnknown 0
#define ptInt 1
#define ptString 2
#define ptFloat 3
#define ptIntVector 4
#define ptFloatVector 5
#define ptDouble 6
#define ptDoubleVector 7

#define ptMaxInd 7

//-----------------------------------------------------------------
//ParamEntry structure 
//-----------------------------------------------------------------
struct ParamEntry
{
 char type; 
 int length;
 char label[20];
 void *data;
};
//-----------------------------------------------------------------
// CParamFile Class
//-----------------------------------------------------------------
class CParamFile
{

// Public
//-----------------------------------------------------------------
public:

CParamFile(); 		// default constructor 
~CParamFile(); 		// default destructor

void Clear();

void Put_i(const char *label,int data);                   // put integer
void Put_iv(const char *label,int cnt, int *data); 	// put integer vector
void Put_f(const char *label,float data);			// put float
void Put_fv(const char *label,int cnt, float *data);       // put float vector
void Put_d(const char *label, double data);               // put double
void Put_dv(char *label,int cnt, double *data);      // put double vector
void Put_s(const char *label, char *data);                // put string

int Get_i(const char *label,int *data,int mandatory);                   // get integer
int Get_s(const char *label,char **data,int mandatory);			// get string
int Get_f(const char *label,float *data,int mandatory);			// get float
int Get_d(const char *label, double *data,int mandatory);               // get double
int Get_dv(const char *label,int *cnt, double **data,int mandatory);     // get double vector
int Get_fv(const char *label,int *cnt, float **data,int mandatory);	// get float vector
int Get_iv(const char *label,int *cnt, int **data,int mandatory);	// get int vector

void SaveToFile( const char *filename);
int LoadFromFile(const  char *filename);

void Display();								// display the dataset

// Private
//-----------------------------------------------------------------

private:

// Protected
//-----------------------------------------------------------------
protected:

void SwapSpaces(char *buf);
int FindEntry(const char *label,int mandatory);

ParamEntry *AddEntry(const char *label,int count);
ParamEntry Entries[MAX_ENTRIES];
unsigned lEntries;


};

#endif
//-----------------------------------------------------------------
// The End
//-----------------------------------------------------------------
