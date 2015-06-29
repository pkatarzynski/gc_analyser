#include "param_file.hpp"

//-----------------------------------------------------------------
// the constructor
CParamFile::CParamFile()
{
	lEntries = 0; 	
}
//-----------------------------------------------------------------
// the destructor 
CParamFile::~CParamFile()
{
        Clear();
}
//-----------------------------------------------------------------
// clears the entry set
void CParamFile::Clear()
{
  for (unsigned i = 0; i < lEntries; i++)
   {
         Entries[i].length = 0;
         free(Entries[i].data);
   }
    lEntries = 0;
}	
//-----------------------------------------------------------------	
// adds a new entry to the entry set 
ParamEntry *CParamFile::AddEntry(const char *label,int count = 1)
{
	lEntries++;
	strncpy(Entries[lEntries-1].label,label,strlen(label)+1);	
	Entries[lEntries-1].length = count;
	return &Entries[lEntries-1];
}
//-----------------------------------------------------------------
void CParamFile::SwapSpaces(char *buf)
{
	int len = strlen(buf);
	for (int i = 0; i < len; i++)
	{
		if (buf[i] == 0x20) {buf[i] = 0x0E; continue;}
		if (buf[i] == 0x0E) {buf[i] = 0x20; continue;}
	}	
}
//-----------------------------------------------------------------
int CParamFile::FindEntry(const char *label,int  mandatory=0)
{
	for (unsigned i = 0; i < lEntries; i++)
	{
		if (strcmp(Entries[i].label,label) == 0) 
		{
			return i;
		}
	}
        if (mandatory) throw new gcError("Unable to find entry %s",label);
	return -1;
}
//-----------------------------------------------------------------
// put integer
void CParamFile::Put_i(const char *label, int data)
{
	ParamEntry *p = AddEntry(label);	
	int *dummy = new int;	
	*dummy = data;	
	p->data = dummy;
	p->type = ptInt;
}
//-----------------------------------------------------------------
// put integer vector 
void CParamFile::Put_iv(const char *label,int cnt, int *data)
{	
	ParamEntry *p = AddEntry(label,cnt);	
	int *dummy = new int[cnt];	
	memcpy(dummy,data,cnt*sizeof(int));		
	p->data = dummy;	 
	p->type =  ptIntVector;
}
//-----------------------------------------------------------------
// put float
void CParamFile::Put_f(const char *label, float data)
{
	ParamEntry *p = AddEntry(label);	
	float *dummy = new float;	
	*dummy = data;	
	p->data = dummy;	 
	p->type =  ptFloat;
}
//-----------------------------------------------------------------
// put double
void CParamFile::Put_d(const char *label, double data)
{
        ParamEntry *p = AddEntry(label);
        double *dummy = new double;
        *dummy = data;
        p->data = dummy;
        p->type =  ptDouble;
}
//-----------------------------------------------------------------
// put float vector 
void CParamFile::Put_fv(const char *label,int cnt, float *data)
{	
	ParamEntry *p = AddEntry(label,cnt);	
	float *dummy = new float[cnt];	
	memcpy(dummy,data,cnt*sizeof(int));		
	p->data = dummy;	 
	p->type =  ptFloatVector;
}
//-----------------------------------------------------------------
// put double vector
void CParamFile::Put_dv(char *label,int cnt, double *data)
{
        ParamEntry *p = AddEntry(label,cnt);
        double *dummy = new double[cnt];
        memcpy(dummy,data,cnt*sizeof(double));
        p->data = dummy;
        p->type =  ptDoubleVector;
}
//-----------------------------------------------------------------
// put string
void CParamFile::Put_s(const char *label, char *data)
{
	ParamEntry *p = AddEntry(label);		
	char *dummy = (char *) calloc(strlen(data),sizeof(char));	
	strncpy(dummy,data,strlen(data));	
	p->data = dummy;		
	p->type =  ptString;
}
//-----------------------------------------------------------------
// gets an integer
int CParamFile::Get_i(const char *label,int *data,int mandatory=0)
{
	int idx = FindEntry(label); 
        if ((mandatory) && (idx < 0)) throw new gcError("Unable to find parameter %s",label);
	if (idx >= 0) *data = *((int *) Entries[idx].data);
	return idx;
}
//-----------------------------------------------------------------
// gets a character string 
int CParamFile::Get_s(const char *label,char **data,int mandatory=0)
{
        int idx = FindEntry(label,mandatory);
	if (idx >= 0) *data = ((char *) Entries[idx].data);
	return idx;
}
//-----------------------------------------------------------------
// gets a float data
int CParamFile::Get_f(const char *label,float *data,int mandatory=0)
{
        int idx = FindEntry(label,mandatory);
	if (idx >= 0) *data = *((float *) Entries[idx].data);
	return idx;
}
//-----------------------------------------------------------------
// gets a double data
int CParamFile::Get_d(const char *label,double *data,int mandatory=0)
{        
        int idx = FindEntry(label,mandatory);

        if (idx < 0) return idx;

        // found float entry instead
        if (Entries[idx].type == ptFloat)
        {
            float buf = *((float *) Entries[idx].data);
             *data = (double)buf;
        }
        else
        {
            *data = *((double *) Entries[idx].data);
        }
        return idx;
}
//-----------------------------------------------------------------
// gets a float vector 
int CParamFile::Get_fv(const char *label,int *cnt, float **data,int mandatory=0)
{
        int idx = FindEntry(label,mandatory);
	if (idx >= 0)
		{
			*cnt = Entries[idx].length;
			memcpy(*data,(float *) Entries[idx].data,sizeof(float)*Entries[idx].length);
		}
	return idx;
}
//-----------------------------------------------------------------
// gets a double vector
int CParamFile::Get_dv(const char *label,int *cnt, double **data,int mandatory=0)
{
        int idx = FindEntry(label,mandatory);
        if (idx < 0) return idx; // nothing was found

        *cnt = Entries[idx].length;

        if (Entries[idx].type == ptFloatVector)
        {
            float *buff = new float[*cnt];
            double *bufd = new double[*cnt];

            memcpy(buff,Entries[idx].data,sizeof(float)*Entries[idx].length);

            for (int i = 0; i < Entries[idx].length; i++) bufd[i] = double(buff[i]);

            memcpy(*data,bufd,sizeof(double)*Entries[idx].length);

            delete buff;
            delete bufd;

        }
        else
        {
            memcpy(*data,Entries[idx].data,sizeof(double)*Entries[idx].length);
        }

        return idx;
}
//-----------------------------------------------------------------
// gets an integer vector 
int CParamFile::Get_iv(const char *label,int *cnt, int **data,int mandatory=0)
{
        int idx = FindEntry(label,mandatory);
	if (idx >= 0)
		{
			*cnt = Entries[idx].length;
			memcpy(*data,(int *) Entries[idx].data,sizeof(int)*Entries[idx].length);
		}
	return idx;
}
//-----------------------------------------------------------------
// displays the dataset
void CParamFile::Display()
{
	printf("The dataset:\n");

	for (unsigned i = 0; i < lEntries; i++)
	{
		switch(Entries[i].type)
		{
			case ptInt:
				{
					int *p = (int *) Entries[i].data;
					printf("%s = %d\n",Entries[i].label,*p);
					break;
				}
			case ptFloat:
				{
					float *p = (float *) Entries[i].data;
					printf("%s = %f\n",Entries[i].label,*p);
					break;
				}

                        case ptDouble:
                        {
                                double *p = (double *) Entries[i].data;
                                printf("%s = %f\n",Entries[i].label,*p);
                                break;
                        }

				
			case ptString:
				{
					char *p = (char *) Entries[i].data;
					printf("%s = %s\n",Entries[i].label,p);
					break;
				}				
				
			case ptIntVector:
				{
					int *p = (int *) Entries[i].data;					
					printf("%s [%d] = ",Entries[i].label,Entries[i].length);
					for (int j = 0; j < Entries[i].length; j++) printf("%d,",*(p+j));
					printf("\n");
					break;
				}				

			case ptFloatVector:
				{
					float *p = (float *) Entries[i].data;					
					printf("%s [%d] = ",Entries[i].label,Entries[i].length);
					for (int j = 0; j < Entries[i].length; j++) printf("%f,",*(p+j));
					printf("\n");
					break;
				}				

                        case ptDoubleVector:
                                {
                                    double *p = (double *) Entries[i].data;
                                    printf("%s [%d] = ",Entries[i].label,Entries[i].length);
                                    for (int j = 0; j < Entries[i].length; j++) printf("%f,",*(p+j));
                                    printf("\n");
                                    break;
                                }
				
			default:
				printf("%s = <unknown>\n",Entries[i].label); break;			
		}		
	}
}
//-----------------------------------------------------------------
// Saves dataset to a file 
void CParamFile::SaveToFile(const char *filename)
{
	FILE *fil = NULL;
        fil  = fopen(filename,"w+");
	
	fprintf(fil,"CCC: Param File version: %d\n",VERSION_NUMBER);
	
	for (unsigned i = 0; i < lEntries; i++)
	{
		fprintf(fil,"PAR: %d %s %d ",Entries[i].type, Entries[i].label,Entries[i].length);
		
		switch(Entries[i].type)
		{
			case ptInt: fprintf(fil,"%d",*((int *)Entries[i].data)); break;
                        case ptFloat: fprintf(fil,"%5.15f",*((float *)Entries[i].data)); break;
                        case ptDouble: fprintf(fil,"%5.15f",*((double *)Entries[i].data)); break;
			case ptString:
			{
				char buf[BUFFER_LEN];
                                strncpy(buf,(char *)Entries[i].data,Entries[i].length+1);
				SwapSpaces(buf);
				fprintf(fil,"%s",buf); break;			
			}
			case ptIntVector: for (int j=0; j<Entries[i].length; j++) fprintf(fil,"%d ",*((int *)(Entries[i].data)+j)); break;
                        case ptFloatVector: for (int j=0; j<Entries[i].length; j++) fprintf(fil,"%5.15f ",*((float *)(Entries[i].data)+j)); break;
                        case ptDoubleVector: for (int j=0; j<Entries[i].length; j++) fprintf(fil,"%5.15f ",*((double *)(Entries[i].data)+j)); break;
		}		
		fprintf(fil," #\n");
	}	
	if (fil) fclose(fil);
}
//-----------------------------------------------------------------
// Loads the dataset from file 
int CParamFile::LoadFromFile(const char *filename)
{
	FILE *fil = NULL;
	fil = fopen(filename,"r");
	
        if (fil == NULL) throw new gcError("Unable to access ",filename);

        Clear();
	
	char buf[BUFFER_LEN];
	
	char param_type = 0;	
	unsigned length = 0;
	
	char label[BUFFER_LEN];
	
	while (!feof(fil))
	{
		// read the invocation
                buf[0] = 0x00;
		fscanf(fil,"%s",buf); 
	
		// unknown invocation
		if (strcmp(buf,"PAR:") != 0) 
		{		
			fgets(buf,BUFFER_LEN,fil); // read and ignore the rest
			continue;			
		}
				
		// read the param type
                buf[0] = 0x00;
		fscanf(fil,"%s",buf); 
		param_type = atoi(buf);
		
		// unknown parameter type 
		if ( ( param_type <= 0) || (param_type > ptMaxInd)) 
		{
			fgets(buf,BUFFER_LEN,fil); // read and ignore the rest
			continue;
		}							
		
                // read the label
                label[0] = 0x00;
		fscanf(fil,"%s",label); 
		
		// invalid label 
		if (strlen(label) < 1) 
		{
			fgets(buf,BUFFER_LEN,fil); // read and ignore the rest
			continue;
		}
		
                // read the length

		fscanf(fil,"%d",&length); 
		
		// invalid length 
		if (length < 1) 
		{
			fgets(buf,BUFFER_LEN,fil); // read and ignore the rest
			continue;
		}
		
		switch(param_type)
		{
			// --------------------------------------
			case ptInt:
			{
				int dummy;
				fscanf(fil,"%d",&dummy);
				Put_i(label,dummy);
				break;
			}
			// --------------------------------------
			case ptFloat:
			{
				float dummy;
				fscanf(fil,"%f",&dummy);
				Put_f(label,dummy);
				break;
			}
                        // --------------------------------------
                        case ptDouble:
                        {
                                double dummy;
                                fscanf(fil,"%lf",&dummy);
                                Put_d(label,dummy);
                                break;
                        }
			// --------------------------------------
			case ptString:
			{			
				char dummy[BUFFER_LEN];				
                                dummy[0] = 0x00;
				fscanf(fil,"%s",dummy);
				SwapSpaces(dummy);
                                Put_s(label,dummy);
				break;
			}
			// --------------------------------------
			case ptIntVector:
			{
				int *dummy = new int[length];
				int buf;
				for (unsigned int i = 0; i < length; i++)
				{
					fscanf(fil,"%d",&buf);
					dummy[i] = buf;
				}
				Put_iv(label,length,dummy);
				delete dummy;
				break;
			}
			// --------------------------------------
                        case ptFloatVector:
                        {
                                float *dummy = new float[length];
                                float buf;
                                for (unsigned int i = 0; i < length; i++)
                                {
                                        fscanf(fil,"%f",&buf);
                                        dummy[i] = buf;
                                }
                                Put_fv(label,length,dummy);
                                delete dummy;
                                break;
                        }
                        // --------------------------------------
                        case ptDoubleVector:
			{
                                double *dummy = new double[length];
                                double buf;
				for (unsigned int i = 0; i < length; i++)
				{
                                        fscanf(fil,"%lf",&buf);
					dummy[i] = buf;
				}
				Put_dv(label,length,dummy);
				delete dummy;
				break;
			}
			// --------------------------------------
			
		} // end of switch 
		
		fgets(buf,BUFFER_LEN,fil); // read and ignore the rest		
	} // while 
		
	fclose(fil);	
	return 0;
}
//-----------------------------------------------------------------
// The End
//-----------------------------------------------------------------
