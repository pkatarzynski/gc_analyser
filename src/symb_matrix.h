#ifndef symb_matrix_H
#define symb_matrix_H

// Includes
//-----------------------------------------------------------------
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "sn.h"
#include "const.h"

// Settings
//-----------------------------------------------------------------
#define DEBUG
#define MAX_RANK 100

// Exception codes definitions 
//-----------------------------------------------------------------
#define exUNDEFINED 0
#define exINVALID_RANK 1
#define exINVALID_INDEX 2
#define exNO_MINORS 3
#define exIO_ERROR 4
// Exception class
//-----------------------------------------------------------------

class SM_Exception
{
public:
		void Show();		
		short code; 
private:
		
};

// Main class
//-----------------------------------------------------------------
class SM
{
public:

		SM(unsigned short new_rank);
		~SM();
		void Display(FILE *dst=stdout);
		void Clean();
		void Initialize();
		void Destroy();
		SN* Get(unsigned short r, unsigned short c);		                
                void Det(FILE *fil=stdout,const char *in=NULL,const char *out=NULL);
	
private:		

	struct Minor
	{
		SN *term;      // multiplying term 
		SM *matrix;    // matrix 
	};


	unsigned short rank;
	SM_Exception E;
	SN **Data;

	// storage for determinant data 
	FILE **det_storage;
	
	char **det_names;

	unsigned short exp_level; // expansion level for partial Laplace expansion
	unsigned short exp_row;
	unsigned short exp_col;
	
	void LaplaceExpand(Minor *M);
	void FindExpansionPath();
	void MakeMinor(short row,short col,Minor *result);
	void RaiseException(unsigned short ex_code);
	unsigned int ColumnWidth();

	void PrepareStorage(const char *file_mode); // prepares file storage for determinant data 
	void CollectFromStorage(SN *result);
	void CleanupStorage(); // deletes temporary storage files 
	void FinalizeStorage(); // closes the storage files 


};

//Additional structures
//-----------------------------------------------------------------

//-----------------------------------------------------------------
#endif
