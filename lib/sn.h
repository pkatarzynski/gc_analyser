#ifndef SN_H
#define SN_H

// Includes
//-----------------------------------------------------------------
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

#include "sn_tree.h"
#include "utils.h"

// Settings
//-----------------------------------------------------------------
#define snDEBUG
#define snMAX_RANK 100
#define snMAX_TREE_CNT 1000000  // top number of trees
#define snMAX_LINE_LEN 2048
#define WIN

// global flag indices used for reading sn parametres from file
//-----------------------------------------------------------------
#define SN_FLAGS_CNT 10
#define SN_BEGIN 0
#define SN_END 1
#define SN_CURRENT_DRP 2
#define SN_RANK 3
#define SN_COUNT 4
#define SN_DIM 5
#define SN_DRP 6

// Exception codes definitions
//-----------------------------------------------------------------
#define snUNDEFINED 0
#define snMEM_STORAGE 1
#define snSELF_MULTIPLY 2
#define snSELF_ADD 3
#define snIO_ERROR 4
#define snTOO_MANY_TREES 5
#define snFILE_CORRUPTED 6
#define snSTORAGE_ERROR 7


enum TSNType {snAny,snNum,snDen};

// Exception class
//-----------------------------------------------------------------
class SN_Exception
{
public:
		void Show();
		unsigned char code;
private:

};
//-----------------------------------------------------------------

unsigned int gc_CreateDimRankPattern(unsigned char dim,unsigned char rank);
unsigned int gc_DecreaseDimRankPattern(unsigned int p, unsigned char dim, unsigned char rank);


// SN class
class SN
{
  public:	 // public interface

	  SN(); // default constructor

	 ~SN(); // default destructor

	 void Init(); // initializes the object

	 void Destroy(); // destroys the object

	 void Clean();	// cleans object data

	 void Multiply (SN *src); // multiplies by src

	 void Multiply(short a); // multiplies by a constant

	 void Add (SN *src); // adds

	 unsigned int Reduce(unsigned int starting_tree=0); // reduces the data

	 void CopyFrom(SN *src); // copies the internal structure from src

	 int LoadFromFile(char *filename, short position=0); // loads from file

	 void LoadFromRawFile(FILE *fil,unsigned char sn_rank = 0); // loads the content from texfile with no formatting

	 void SaveToFile(char *filename, unsigned short append=0); // saves to file

	 void SaveToFile(FILE *fil); // saves to file

	 void SplitToRawFiles(FILE **Handles, unsigned char max_rank);

	 sn_tree *AddTree(); // adds a new tree

	 void Print(FILE *dst=stdout); // prints the SN into stream

	 void RawPrint(FILE *dst=stdout); // prints the SN into stream widhtout formatting

	 void PlainPrint(char operator_chr='s', char variable_chr='x');

	 unsigned int ToString(char *str,unsigned int str_size,char operator_chr='s', char variable_chr='x');

	 int CompareWith(SN *src); // compares two SN's

	 // getters
	 unsigned int GetTreeCount();
	 int GetTreeCount(int *data);
	 unsigned char GetRank();
         unsigned char GetDim();
         unsigned int GetDRP();
	 unsigned char GetWidth();
	 char *GetName();
         TSNType GetType();
         char *GetIN();
         char *GetOUT();

	 unsigned short IsEmpty();

	 // setters
         void SetName(const char *new_name);
         void SetType(TSNType t);
         void SetIN(const char *new_in);
         void SetOUT(const char *new_out);

        protected:

	unsigned int lTrees;
	unsigned char Rank;
        unsigned char Dim;
        unsigned int DRP;
	unsigned char Width;

	char name[10];
        char in_name[200];
        char out_name[200];
        TSNType type;

	sn_tree *Trees;

	void Separator(FILE *fil);

	void PutHeader(FILE *fil);
	void ParseParamsFromString(char *buf,unsigned *params);

	void RaiseException(unsigned char ex_code); // raises predefined exception
	SN_Exception E; // exception class

};

#endif
//-----------------------------------------------------------------
