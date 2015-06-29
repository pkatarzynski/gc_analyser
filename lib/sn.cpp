#include "sn.h"


//---------------------------------------------------
// some pattern utils - move them to utils.cpp later on
//---------------------------------------------------
unsigned int gc_CreateDimRankPattern(unsigned char dim,unsigned char rank)
{
    unsigned int result=0;
    int i =0;
    for (i=0; i < dim; i++)
    {
        result |= rank << DIM_BITS*i;
    }
    return result;
}
//-------------------------------------------
unsigned int gc_DecreaseDimRankPattern(unsigned int p, unsigned char dim, unsigned char rank)
{
    unsigned int result=p;

    int i =0;

    while (1)
    {
       if (result & (0x0000000f << DIM_BITS*i))
       {
           result -= (0x00000001 << DIM_BITS*i);
           break;
       }
       else
       {
           result |= (rank << DIM_BITS*i);
           i++;
       }
       if (i>=dim) break;
    }

    return result;
}

//-------------------------------------------
void SN_Exception::Show()
{
	printf("Exception code: %d\n",this->code);
}
//-----------------------------------------------------------------
// Throws prefedined exception 
void SN::RaiseException(unsigned char ex_code)
{
	E.code = ex_code;
	throw E;
}
//-----------------------------------------------------------------
// default constructor
SN::SN()
{
	Init(); // perform the initialization
}
//-----------------------------------------------------------------
// Default destructor 
SN::~SN()
{
	Destroy();
}
//-----------------------------------------------------------------
// Initialization routine 
void SN::Init()
{	
	// reset default parametres 
	lTrees = 0; 
	Width = 0; 
	Rank = 0;
	Trees = NULL;
        strncpy(name,"SN",3);
        strncpy(in_name,"#",3);
        strncpy(out_name,"#",3);
        type = snAny;
}
//-----------------------------------------------------------------
// Destroys all allocated data
void SN::Destroy()
{
	lTrees = 0;
	Width = 0; 
	Rank  = 0;
	if (Trees == NULL) return; 
}
//-----------------------------------------------------------------
// cleans the trees
void SN::Clean()
{
	Rank = 0; 
	Width = 0; 
        Dim = 1;
	// release memory content
	for (unsigned int t = 0; t<lTrees; t++) Trees[t].Destroy();

	free(Trees);
	Trees  = NULL;
	lTrees = 0; 
}
//-----------------------------------------------------------------
//adds a new tree
sn_tree *SN::AddTree()
{	
	lTrees++;

	if (lTrees > snMAX_TREE_CNT) RaiseException(snTOO_MANY_TREES);

	Trees = (sn_tree*) realloc(Trees,lTrees*sizeof(sn_tree));	
	
	if (Trees == NULL) RaiseException(snMEM_STORAGE);

	Trees[lTrees-1].Init();
	
	return &Trees[lTrees-1];
}
//-----------------------------------------------------------------
void SN::Separator(FILE *fil)
{
	fprintf(fil,"%%");
        fprintf(fil,"---");
        for (int i=0; i < Width; i++) fprintf(fil,"----");
	fprintf(fil,"\n");
}
//-----------------------------------------------------------------
// prints the SN into the stream 
void SN::Print(FILE *dst)
{
  if (lTrees == 0) return;

  GetWidth();
  GetRank();
  GetDim();

  unsigned int drp = gc_CreateDimRankPattern(Dim,Rank);

   unsigned char done = 0;
   while(done == 0)
   {       

     // check if there are non empty terms for current drp
     unsigned short has_terms = 0;
     for (unsigned int i = 0; i < lTrees; i++)
         {
            if (Trees[i].GetMult() == 0) continue;
            if (Trees[i].GetDimRankPattern() == drp)
            {
               has_terms=1;
               break;
            }
         } // for i

     if (has_terms)
     {

      Separator(dst);

      if (Dim > 1)
      {        
        fprintf(dst,"%%! S \t%d\n",drp);
        fprintf(dst,"%%DRP \t%08x\n",drp);
      }
      else
        fprintf(dst,"%% S %d\n",drp);

      Separator(dst);

      for (unsigned int i = 0; i < lTrees; i++)
          {
             if (Trees[i].GetDimRankPattern() != drp) continue;
             if (Trees[i].GetMult() == 0) continue;
             Trees[i].Print(dst);
             fprintf(dst,"\n");
          } // for i
    }

      if (drp)
        drp = gc_DecreaseDimRankPattern(drp,Dim,Rank);
      else
        done  = 1;

    }
     Separator(dst);
}
//-----------------------------------------------------------------
// prints the SN content into stream without formatting
void SN::RawPrint(FILE *dst)
{
	for (unsigned int t=0; t<lTrees; t++)
	{
		printf("%3d|",t);
		Trees[t].Print(dst);
		printf("\n");
	}
}
//-----------------------------------------------------------------
// prints the SN in polynominal style 
void SN::PlainPrint(char operator_chr, char variable_chr)
{
	char buf[snMAX_LINE_LEN];
	buf[0] = 0x00;
	ToString(buf,snMAX_LINE_LEN,operator_chr,variable_chr);
	printf("%s",buf);
}
//-----------------------------------------------------------------
// converts the SN into NULL terminated string 
// returns number of characters occupied by the string 
unsigned int SN::ToString(char *str,unsigned int str_size,char operator_chr, char variable_chr)
{
	GetRank();

	if (lTrees == 0)
	{
		gc_strcat(str,str_size,(char *)"0");
		return strlen(str);
	}

	for (unsigned int t=0; t<lTrees; t++)
	{
		if (t)
		{
			#ifdef UNIX
				if (Trees[t].GetMult() > 0) sprintf(str+strlen(str),"+");
			#else
				if (Trees[t].GetMult() > 0) sprintf_s(str+strlen(str),str_size,"+");
			#endif
		}
		Trees[t].ToString(str,str_size,operator_chr,variable_chr);						
	}		

	return strlen(str);
}
//-----------------------------------------------------------------
// compares the SN with counterpart 
// returns 1 when the SN's are the same
int SN::CompareWith(SN *src)
{
	unsigned int N = lTrees; 
	unsigned int K = src->lTrees;
	
	if (N != K) return 0; 

	unsigned short hit = 0;
	for (unsigned int n=0; n < N; n++)
	{
		hit = 0;
		for (unsigned int k=0; k < K; k++)
		{
			if (&Trees[n] == &Trees[k])
			{
				hit = 1;
				break;
			}			
		}
		if (!hit) return 0;
	}

	return 1;
}
//-----------------------------------------------------------------
unsigned short SN::IsEmpty()
{
	if (lTrees) return 0; else return 1;
}
//-----------------------------------------------------------------
// returns actual entry count 
unsigned int SN::GetTreeCount() {return lTrees;}
//-----------------------------------------------------------------
// returns entry count among particular powers
int SN::GetTreeCount(int *data)
{
    GetRank();

	for (int r = Rank; r >= 0; r--) data[r] = 0;
	
	
	for (int r = Rank; r >= 0; r--)
	{
		for (unsigned int t = 0; t <  lTrees; t++)
		{
			if (Trees[t].GetRank() != (unsigned int)r) continue;
			if (Trees[t].GetCnt() > 0) data[r]++;
		} 
	}	
	
	return 0;
}
//-----------------------------------------------------------------
// returns actual rank
unsigned char SN::GetRank() 
{
	Rank = 0;	
	for (unsigned int t=0; t<lTrees; t++)
	{
		if ((Trees[t].GetRank() > Rank)&&(Trees[t].GetCnt()>0)) Rank = Trees[t].GetRank();
	}
	return Rank;
}
//-----------------------------------------------------------------
// returns actual dimension
unsigned char SN::GetDim()
{
    Dim = 0;
    for (unsigned int t=0; t<lTrees; t++)
    {
            if ((Trees[t].GetDim() > Dim)&&(Trees[t].GetCnt()>0)) Dim = Trees[t].GetDim();
    }
    return Dim;
}
//-----------------------------------------------------------------
// returns actual dimension
unsigned int SN::GetDRP()
{
    DRP = 0;
    for (unsigned int t=0; t<lTrees; t++)
    {
            if ((Trees[t].GetDimRankPattern() > DRP)&&(Trees[t].GetCnt()>0)) DRP = Trees[t].GetDimRankPattern();
    }
    return DRP;
}
//-----------------------------------------------------------------

// returns actual width
unsigned char SN::GetWidth() 
{
	Width = 0;
	for (unsigned int t=0; t<lTrees; t++)
	{
		if (Trees[t].GetCnt() > Width) Width = Trees[t].GetCnt();
	}
	return Width;
}
//-----------------------------------------------------------------
char *SN::GetName()
{
	return name;
}
//-----------------------------------------------------------------
TSNType SN::GetType()
{
        return type;
}
//-----------------------------------------------------------------
char *SN::GetIN()
{
return in_name;
}
//-----------------------------------------------------------------
char *SN::GetOUT()
{
return out_name;
}
//-----------------------------------------------------------------
void SN::SetName(const char *new_name)
{
        memcpy(name,new_name,strlen(new_name)+1);
}
//-----------------------------------------------------------------
void SN::SetType(TSNType t)
{
    type = t;
}
//-----------------------------------------------------------------
void SN::SetIN(const char *new_in)
{
    memcpy(in_name,new_in,strlen(new_in)+1);
}
//-----------------------------------------------------------------
void SN::SetOUT(const char *new_out)
{
    memcpy(out_name,new_out,strlen(new_out)+1);
}
//-----------------------------------------------------------------
// Reduces the data 
unsigned int SN::Reduce(unsigned int starting_tree)
{  
// overall number of reductions 
  unsigned int lReductions = 0;

// perform algebraic reductions 
  for (unsigned int i = starting_tree; i<lTrees; i++)
  {
	  for (unsigned int j=i+1; j<lTrees; j++)
	  {
		  if (Trees[i].ReduceWith(&Trees[j])) lReductions++;
	  }
  }

// shrink the tree table 
	unsigned int j=0;
	for (unsigned int i=starting_tree; i<lTrees; i++) 
	{
		if (Trees[i].GetMult() == 0) continue;
		if (Trees[i].GetCnt() == 0) continue;		
		memcpy(&Trees[j],&Trees[i],sizeof(sn_tree));
		j++;		
	}	
// reallocate memory 
  lTrees = j+starting_tree;
  Trees = (sn_tree *) realloc(Trees,lTrees*sizeof(sn_tree));
  if ((lTrees)&&(Trees == NULL)) RaiseException(snMEM_STORAGE);

  return lReductions;  // will return the number of reductions 
}
//-----------------------------------------------------------------
// Adds a src 
void SN::Add(SN *src)
{
	// due to problems with memory management prevent from self add 
	if (this == src) RaiseException(snSELF_ADD);

	sn_tree *t;
	for (unsigned int i=0; i < src->lTrees; i++)
	{
		t = AddTree();		
		t->CopyFrom(&src->Trees[i]);
	}
}
//-----------------------------------------------------------------
 // Multiplies by src
void SN::Multiply (SN *src)
{
  // spawn the SN
	unsigned int N = src->lTrees; // remote tree count
	unsigned int K = lTrees; // local tree count

	if (N == 0 ) // multiplicant is empty 
	{
		Clean();
		return;
	}

	// due to problems with memory management prevent from self multiplying 
	if (this == src) RaiseException(snSELF_MULTIPLY);

	sn_tree *t;
	for (unsigned int k=0; k < K; k++)
	{
		for (unsigned int n = 1; n < N; n++)
		{
			t = AddTree();
			t->CopyFrom(&Trees[k]);
			t->Multiply(&src->Trees[n]);
		}
		t = &Trees[k];
		t->Multiply(&src->Trees[0]);
	}
}
//-----------------------------------------------------------------
 // Multiplies by src
void SN::Multiply (short a)
{
	for (unsigned int t = 0; t < lTrees; t++) Trees[t].Multiply(a);
}
//-----------------------------------------------------------------
// Copies the internal structure from src 
void SN::CopyFrom(SN *src)
{
  if (this == src) return; //prevent from self-copying
  Clean();
  Add(src);
}
//-----------------------------------------------------------------
void SN::ParseParamsFromString(char *buf,unsigned *params)
{
	char tmp[200];
	unsigned int len = strlen(buf); 	
	bool is_param = true;
	
	char param[200];
	char value[200];

	buf[len-1] = ' ';
	buf[len] = 0x00;
	
	int j = 0;	
	for (unsigned int i = 0; i < len; i++)
	{
		if (buf[i] != ' ') {tmp[j] = buf[i]; j++;}
		
		if ((j) && (buf[i] == ' '))
		{
			tmp[j]=0x00;

			if (is_param) // the string is a param 
			{
				param[0] = 0x00;
				gc_strcpy(param,200,tmp);
			}
			else // the string is a value 
			{
				value[0] = 0x00;
				gc_strcpy(value,200,tmp);
                                if (strcmp(param,"BEGIN") == 0)  params[SN_BEGIN] = 1;
                                if (strcmp(param,"S") == 0) params[SN_CURRENT_DRP] = atoi(value);
                                if (strcmp(param,"DRP") == 0) params[SN_DRP] = atoi(value);
                                if (strcmp(param,"DIMENSION") == 0) params[SN_DIM] = atoi(value);
                                if (strcmp(param,"RANK") == 0) params[SN_RANK] = atoi(value);
                                if (strcmp(param,"END") == 0) params[SN_END] = 1;
                                if (strcmp(param,"LENTRIES") == 0) params[SN_COUNT] = atoi(value);
                                if (strcmp(param,"NAME") == 0) strncpy(name,value,strlen(value)+1);
                                if (strcmp(param,"IN") == 0) strncpy(in_name,value,strlen(value)+1);
                                if (strcmp(param,"OUT") == 0) strncpy(out_name,value,strlen(value)+1);
                                if (strcmp(param,"TYPE") == 0) type = TSNType(atoi(value));
			}
			j=0;
			tmp[j] = 0x00;
			is_param = !is_param;
		}
	}// for i
}
//-----------------------------------------------------------------------------------
// Loads entries from given file 
int SN::LoadFromFile(char *filename,short position)
{
	FILE *fil = NULL;

        int result  = 0;
	
	gc_fopen(&fil,filename,(char *)"r+");
	
	// check if the file was opened 
        if (fil == NULL) throw new gcError("Unable to open file",filename);

	char buf[snMAX_LINE_LEN]; // single line	acquired from file 

	Clean(); // clean previously stored data
	
        unsigned int current_drp = 0; // actual dimension-rank pattern

	unsigned short top_rank = 0; // top rank of the entries
	
	unsigned int tree_count = 0;  // tree count acquired from file 

	unsigned int len; // string length
	
	short pos = -1;  // current SN position in a file 

	unsigned int params[SN_FLAGS_CNT]; // storage for parametres acquired from file 

	sn_tree *t; // single entry 

	for (int i = 0; i < SN_FLAGS_CNT; i++) params[i] = 0; // reset the table 


	while (!feof(fil))
	{
      
	  fgets(buf,snMAX_LINE_LEN,fil); // read a single string 
	  len = strlen(buf);

	  if (len < 2) continue; // line too short 

	  if ((buf[0] == '%') && (buf[1] == '!'))
	  {
		ParseParamsFromString(buf+2,params);	  	  

                // don't parse anything with no BEGIN statement
                if (params[SN_BEGIN])
                {
                    pos++;
                    params[SN_BEGIN] = 0;
                    if (pos > position) break;
                }

		continue;
	  }


          if (pos < position) continue;
          if (pos == position) result = 1;

	  // change current parametres according to the readouts 
          current_drp = params[SN_CURRENT_DRP];
	  tree_count = params[SN_COUNT];	  
	  top_rank = params[SN_RANK];

	  if ((buf[0] == '%') && (buf[1] != '!')) continue;

	  t = AddTree();          
          t->SetDim(params[SN_DIM]);
          t->SetDRP(current_drp);
          t->SetRankFromDRP();
	  t->ParseFromString(buf);	  
	} // while eof

	fclose(fil);

        return 1;

	// check consistency 
		if (tree_count != this->lTrees) RaiseException(snFILE_CORRUPTED);	
		if (top_rank != this->GetRank()) RaiseException(snFILE_CORRUPTED);	
                return result;
}
//-----------------------------------------------------------------
// loads the content from texfile with no formatting 
void SN::LoadFromRawFile(FILE *fil,unsigned char sn_rank)
{
	sn_tree *t;

	if (fil == NULL) RaiseException(snIO_ERROR); // file wasnt opened 
	
	// read the entire file 
	while (!feof(fil))
	{
		t = AddTree();
		t->LoadFromStream(fil);
	}
}
//-----------------------------------------------------------------
// puts a file header 
void SN::PutHeader(FILE *fil)
{
	fprintf(fil,"%%! BEGIN %s\n",name);
        fprintf(fil,"%%! NAME %s\n",name);
        fprintf(fil,"%%! TYPE %d\n",type);
        fprintf(fil,"%%! IN %s\n",in_name);
        fprintf(fil,"%%! OUT %s\n",out_name);
	fprintf(fil,"%%! RANK %d\n",GetRank());
        fprintf(fil,"%%! DIMENSION %d\n",GetDim());
        fprintf(fil,"%%! DRP %d\n",GetDRP());
	fprintf(fil,"%%! LENTRIES %d\n",GetTreeCount());
	fprintf(fil,"%%! COLUMNS %d\n",GetWidth());
}
//-----------------------------------------------------------------
// Saves data to file 
void SN::SaveToFile(char *filename, unsigned short append)
{
	FILE *fil = NULL;
	
        if (append)
            gc_fopen(&fil,filename,(char *)"a");
        else
            gc_fopen(&fil,filename,(char *)"w");
	
	if (fil == NULL) RaiseException(snIO_ERROR);

	PutHeader(fil);	
	Print(fil);

	fprintf(fil,"%%! END %s\n",name);

	fclose(fil);

}
//-----------------------------------------------------------------
//saves sn content to a file 
void SN::SaveToFile(FILE *fil)
{
	if (fil == NULL) RaiseException(snIO_ERROR);

	PutHeader(fil);	
	Print(fil);
	fprintf(fil,"%%! END %s\n",name);
}
//-----------------------------------------------------------------
// splits the SN content between a set of files defined by Handles table
void SN::SplitToRawFiles(FILE **Handles, unsigned char max_rank)
{
	for (unsigned int t=0; t<lTrees; t++)
	{
		if (Trees[t].GetRank() > max_rank) RaiseException(snSTORAGE_ERROR);
		if (Trees[t].GetMult() == 0) continue;

		Trees[t].BinaryPrint(Handles[Trees[t].GetRank()]);
//		Trees[t].Print(Handles[Trees[t].GetRank()]);
		//fprintf(Handles[Trees[t].GetRank()],"\n");
	}
}

// THE END
//-----------------------------------------------------------------
