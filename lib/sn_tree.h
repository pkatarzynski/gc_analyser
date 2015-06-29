#ifndef sn_tree_H
#define sn_tree_H

// Settings
//-----------------------------------------------------------------
#define MAX_CNT 255 // top tree item capacity 
#define MAX_DIM 8 // maximal dimension
// Includes
//-----------------------------------------------------------------
#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <math.h>
#include <string.h>

#include "utils.h"
#include "const.h"
//-----------------------------------------------------------------

//Exception codes
//-----------------------------------------------------------------
#define stUNDEFINED 0
#define stINVALID_INDEX 1
#define stCOPY_MEM_FAULT 2
#define stTREE_TOO_BIG 3
//-----------------------------------------------------------------
class sn_tree_exception
{
    sn_tree_exception(const char *msg);
public:
    unsigned char code;
    void Display();
};
//-----------------------------------------------------------------
class sn_tree
{
public:

	void Print(FILE *dst = stdout);
	void BinaryPrint(FILE *dst);
	void PlainPrint(char operator_chr='s', char variable_chr='x');
	unsigned int ToString(char *str,unsigned int str_size,char operator_chr='s', char variable_chr='x');
	void Init();
	void Destroy();

	unsigned char GetItem(unsigned char ind);
        unsigned int GetRank();
        unsigned int GetDimRankPattern();
        unsigned int GetDim();
	unsigned int GetCnt();
        unsigned char *GetData();
        void SetRank(unsigned int new_rank, unsigned char new_dimension=1);
        void SetDim(unsigned char new_dimension);
        void SetDRP(unsigned int new_drp);
        int SetRankFromDRP();

	void SetMult(char new_mult);
	char GetMult();
	unsigned short GetSum();

	void Clean();
        void AddItem(char item);

	void CopyFrom(sn_tree *src);
	void Multiply(sn_tree *src);
	void Multiply(char a);
	unsigned short ReduceWith(sn_tree *src);
	void ParseFromString(char *str);
	void LoadFromStream(FILE *fil);

private:

	void RaiseException(unsigned char ex_code);
	
	unsigned short isdigit(char c);

	char mult;	        
        unsigned char rank;
        unsigned char dim;
        unsigned int drp; //dimension-rank-pattern
	unsigned char cnt;
	unsigned char *data;
	unsigned short sum;

	sn_tree_exception E;
};
#endif
//THE END
//-----------------------------------------------------------------

