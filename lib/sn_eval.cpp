	#include "sn_eval.h"
//-----------------------------------------------------------------
// Default constructor
SN_EVAL::SN_EVAL()
{
	Params = NULL;
	Terms = NULL;
	Derives = NULL;
	has_derives = 0;
	Clean();
}
//-----------------------------------------------------------------
// Cleans the inherited data
void SN_EVAL::CleanSN()
{
	SN::Clean();
}
//-----------------------------------------------------------------
// Cleans the class specific fields
void SN_EVAL::Clean()
{
	if (has_derives)
	{
		for (int i = 0; i<lParams; i++) 
		{
			free(Derives[i]);
			Derives[i] = NULL;
		}
		has_derives = 0; 
	}

	lParams = 0;
	lTerms = 0;
	if (Params != NULL) free(Params);
	if (Terms != NULL) free(Terms);
	Params = NULL;
	Terms = NULL;
}
//-----------------------------------------------------------------
void SN_EVAL::LoadParams(int cnt, double *_params)
{    
    lParams = cnt;
    Params = (double *) realloc(Params,lParams*sizeof(double));
    for (int i = 0; i<lParams; i++)
   
    {        
        Params[i] = _params[i];        
        fflush(stdout);
    }
}
//-----------------------------------------------------------------
// Loads parameters from file 
void SN_EVAL::LoadParams(char *filename)
{	
	FILE *fil = NULL;
	
	// try to open the file 
	fil  = fopen(filename,"r+");
	if (fil == NULL) RaiseException(snIO_ERROR);

	lParams = 0;
	float dummy;
	while (!feof(fil))
	{
                fscanf(fil,"%f",&dummy);
		lParams++;
                Params = (double *) realloc(Params,lParams*sizeof(double));
		Params[lParams-1] = dummy;
	}
	fclose(fil);
}
//-----------------------------------------------------------------
// Prints the param set
void SN_EVAL::PrintParams(FILE *fil)
{
	for (int i=0; i < lParams; i++)
	{
	  fprintf(fil,"\t%10.10f;\t%%x%d\n",Params[i],i+1);
	}	
}
//------------------------------------------------------------------
// Prints the derive set
void SN_EVAL::PrintDerives(FILE *fil)
{
	if (!has_derives) return;

	for (int y = 1; y <= lParams; y++)
	{
		for (int x = this->Rank; x >= 0; x--)
		{
			fprintf(fil,"%10.10f \t",Derives[y-1][x]);
		}
		fprintf(fil,"%% x%d\n",y);
	}	
}
//------------------------------------------------------------------
void SN_EVAL::PrintTerms(FILE *fil)
{
	if (lTerms == 0) RaiseException(snNO_TERMS);
	for (char t = lTerms-1; t >= 0; t--)
                fprintf(fil,"\t%10.10f; \t %%s^%d\n",Terms[int(t)],t);
}
//------------------------------------------------------------------
void SN_EVAL::PrintMDTerms(FILE *fil)
{
//FIXME: Add proper formatting for dimensions of s
	if (lTerms == 0) RaiseException(snNO_TERMS);
	for (int t = lTerms-1; t >= 0; t--)
//                fprintf(fil,"\t%10.10f; \t %%s^%d\n",MDTerms[t].val,t);
                fprintf(fil,"drp = %d; \t val=%f\n",MDTerms[t].drp,MDTerms[t].val);
}
//------------------------------------------------------------------
// Calculates values obtained by a single tree
double SN_EVAL::CalculateTree(sn_tree *t)
{
        int ind;	
        double result = t->GetMult();

        for (unsigned int i = 0; i < t->GetCnt(); i++)
	{
		ind = t->GetItem(i);
		if ((ind-1) > lParams) RaiseException(snINVALID_PARAM_INDEX);
		result *= Params[ind-1];
	}
	return result;	
}
//------------------------------------------------------------------
// Calculates parameter values
void SN_EVAL::CalculateTerms()
{
	if (lParams == 0) RaiseException(snNO_PARAMS);
	sn_tree *sn_t;
	lTerms = 0;
	MDTerms = (MDTerm *) realloc(MDTerms,(lTerms+1)*sizeof(MDTerm));
        for (unsigned int t = 0; t < this->GetTreeCount(); t++)
		{
		sn_t = &this->Trees[t]; // take one tree
		int term_found = 0;  
		for (int i=0;i<lTerms; i++) // search the table with MDTerms
		{
			if (MDTerms[i].drp == sn_t->GetDimRankPattern())
			{
				MDTerms[i].val += CalculateTree(sn_t);			
				term_found = 1;
				break;
			}
		}
			if (term_found == 0) // no such term so far 
			{
				lTerms++;
				MDTerms = (MDTerm *) realloc(MDTerms,(lTerms+1)*sizeof(MDTerm));
				MDTerms[lTerms-1].drp = sn_t->GetDimRankPattern();
				MDTerms[lTerms-1].val = CalculateTree(sn_t);
			}

	}

}
//------------------------------------------------------------------
// Loads the terms from external table holding double real terms
void SN_EVAL::LoadTerms(unsigned char cnt, double *t)
{
	this->Clean();
	// prepare storage for the terms
        this->Rank = cnt-1;
        lTerms = cnt;
	Terms = (double *) realloc(Terms,lTerms*sizeof(double));
        for (int i = 0; i < lTerms; i++) Terms[lTerms-i-1] = t[i];
}
//------------------------------------------------------------------
// Loads the terms from external table holding complex terms  
void SN_EVAL::LoadTerms(unsigned char cnt, complex *t)
{
	this->Clean();
	// prepare storage for the terms
	this->Rank = cnt;
	lTerms = cnt+1;

	//printf("lTerms=%d	",lTerms);
	Terms = (double *) realloc(Terms,lTerms*sizeof(double));

	if (Terms == NULL) printf("NULL terms\n");
	
	for (int j = 0; j < lTerms; j++)
	{
		// unbalanced poles result with having non zero imagined parts 
		if (fabs(t[j].im) > 1e-2) RaiseException(snINVALID_TERMS); 		
		Terms[j] = t[j].re; // take real parts only 
	}		
}
//------------------------------------------------------------------
double SN_EVAL::GetTerm(unsigned char id)
{
      if (id < this->lTerms)
          return Terms[id];
      else
          return -1e64;
}
//------------------------------------------------------------------
// Calculates derive for a signle tree
void SN_EVAL::Derive(sn_tree *t,unsigned char symbol)
{
	float result = 1.0;

	if (t->GetMult() == 0) return;

	unsigned short had_symbol = 0;

	for (unsigned int i=0; i < t->GetCnt(); i++)
	{		
		if (t->GetItem(i) == symbol)
		{			
			had_symbol++;
			if (had_symbol == 1) continue;
		}				
		result *= Params[t->GetItem(i)-1];
	} 
	
	result *= had_symbol;	
	result *= t->GetMult();

	Derives[symbol-1][t->GetRank()] += result;

}
//------------------------------------------------------------------
// Calculates the derive values with respect to given parameters
void SN_EVAL::CalculateDerives()
{	
	// check if there are params loaded
	if (lParams == 0) return;		

	CleanDerives(); // cleanup previously stored data

	// allocate and reset the memory first; 
	Derives = (float **) calloc(lParams,sizeof(float *)); // allocate main storage

	for (unsigned char i = 0; i < lParams; i++)
	{
		Derives[i] = (float *) calloc((this->Rank+1),sizeof(float)); // allocate sub storage
		for (unsigned char  j = 0; j <= this->Rank; j++) Derives[i][j] = 0.0; // reset
	}

	for (unsigned char symbol = 1; symbol <= lParams; symbol++)
	{
		for (unsigned int t = 0; t < this->lTrees; t++)
		{
			Derive(&this->Trees[t],symbol);
		} // for symbol
	}// for p


	
	has_derives = 1;
}
//------------------------------------------------------------------
// Cleans the derive storage 
void SN_EVAL::CleanDerives()
{
	if ( has_derives == 0) return;
	
	// release the sub table 
	for (unsigned char i = 0; i <= this->Rank; i++)
	 if (Derives[i] != NULL) free(Derives[i]);

	// release the parent table
	if (Derives != NULL) free(Derives);

	has_derives = 0;

}
//------------------------------------------------------------------
// Prints the polynomial acquired from generic SN
void SN_EVAL::PrintPolynomial(FILE *dst)
{
	if (lTerms == 0) RaiseException(snNO_TERMS);

        for (int t = this->Rank;  t >= 0; t--)
	{		
		if (fabs(Terms[t]) < 1e-6) continue;
		if (Terms[t] > 0) fprintf(dst,"+");
		fprintf(dst,"%2.4f",Terms[t]);
		if (t > 0) fprintf(dst,"*s^%d",t);		
	}
}
//------------------------------------------------------------------
// Evaluates the polynominal with given argument
complex SN_EVAL::EvalPoly(double w)
{
	if (w < 0.0) RaiseException(snINVALID_OMEGA);
	complex result;

	result.im = 0.0;
	result.re = 0.0;

	complex wa;
	wa.re = 0.0;
	wa.im = w;
	
	double term;
	complex power;
	complex mul;

	for (unsigned char t=0; t <= this->Rank; t++) 
	{
		term = Terms[t];
		power = Pow(wa,double(t));
		mul = Mult(power,term);
		SelfSum(result,mul);
        }

	return result;
}
//------------------------------------------------------------------
//Evaluates polynomial for multidimensional domain 
complex SN_EVAL::EvalPolyMD(int dim,double *w)
{	
	complex result;

	result.im = 0.0;
	result.re = 0.0;
	
	complex wa;
	
	unsigned char pow;
	complex power;
	complex mul;
	complex c_pow;
	for (unsigned char t=0; t < this->lTerms; t++) 
	{
		power.re = 1.0;
		power.im = 0.0;
		for (int d=0;d<dim;d++)
		{
			pow = (MDTerms[t].drp &(0xf << d)) >> d*DIM_BITS;			
			wa.re = 0.0;
			wa.im = w[d];		
			c_pow = Pow(wa,double(pow));
			power = Mult(power,c_pow);			
		}		
		mul = Mult(power,MDTerms[t].val);
		SelfSum(result,mul);
    }
	return result;
}
//------------------------------------------------------------------
// evaluates the sensitivity for given omega
float SN_EVAL::EvalSens(unsigned char p,float w)
{
	if (lParams == 0) RaiseException(snNO_PARAMS);
	if (p > lParams) RaiseException(snINVALID_PARAM_INDEX);	
	if (p <= 0) RaiseException(snINVALID_PARAM_INDEX);	
	if (w <  0) RaiseException(snINVALID_OMEGA);

	// prepare additional variables 
	complex h;		h.im = 0.0;		h.re = 0.0;	
	complex d;		d.im = 0.0;		d.re = 0.0;
        complex wa;		wa.im = w;              wa.re = 0.0;
	complex x;		x.im = 0.0;		x.re = Params[p-1];
	
	double derive;
	complex mul;
	complex power;
	// calculate d
	for (unsigned char s=0; s <= this->Rank; s++) 
	{
		derive = double(Derives[p-1][s]);
		power = Pow(wa,s);
		mul = Mult(power  , derive );
		SelfSum( d , mul );
	}

	// calculate h
	complex eval = EvalPoly(w);
	h = Div(x,eval);

	return Re(Mult(h,d));

}
//------------------------------------------------------------------
// Default destructor
SN_EVAL::~SN_EVAL()
{
	Clean();
}
//------------------------------------------------------------------
// returns param count 
unsigned char SN_EVAL::GetParamCnt() {return lParams;}

// The End
//------------------------------------------------------------------
