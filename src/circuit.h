#ifndef CIRCUIT_H
#define CIRCUIT_H

#include "const.h"
#include "utils.h"
#include "symb_matrix.h"
#include "param_file.hpp"
#include "ams_parser.hpp"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>


//-----------------------------------------------------------------
// Class representing a signle circuit 
class CCircuit
{

//-----------------------------------------------------------------

// public section goes here
//---------------------------
public:

        CCircuit();     // default constructor

	~CCircuit();	// default destructor

        int LoadAMSModel(const char *filename); // loads the AMS model

        int ParseBlockFile(char *filename); // parses block file

        void ListElements(FILE *fil=stdout,const char *prefix=NULL);  // provides a list of elements

	int ListBlocks(FILE *fil=stdout); // provides a list of blocks 
	
	int ExportToMatlab(char *filename); // exports node potential matrix into Matlab file 

        void LoadParams(const char *filename); // loads evaluated parameter values

        void ExportToVHDLAMS(const char *filename); // exports circuit structure into VHDL-AMS

        void ExportToHSpice(const char *filename); // exports circuit to the HSpice

        void NodeEquations(); // produces the node equations

	void Solve(char *filename); // solves the circuit transfer functions by using symbolic analysis

	int SortElements(); // sorts elements loaded via ParseBlock routine 

	int MakeBlockFile(); // generates a file with a table of blocks 

        int NodePotentials(SM *M,short on=0,TElement *ie=NULL);

	int SolveAsSNPermutative(char *filename); // solves the circuit by using permutative method

// private section goes here
//---------------------------
private:

        int ReorderElements(int &iterator,int &index, TElementRole role,TElement *storage);

        int AddGeneric(TElementRole role,short vin,short vout,short block_id,short sign);

	int Initialize();

        int AssignName(TElementType Type,char *output);

        int GetElement(TElementRole role,short block_id);

        int GetElement(TElementRole role,short vin, short vout,short *start_ind);

	void ConvertToTree(TElement *element,short vin,short vout,sn_tree *t);

	int FormatAsMatrixElement(TElement *element,short not_first,short vin,short vout,char *buffer);	

	int GetOutputNodes(short *count,short *nodes);

	int GetInputNodes(short *count,short *nodes);

        int ParseString(const char *str,int *cnt, int *data);
        int ParseBlockNames(const char *str);

	//data section 
	//----------------------------------------------------
	int lBlocks;
	int lElements;
	int lVertices;
	TElement *Elements;	
	char comment_chr;
        double *Params;
		
	// the filename 
	char blo_filename[256];

};

#endif
// The End
//-----------------------------------------------------------------
