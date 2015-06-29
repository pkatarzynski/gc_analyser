#ifndef CTFF_H
#define CTFF_H

#define MAX_RANK 100
//#define MAX_GATES 2

#include "param_file.hpp"
#include "sn.h"
#include "utils.h"
#include <math.h>
#include <stdlib.h>
#include <stdio.h>

#ifndef MAX_FILE_PATH
#define MAX_FILE_PATH 1024
#endif

struct sn_entry
{
    char name[200];
    char in[200];
    char out[200];
    TSNType type;
    int rank;
    int width;
};

struct FilterDesign
{
    double Ap;
    double As;
    double wp;
    double ws;
    double scale;
    int type;
};


class CTff
{
public:
    CTff();

    void SetCoeff(int p,int q,double value);
    double GetCoeff(int p,int q);

    void SetBlank(int p,int q,int value);

    sn_entry *GetEntry(int ind);
    int GetSNCnt();
    int GetSNOrder();

    char *GetTFFFile();
    char *GetSNFile();


    int Clear();

    int LoadFromFile( char *new_tff_file, char *new_sn_file);
    void SaveToFile(const char *tff_file);
    void MakePNFile(const char *pn_file);
    void ClearParams();
    void Display(FILE *fil = stdout);

        FilterDesign Design;
        FilterDesign NewDesign;
        bool DesignChanged;


protected:
    char sn_file[MAX_FILE_PATH];
    char tff_file[MAX_FILE_PATH];
    double params[100][20];
    int blanks[100][20];
    int ScanSN(const char *new_sn_file);    
    sn_entry *items;
    int sn_rank;
    int sn_cnt;
};

#endif // CTFF_H
