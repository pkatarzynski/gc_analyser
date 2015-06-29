#ifndef sn_eval_H
#define sn_eval_H

// Includes
//-----------------------------------------------------------------
#include "sn.h"
#include "complex.h"

// Settings
//-----------------------------------------------------------------
#define MAX_VALUES 100

#ifndef MAX_RANK
    #define MAX_RANK 100
#endif

//#define snDERIVES 1
//#define snVALUES 2

// Exception codes definitions 
//-----------------------------------------------------------------
#define snNO_PARAMS 100
#define snINVALID_PARAM_INDEX 101
#define snINVALID_OMEGA 102
#define snNO_TERMS 103
#define snINVALID_TERMS 104
//------------------------------------------------------------------
//multidimensional terms
struct MDTerm {
                  unsigned int drp;
                  double val;
};
class SN_EVAL : public SN
{

public:

        SN_EVAL();					// default constructor
        ~SN_EVAL();					// default destructor
        void CleanSN();					// clears the sn structure
        void Clean();					// clears the class
        void LoadParams(char *filename);		// loads parameter values from file
        void LoadParams(int cnt, double *params);       // loads parameter values from vector
        void PrintParams(FILE *fil=stdout);		// prints parameter values
        void PrintDerives(FILE *fil=stdout);		// prints the derives table
        void PrintTerms(FILE *fil = stdout);		// prints the polynomial terms
		void PrintMDTerms(FILE *fil = stdout);		// prints the polynomial terms for multidimensional analysis
		void CalculateTerms();
		void LoadTerms(unsigned char cnt, double *t);	// loads the polynomial terms from external table of real coeffs
		void LoadTerms(unsigned char cnt, complex *t);	// loads the polynomial terms from external table of complex coeffs
        void CalculateDerives();			// calculates derives with respect to parametres and rank
        void CleanDerives();				// cleans the derive storage
        double CalculateTree(sn_tree *t);		// calculates values from a single tree
        void PrintPolynomial(FILE *dst = stdout);	// prints the polynomial acquired from generic SN
        complex EvalPoly(double w);			// evaluates the polynominal with given argument w
        complex EvalPolyMD(int dim,double *w);			// evaluates polynomial in multidimensional domain 
        float EvalSens(unsigned char p,float w);	// evaluates the sensitivity for param p with given omega
	// getters 
	unsigned char GetParamCnt();        
        double GetTerm(unsigned char id);
	
private:

	void Derive(sn_tree *t,unsigned char symbol);

	// parametres
        double *Params;
	unsigned char lParams;

	// polynomial terms 
	double *Terms;
	MDTerm *MDTerms; // table of multidimensional terms
	unsigned char lTerms;
	
	// derives
	float **Derives;
	unsigned char has_derives;

};

#endif
