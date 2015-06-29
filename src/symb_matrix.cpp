#include "symb_matrix.h"

//-----------------------------------------------------------------
void SM_Exception::Show()
{
	printf("Exception code: %d\n",this->code);
}
//-----------------------------------------------------------------
void SM::RaiseException(unsigned short ex_code)
{
	E.code = ex_code;
	throw E;
}
//-----------------------------------------------------------------
// Class' constructor
SM::SM(unsigned short new_rank)
{
	if (new_rank <= 0) RaiseException(exINVALID_RANK);
	if (new_rank >= MAX_RANK) RaiseException(exINVALID_RANK);	
	rank = new_rank;
	Initialize();
}
//-----------------------------------------------------------------
SM::~SM()
{
	Destroy();
}
//-----------------------------------------------------------------
// performs matrix initialization (after creation)
void SM::Initialize()
{
	exp_level = 0;
	exp_row = 0;
	exp_col = 0;

	Data = (SN**)malloc(rank*sizeof(*Data));
	for (int i=0; i<rank; i++)
	{
		Data[i] = (SN *) malloc(rank*sizeof(**Data));

		for (int j=0;j<rank; j++)
		{
			Data[i][j].Init();
		}
	}
	Clean();
}
//-----------------------------------------------------------------
// cleans the matrix by setting its elements to NULL
void SM::Clean()
{
	for (int r=0; r<rank; r++)	
		for (int c=0; c<rank; c++) Data[r][c].Clean();
	exp_level = 0;
}
//-----------------------------------------------------------------
// Releases all allocated memory 
void SM::Destroy()
{
	if (Data == NULL) return;  // matrix already destroyed

	for (int i=0; i<rank; i++)
	{
		for (int j=0; j<rank; j++) Data[i][j].Destroy();		
		free(Data[i]);
	}

	free(Data);
	Data = NULL;
}
//-----------------------------------------------------------------
// calculates the column width for a content
unsigned int SM::ColumnWidth()
{
	unsigned int result = 0; 
	unsigned int len = 0;

	char dummy[2048];
	
	for (unsigned short r = 0; r < rank; r++)
	{
		for (unsigned short c = 0; c < rank; c++)
 		{			
			dummy[0] = 0x00;
			len = Data[r][c].ToString(dummy,2048);
			if (len > result) result = len;
		}
	}
	return result;
}
//-----------------------------------------------------------------
// Displays matrix elements
void SM::Display(FILE *fil)
{
	char str[2048];
	
	unsigned int col_width = ColumnWidth();
	unsigned int len = 0;

	if (col_width % 2) col_width++;

	if (Data == NULL) return;

	fprintf(fil,"[\t");
	for (int r=0; r < rank; r++)
	{
		if (r>0) fprintf(fil,"\t");
		for (int c=0; c < rank; c++)
		{
			str[0] = 0x00;
			len = Data[r][c].ToString(str,2048);
			
			fprintf(fil,"%s",str);
			for (unsigned int i=0; i<(col_width - len); i++) fprintf(fil," ");

			if (c < rank-1) fprintf(fil,",");
		}
		if ( r < rank-1) fprintf(fil,";\n");
		if ( r == rank-1) fprintf(fil,"]\n");
	}

}
//-----------------------------------------------------------------
//Gets a single matrix element
SN* SM::Get(unsigned short r, unsigned short c)
{
	if (r > rank) RaiseException(exINVALID_INDEX);
	if (c > rank) RaiseException(exINVALID_INDEX);
	if (r <= 0) RaiseException(exINVALID_INDEX);
	if (c <= 0) RaiseException(exINVALID_INDEX);

	return &Data[r-1][c-1];
}
//-----------------------------------------------------------------
// makes a single minor
void SM::MakeMinor(short row,short col,Minor *result)
{		
	// multiplying term
	if (result->term == NULL)
	{
		//printf("term created\n");
		result->term = (SN*) malloc(sizeof(SN));
		result->term->Init();			
	}
	result->term->Clean();		
	result->term->CopyFrom(this->Get(row,col));

	// determine the sign for minor term
	if ((row+col)%2)
	{	
		result->term->Multiply(-1);
	}

	// minor matrix
	if (result->matrix == NULL)
	{
		//printf("matrix created\n");
		result->matrix = (SM *) malloc(sizeof(SM)); // create sub matrix 

		result->matrix->rank = this->rank-1;
		result->matrix->Initialize();	
	}

	result->matrix->Clean();

	SN *src_e; // source element
	SN *dst_e; // destination element

	// fill the minor matrix
	short m_r = 1;
	short m_c = 1;
	for (short r=1; r <= this->rank; r++)
	{
		if (r == row) continue;
		m_c = 1;
		for (short c=1; c <= this->rank; c++)
		{
			if (c == col) continue;
			src_e = this->Get(r,c);
			dst_e = result->matrix->Get(m_r,m_c);
			dst_e->CopyFrom(src_e);
			m_c++;
		}
		m_r++;
	}
}
//-----------------------------------------------------------------
// Finds the optimal path for the Laplace expansion
void SM::FindExpansionPath()
{
	SN *s;
	unsigned short e_cnt; // empty count

	unsigned short re_max; // maximal row empty terms 
	unsigned short ce_max; // maximal col empty terms 

	unsigned short opt_r = 1; // optimal row
	unsigned short opt_c = 1; // optimal column

	// find optimal row
	re_max = 0; // reset max empty count 
	for (unsigned short r = 1; r <= rank; r++)
	{
		e_cnt = 0;
		for(unsigned short c = 1; c <= rank; c++)
		{	
			s = &Data[r-1][c-1]; // get the matrix element
			if (s->IsEmpty()) e_cnt++;
		}

		if (e_cnt > re_max)
		{
			opt_r = r;
			re_max = e_cnt;
		}
	}

	// find optimal column
	ce_max = 0; // reset max empty count 
	for (unsigned short c = 1; c <= rank; c++)
	{
		e_cnt = 0;
		for(unsigned short r = 1; r <= rank; r++)
		{	
			s = &Data[r-1][c-1]; // get the matrix element
			if (s->IsEmpty()) e_cnt++;
		}

		if (e_cnt > ce_max)
		{
			opt_c = c;
			ce_max = e_cnt;
		}
	}

	if (re_max >= ce_max)  // the row expansion is more optimal 
	{
		exp_row = opt_r;
		exp_col = 0;
	}
	else // the col expansion is more optimal 
	{
		exp_col = opt_c;
		exp_row = 0;
	}

}
//-----------------------------------------------------------------
// Makes a single Laplace expansion via optimal row or column
void SM::LaplaceExpand(Minor *M)
{ 
  // find the optimal path for the first expansion 
  if (exp_level == 0) FindExpansionPath();

  if (exp_level == rank) return; // no further expansion is possible 
  exp_level++; // increase expansion level 

  // update the minor matrix 
  if (exp_row) MakeMinor(exp_row,exp_level,M); 
  if (exp_col) MakeMinor(exp_level,exp_col,M);  

}
//-----------------------------------------------------------------
// prepares the file storage for det data 
void SM::PrepareStorage(const char *file_mode)
{
	// allocate memory for handle table 

	det_storage = (FILE **) calloc(rank+1,sizeof(FILE*));
	det_names = (char **) calloc(rank+1,sizeof(char*));
	
	char prefix[10];
	char postfix[10];

	// prepare file name prefix 
	gc_strncpy(prefix,10,(char *)"det_",5);

	// open the files 
	for (unsigned char i = 0; i <= rank; i++)
	{
		det_storage[i] = NULL; //reset the handle 

		// prepare filename
		det_names[i] = (char *) malloc(20);

		postfix[0] = 0x00;
		det_names[i][0] = 0x00; 		
		
		gc_itoa(i,postfix,10);		
		
		gc_strncat(det_names[i],20,prefix,strlen(prefix));
		gc_strncat(det_names[i],20,postfix,strlen(postfix));

		gc_fopen(&det_storage[i],det_names[i],file_mode);
		if (det_storage[i] == NULL)
		{
			printf("not opened: %s\n",det_names[i]);
			RaiseException(exIO_ERROR);
		}
	}
}
//-----------------------------------------------------------------
// combines resulting det from storage files
void SM::CollectFromStorage(SN *result)
{
	try
	{
		SN *s = new SN();		

		result->Clean();
		s->Clean();
		
		for (unsigned char i=0; i <= rank; i++)
		{
			s->Clean();			
			s->LoadFromRawFile(det_storage[i],i);
			s->Reduce();	
			result->Add(s);			
		}

		delete s;
	}
	catch(SN_Exception &e)
	{
		e.Show();
	}
}
//-----------------------------------------------------------------
// closes active file storage for det results 
void SM::FinalizeStorage()
{
	// close all files
	for (unsigned char i = 0; i <= rank; i++)
	{
		if (det_storage[i] != NULL) fclose(det_storage[i]);
	}
	free (det_storage);

}
//-----------------------------------------------------------------
// deletes storage files 
void SM::CleanupStorage()
{
	for (unsigned char i = 0; i <= rank; i++)
	{
                //gc_unlink(det_names[i]);
		free(det_names[i]);
	}
	free (det_names);
}
//-----------------------------------------------------------------
// Calculates the determinant by Laplace expansion
void SM::Det(FILE *fil,const char *in,const char *out)
{  
  Minor *Minors = (Minor *) calloc(rank+1,sizeof(Minor));

  PrepareStorage("wb"); // prepare files for storing det results

  short dummy; // dummy flag
  short level;   // current expansion level 
  short start_level; // starting expansion level 
  short empty_term; // last expanded term is empty (flag)
  
  while (exp_level < rank)
  {
      
   LaplaceExpand(&Minors[0]);  // self expand one minor 
   
   if (Minors[0].term->IsEmpty()) continue;

   start_level = 0;
   level = 0;
   dummy = 1;
   
 while (dummy)
  {    
	//spawn minors from starting level
    empty_term = 0;
	for (level = start_level; level < rank-1; level++)
	{
		Minors[level].matrix->LaplaceExpand(&Minors[level+1]);

		if (Minors[level+1].term->IsEmpty()) 
		{
			empty_term = 1;
			break;
		}
		Minors[level+1].term->Multiply(Minors[level].term);	  
	}

	if (!empty_term) // the outcome is here
	{				
		Minors[level].term->SplitToRawFiles(det_storage,rank);
	}
	
	// calculate the next level 	
	start_level = level;
	
   while (Minors[start_level].matrix->exp_level == Minors[start_level].matrix->rank)
	{
		start_level--;
		if (start_level == -1)
		{
			dummy = 0;
			break;
		} // if start_level == -1 
	} // while exp_level == rank 

 } // while (dummy)
		
} // for i (passing throughout main matrix)
  
  // release used memory 
  for (int m = 0; m < rank; m++)
  {
	if (Minors[m].matrix != NULL) Minors[m].matrix->Destroy();
	if (Minors[m].term != NULL)  Minors[m].term->Destroy();
  }

  free(Minors);

  FinalizeStorage(); // close det storage files 
  
  // acquire stored data
  try 
  {
	PrepareStorage("rb");  

	SN *result = new SN();
        if (in)
        {
            char buf[200];
            strncpy(buf,in,strlen(in)+1);
            strncat(buf,out,strlen(out)+1);
            result->SetName(buf);
            result->SetIN(in);
            result->SetOUT(out);
            result->SetType(snNum);
        }
        else
        {
             result->SetName("DEN");
             result->SetType(snDen);
        }
	CollectFromStorage(result);	
	result->SaveToFile(fil);
	FinalizeStorage(); // close det storage files 
	CleanupStorage(); // remove temporary storage files
	delete result;	
  }
  catch(...)
  {
	printf("error");
  }
}
//-----------------------------------------------------------------
