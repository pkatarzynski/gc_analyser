#include "circuit.h"
#include "time.h"
//-------------------------------------------------------------
// Default constructor for the class
CCircuit::CCircuit()
{
        Elements = NULL;
        Params = NULL;
	Initialize();
}
//-------------------------------------------------------------
// Default destructor 
CCircuit::~CCircuit()
{
	if (Params != NULL) free(Params);
	if (Elements != NULL) free(Elements);
}
//-------------------------------------------------------------
// Initialization procedure 
int CCircuit::Initialize()
{	
	lVertices = 0; 
	lBlocks = 0; 
        lElements = 0;
        if (Elements) free(Elements);
        if (Params) free(Params);
	comment_chr = '%';	
	return 0; 
}
//-------------------------------------------------------------
//assigns names for the elements
int CCircuit::AssignName(TElementType type,char *output)
{
  
  int ind=0;
  for (int i=0; i<lElements; i++)
  {
	  if (Elements[i].type==type) ind++;
  }
  ind++;

  gc_itoa(ind,output+1,10);
  
  switch(type)
  {
   case etCapacitor:{output[0]='C';	break;}
   case etGyrator:{output[0]='g';	break;}
   case etConductance:{output[0]='G';	break;}
   default: {output[0]='x';	break;}
  }
 
  return 0;
}
//-------------------------------------------------------------
// Adds a generic element of a type spanned between vin and vout 
int CCircuit::AddGeneric(TElementRole role,short vin,short vout,short block_id,short sign=1)
{
	if (lElements==0)
	{
		Elements = (TElement *) malloc(sizeof(TElement)*(lElements+1));
	}

	lElements++;
	Elements = (TElement *) realloc(Elements,sizeof(TElement)*(lElements+1));

	TElement *el = &Elements[lElements-1];	

        // determine the role basing on type
        switch(role)
        {
            case erCi:
            case erCj:
            case erCf:
                el->type = etCapacitor;
                break;
            case erIg:
                el->type = etGyrator;
                break;
            case erGi1:
            case erGi2:
            case erGj1:
            case erGj2:
                el->type = etConductance;
                break;
        default: el->type = etAny; break;
        }

        AssignName(el->type,el->name);

        el->alias[0] = 'x';
        gc_itoa(lElements,el->alias+1,10);

	el->index = lElements;
	el->vin = vin;
	el->vout = vout;
	el->value = 0.0;
	el->sign = sign;
        el->role = role;
	el->block_id = block_id;

        if ((el->role == erGi1)||(el->role == erGi2)||(el->role == erGj1)||(el->role == erGj2))
        {
            strncpy(el->sig,el->alias,strlen(el->alias)+1);
        }

	return 0;
}
//-------------------------------------------------------------
// parses a string to get additional signal names for elements
int CCircuit::ParseBlockNames(const char *str)
{    
    int len = strlen(str);

    char buf[200];
    char key[200];    

    int lbuf = 0;

    short ind = 0;
    char i_in[200]; i_in[0] = 0x00;
    char i_out[200];i_out[0] = 0x00;
    char j_in[200]; j_in[0] = 0x00;
    char j_out[200]; j_out[0] = 0x00;

    for (int i = 0; i < len; i++)
    {        

        if (str[i] == ' ')
        {
            if(lbuf)
            {
                buf[lbuf]=0x00;
                //printf("value = '%s'\n",buf);
                lbuf = 0;

                if (strcmp(key,"B") == 0) ind = atoi(buf);
                if (strcmp(key,"i_in") == 0) strncpy(i_in,buf,strlen(buf)+1);
                if (strcmp(key,"i_out") == 0) strncpy(i_out,buf,strlen(buf)+1);
                if (strcmp(key,"j_in") == 0) strncpy(j_in,buf,strlen(buf)+1);
                if (strcmp(key,"j_out") == 0) strncpy(j_out,buf,strlen(buf)+1);
            }
            continue;
        }

        if (str[i] == ':')
        {
            if(lbuf)
            {
                buf[lbuf]=0x00;
                strncpy(key,buf,strlen(buf)+1);                
                lbuf = 0;
            }
            continue;
        }
        buf[lbuf++] = str[i];
    }

    // assign collected names to the corresponding elements

    int i;
    if (ind <= 0) return 0;
    if (strlen(i_in) > 0)
    {
        i = GetElement(erGi1,ind);
        if (i >= 0) strncpy(Elements[i].sig,i_in,strlen(i_in)+1);
    }

    if (strlen(i_out) > 0)
    {
        i = GetElement(erGi2,ind);
        if (i >= 0) strncpy(Elements[i].sig,i_out,strlen(i_out)+1);
    }

    if (strlen(j_in) > 0)
    {
        i = GetElement(erGj1,ind);
        if (i >= 0) strncpy(Elements[i].sig,j_in,strlen(j_in)+1);
    }

    if (strlen(j_out) > 0)
    {
        i = GetElement(erGj2,ind);
        if (i >= 0) strncpy(Elements[i].sig,j_out,strlen(j_out)+1);
    }

    return 0;
}
//-------------------------------------------------------------
// parses a string to get an array of integer data
int CCircuit::ParseString(const char *str,int *cnt, int *data)
{
    *cnt = 0;
    int len = strlen(str);

    char buf[200];
    int lbuf = 0;

    for (int i = 0; i < len; i++)
    {
        if (str[i] == '%')
        {
            if(lbuf)
            {
                buf[lbuf]=0x00;
                data[*cnt] = atoi(buf);
                (*cnt)++;
                lbuf = 0;
            }
            return 0; // skip rest of the line when comment is found
        }

        if (str[i] == ' ')
        {
            if(lbuf)
            {
                buf[lbuf]=0x00;
                data[*cnt] = atoi(buf);
                (*cnt)++;
                lbuf = 0;
            }
            continue;
        }

        if (str[i] == 0x0A) break;
        if (str[i] == 0x0D) break;

        if ((str[i] < 45)||(str[i] > 57)) return 1;

        buf[lbuf] = str[i];
        lbuf++;
    }

    if ((lbuf) && (buf[0]!=' '))
    {
        buf[lbuf]=0x00;
        data[*cnt] = atoi(buf);
        (*cnt)++;
        lbuf = 0;
    }

    return 0;
}
//-------------------------------------------------------------

//Parses the blockfile and loads the block structure 
int CCircuit::ParseBlockFile(char *filename)
{
	FILE *fil = NULL;
			
	gc_strncpy(blo_filename,sizeof(blo_filename),filename,strlen(filename));	
	gc_fopen(&fil,blo_filename,(char *)"r"); // try to open the file for reading 
		
        if (fil==NULL){ fprintf(stdout,"File not found %s",blo_filename); fflush(stdout); throw new gcError("File not found: %s ",filename);  return -1; 	}

        fprintf(stdout,"Processing: %s\n",blo_filename);

        char buf[1024];
        int *data = new int[200];
        int cnt;

        int line = 0;
        int first = 1;

        //temporary storage for block content
        int ci,cj,cf,g,Gi1,Gi2,Gj1,Gj2;
        int ind;
        int vin;
        int vout;        

        int *bl = NULL;

        while(!feof(fil))
        {
            buf[0] = 0x00;
            fgets(buf,1024,fil);            
            line++;


            if ( (buf[0]=='%') && (buf[1] == '!'))
            {
                ParseBlockNames(buf+2);
                continue;
            }

            if (ParseString(buf,&cnt,data))
            {
                fprintf(stdout,"\tERROR line %d : Invalid syntax\n",line);
                fflush(fil);
                return 1;
            }

            if ( (first) && (cnt)) // obtain block and vertex count
            {
                if (cnt != 2) {fprintf(stdout,"\tERROR line %d : Incomplete block count information\n",line); fflush(fil); return 1;}
                lBlocks = data[0];
                if (lBlocks <= 0) {fprintf(stdout,"\tERROR line %d : Invalid blocks count\n",line); fflush(fil); return 1;}
                lVertices = data[1];
                if (lVertices < 2) {fprintf(stdout,"\tERROR line %d : Invalid vertices count\n",line); fflush(fil); return 1;}

                bl = new int[lBlocks+1];
                for (int i = 0; i <= lBlocks; i++) bl[i] = 0;
                first = 0;
                continue;
            }

            if (cnt == 0) continue; // no block data so get the next line
            if (data[cnt-1] != 0) {fprintf(stdout,"\tERROR line %d : Expected 0 at the end of the line.\n",line); fflush(fil); return 1;}
            if (cnt < 12) {fprintf(stdout,"\tERROR line %d : Too few block data\n",line); fflush(fil); return 1;}

            ci = data[0];
            if ((ci < 0 ) || (ci > 1)) {fprintf(stdout,"\tERROR line %d : Expected 0 or 1 as the Ci value.\n",line); fflush(fil); return 1;}
            cj = data[1];
            if ((cj < 0 ) || (cj > 1)) {fprintf(stdout,"\tERROR line %d : Expected 0 or 1 as the Cj value.\n",line); fflush(fil); return 1;}
            cf = data[2];
            if ((cf < 0 ) || (cf > 1)) {fprintf(stdout,"\tERROR line %d : Expected 0 or 1 as the Cf value.\n",line); fflush(fil); return 1;}
            g = data[3];
            if ((g < -1 ) || (g > 1)) {fprintf(stdout,"\tERROR line %d : Expected 0 or +//- 1 as the g value.\n",line); fflush(fil); return 1;}
            Gi1 = data[4];
            if ((Gi1 < 0 ) || (Gi1 > 1)) {fprintf(stdout,"\tERROR line %d : Expected 0 or 1 as the Gi1 value.\n",line); fflush(fil); return 1;}
            Gi2 = data[5];
            if ((Gi2 < 0 ) || (Gi2 > 1)) {fprintf(stdout,"\tERROR line %d : Expected 0 or 1 as the Gi2 value.\n",line); fflush(fil); return 1;}
            Gj1 = data[6];
            if ((Gj1 < 0 ) || (Gj1 > 1)) {fprintf(stdout,"\tERROR line %d : Expected 0 or 1 as the Gj1 value.\n",line); fflush(fil); return 1;}
            Gj2 = data[7];
            if ((Gj2 < 0 ) || (Gj2 > 1)) {fprintf(stdout,"\tERROR line %d : Expected 0 or 1 as the Gj2 value.\n",line); fflush(fil); return 1;}

            for (int i = 8; i < cnt; i+=3)            
            {                

                ind = data[i]; // read the block index

                if (ind == 0) continue;

                if ((ind < 0)||(ind > lBlocks)){fprintf(stdout,"\tERROR line %d : Block index out of bounds (%d).\n",line,ind); fflush(fil); return 1;}
                if (bl[ind]){fprintf(stdout,"\tERROR line %d : Block index %d already assigned.\n",line,ind); fflush(fil); return 1;}
                bl[ind] = 1;

                vin = data[i+1]; // read the block input and output vertices
                vout = data[i+2];

                if ((vin <=0)||(vin > lVertices)){fprintf(stdout,"\tERROR line %d : Vertex index out of bounds (%d).\n",line,vin); fflush(fil); return 1;}
                if ((vout <=0)||(vout > lVertices)){fprintf(stdout,"\tERROR line %d : Vertex index out of bounds (%d).\n",line,vout); fflush(fil); return 1;}

                if (ci) AddGeneric(erCi,vin,0,ind,1);
                if (cj) AddGeneric(erCj,vout,0,ind,1);
                if (cf) AddGeneric(erCf,vin,vout,ind,1);
                if (g) AddGeneric(erIg,vin,vout,ind,g);
                if (Gi1) AddGeneric(erGi1,0,vin,ind,1);
                if (Gi2) AddGeneric(erGi2,vin,0,ind,1);
                if (Gj1) AddGeneric(erGj1,0,vout,ind,1);
                if (Gj2) AddGeneric(erGj2,vout,0,ind,1);
            }

        }

        delete data;
        delete bl;
        fclose(fil);

	return 0;
}
//-------------------------------------------------------------
// loads the AMS model
int CCircuit::LoadAMSModel(const char *filename)
{
    AMSParser *ap = new AMSParser();

    char output_file[1024];
    output_file[0] = 0x00;
    strncpy(output_file,filename,strlen(filename)+1);
    if (strlen(output_file)>=4) output_file[strlen(output_file)-4] = 0x00;
    strcat(output_file,".gcb");

    try
    {
        if (ap->Parse((filename)))
        {
            ap->ShowBlocks();

            if ((ap->HasBlocks() == 1) && (ap->HasComponents() == 0)) // blocks only
            {
                printf("This AMS file contains block definitions only it will be converted to gcb.\n");
                ap->SaveAsBLO(output_file);
                ParseBlockFile(output_file);                
            }

            if ((ap->HasBlocks() == 0) && (ap->HasComponents() == 1)) // components only
            {
                printf("This AMS file contains discrete components only.\n Performing multiport multidimensional analysis.\n");

                lElements = ap->el_cnt;
                if (Elements)
                {
                    free(Elements);
                    Elements=NULL;
                }
                Elements = (TElement *)calloc(lElements,sizeof(TElement));
                if (Elements)
                {
                    memcpy(Elements,ap->el,sizeof(TElement)*lElements);
                }
                lVertices = ap->v_cnt;
                return 0;
            }

            if ((ap->HasBlocks() == 1) && (ap->HasComponents() == 1)) // both
            {
                printf("Hybrid AMS with components and blocks detected. This format is not currently supported.\n");
                return 1;
            }

            if ((ap->HasBlocks() == 0) && (ap->HasComponents() == 0)) // none
            {
                printf("Neither supported blocks nor components found in the AMS file. Skipping.\n");
                return 1;
            }
        }
        else
        {
            printf("No supported AMS format found. Will try to parse file as generic block file.\n");
            fflush(stdout);
            ap->CopyAsBLO(filename,output_file);
            ParseBlockFile(output_file);
        }
    }
    catch (AMSError *e)
    {
        e->Display();
        return 1;
    }
    catch (ParseError *pe)
    {
        pe->Display(stdout);
        return 1;
    }

    delete ap;
    return 0;
}

//-------------------------------------------------------------
// arranges the elements in the 'storage' space 
// class_id denotes source class id
// iterator gives following numers for aliasses 
// index gives following indices for elements of the given class 
int CCircuit::ReorderElements(int &iterator,int &index, TElementRole role,TElement *storage)
{
   char prefix=' ';

   switch(role)
   {
                default: prefix='x'; break;
                case erCi: prefix='C'; break;
                case erCj: prefix='C'; break;
                case erCf: prefix='C'; break;
                case erGi1: prefix='G'; break;
                case erGi2: prefix='G'; break;
                case erGj1: prefix='G'; break;
                case erGj2: prefix='G'; break;
                case erIg: prefix='g'; break;
   }

   for (int b=0; b<lBlocks; b++)
   {	   
	   for (int i=0; i < lElements; i++) 
	   {
		   if (Elements[i].block_id != b+1) continue;
                   if (Elements[i].role == role)
		   {
		    memcpy((TElement *)&storage[iterator++],(TElement *)&Elements[i],sizeof(TElement));

		    index++;
		    storage[iterator-1].name[0]=prefix;
		    gc_itoa(index,storage[iterator-1].name+1,10);

		    storage[iterator-1].alias[0]='x';
		    gc_itoa(iterator,storage[iterator-1].alias+1,10);
			storage[iterator-1].index = iterator;
		   }
	   }
   }
	return 0;
}
//-------------------------------------------------------------
// produces the blockfile.tmp having current block structure 
int CCircuit::MakeBlockFile()
{
	FILE *fil;
	gc_fopen(&fil,(char *)"blockfile.tmp",(char *)"w");
        ListBlocks(fil);
	fclose(fil);
	return 0;
}

//-------------------------------------------------------------
// arranges all elements in CI CJ CF IG GI GJ order
int CCircuit::SortElements()
{
 // temporary storage
   TElement *buffer = new TElement[lElements];

   int index=0;
   int iterator=0;

        ReorderElements(iterator,index,erCi,buffer);
	index=0;
        ReorderElements(iterator,index,erCj,buffer);
	index=0;
        ReorderElements(iterator,index,erCf,buffer);
	index=0;
        ReorderElements(iterator,index,erIg,buffer);
	index=0;
        ReorderElements(iterator,index,erGi1,buffer);
	index=0;
        ReorderElements(iterator,index,erGi2,buffer);
	index=0;
        ReorderElements(iterator,index,erGj1,buffer);
	index=0;
        ReorderElements(iterator,index,erGj2,buffer);

	//swap storage spaces 
   delete Elements;
   Elements = buffer;
return 0;
}

//-------------------------------------------------------------
// composes a list of elements with corresponding values attached 
void CCircuit::ListElements(FILE *fil,const char *prefix)
{
	for (int i = 0; i < lElements; i++)
	{                        
            if (prefix != NULL) printf("%s",prefix);
            switch(Elements[i].role)
            {
                case erCi: fprintf(fil,"Ci\t"); break;
                case erCj: fprintf(fil,"Cj\t"); break;
                case erCf: fprintf(fil,"Cf\t"); break;
                case erGi1: fprintf(fil,"Gi1\t"); break;
                case erGi2: fprintf(fil,"Gi2\t"); break;
                case erGj1: fprintf(fil,"Gj1\t"); break;
                case erGj2: fprintf(fil,"Gj2\t"); break;
                case erIg: fprintf(fil,"Ig\t"); break;
                default:   fprintf(fil,"Unknown\t"); break;
            }
            //fprintf(fil,"%s\t alias %s:\t sign=%d VALUE=%f\t %d\t %d\t BLOCK=%d",Elements[i].name,Elements[i].alias,Elements[i].sign,Elements[i].value,Elements[i].vin,Elements[i].vout,Elements[i].block_id);
            fprintf(fil,"%s\t alias %s:\t sign=%d %d\t %d\t",Elements[i].name,Elements[i].alias,Elements[i].sign,Elements[i].vin,Elements[i].vout);
            if ((Elements[i].role == erGi1)||(Elements[i].role == erGi2)||(Elements[i].role == erGj1)||(Elements[i].role == erGj2))
            {
                fprintf(fil,"\tsignal=%s",Elements[i].sig);
            }
            if (Elements[i].type == etCapacitor)
                fprintf(fil,"dim=%d ",Elements[i].dim);
            fprintf(fil,"\n");
	}
}
//-------------------------------------------------------------
// makes a list of blocks avaiable in the current circuit
int CCircuit::ListBlocks(FILE *fil)
{
	int i=0;
	int g=0; 
	int c=0; 

	short vin = 0;
	short vout = 0;

        fprintf(fil,"%c",comment_chr);
        fprintf(fil,"\tCi\tCj\tCf\tIg\tGi1\tGi2\tGj1\tGj2\tNi\tNj\n");

	for (int b = 0; b < lBlocks; b++) // pass through block indices 
	{
		// determine block span nodes by finding feedback capacitors or gyrators 
                c = GetElement(erCf,b+1);
                g = GetElement(erIg,b+1);
		if ( c>=0 ) {vin = Elements[c].vin; vout = Elements[c].vout;}
		if ( g>=0 ) {vin = Elements[g].vin; vout = Elements[g].vout;}

		fprintf(fil,"%c# B%d:\t",comment_chr,b+1);
		
                i = GetElement(erCi,b+1);
			if (i >=0) fprintf(fil,"%s\t",Elements[i].alias+1); else fprintf(fil,"0\t");
                i = GetElement(erCj,b+1);
			if (i >=0) fprintf(fil,"%s\t",Elements[i].alias+1); else fprintf(fil,"0\t");
                i = GetElement(erCf,b+1);
			if (i >=0) fprintf(fil,"%s\t",Elements[i].alias+1); else fprintf(fil,"0\t");
                i = GetElement(erIg,b+1);
			if (i >=0) fprintf(fil,"%s\t",Elements[i].alias+1); else fprintf(fil,"0\t");
                i = GetElement(erGi1,b+1);
			if (i >=0) fprintf(fil,"%s\t",Elements[i].alias+1); else fprintf(fil,"0\t");
                i = GetElement(erGi2,b+1);
			if (i >=0) fprintf(fil,"%s\t",Elements[i].alias+1); else fprintf(fil,"0\t");
                i = GetElement(erGj1,b+1);
			if (i >=0) fprintf(fil,"%s\t",Elements[i].alias+1); else fprintf(fil,"0\t");
                i = GetElement(erGj2,b+1);
			if (i >=0) fprintf(fil,"%s\t",Elements[i].alias+1); else fprintf(fil,"0\t");
		
		fprintf(fil,"%d\t%d",vin,vout);	
	
                fprintf(fil,"\n");

	} // for b

	return 0;
}

//-------------------------------------------------------------
// gets an element of given class id from given block 
int CCircuit::GetElement(TElementRole role,short block_id)
{
	for (int i = 0; i < lElements; i++)
	{
                if ((Elements[i].block_id == block_id) && (Elements[i].role == role)) return i;
	}
	return -1;
}

//-------------------------------------------------------------
// gets an element spanned between vin and vout
int CCircuit::GetElement(TElementRole role,short vin, short vout,short *start_ind)
{
// range constraints	
//TODO: raise exceptions here
if (*start_ind < 0){*start_ind = 0;}

if (int(*start_ind) > lElements) *start_ind = lElements;

for (int i=*start_ind; i<lElements; i++)
 {	 
	 
         if ((vin==0) || (vout == 0))
	 {
                 if ((vin == 0) && (Elements[i].vout == vout) && (Elements[i].role == role))
		{
			 *start_ind = i; 
			 return 1;
		}

                if ((Elements[i].vin == vin) && (vout == 0) && (Elements[i].role == role))
		{
			 *start_ind = i; 
			 return 1;
		}

	 }
	 else
	 {
                if ((Elements[i].vin == vin) && (Elements[i].vout == vout) && (Elements[i].role == role))
		{
			*start_ind = i; 
			return 1;
		}
	 }

 }
return 0;
}

//-------------------------------------------------------------
// converts an element into tree
void CCircuit::ConvertToTree(TElement *element,short vin,short vout,sn_tree *t)
{
	
short sign = element->sign;

if (vin!=vout) // out of diagonal elements
{
	sign = -1*element->sign; 
        if (element->role==erIg) // gyrator with inverted orientation
	{
		if ((element->vin==vin) && (element->vout=vout)) sign = -sign;
	}
}

if (vin == 0) sign = 1; // input elements

if (vout == 0) sign = 1; // output elements

t->AddItem(element->index);

t->SetMult(sign);

if (element->type == etCapacitor) t->SetRank(1,element->dim);

}
//-------------------------------------------------------------
// finds output nodes 
int CCircuit::GetOutputNodes(short *count,short *nodes)
{
	*count = 0;
 	for (int e=0; e<lElements; e++) // pass through all elements 
	{
                if ((Elements[e].role == erGi2) || (Elements[e].role == erGj2) ) // output element was found
		{
			nodes[(*count)++] = Elements[e].vin;
		}
	}
	return 0;
}
//-------------------------------------------------------------
// finds input nodes
int CCircuit::GetInputNodes(short *count,short *nodes)
{
	*count = 0;
 	for (int e=0; e<lElements; e++) // pass through all elements 
	{
                if (Elements[e].role == erGi1) // input element was found
		{
                        nodes[(*count)++] = Elements[e].vout;
		}

                if (Elements[e].role == erGj1) // input element was found
                {
                        nodes[(*count)++] = Elements[e].vin;
                }
	}
	return 0;
}
//-------------------------------------------------------------
// generates the symbolic matrix of node potentials 
int CCircuit::NodePotentials(SM *M,short on,TElement *ie)
{

M->Clean(); // perform the cleanup 
SN *s;
sn_tree *t;
	
short ind=0;

for(short r=1;r <= lVertices;r++) // loop through rows 
   {	   
	   for (short c=1; c <= lVertices; c++) // loop through columns
	   {	   
		   ind=-1;


		   // if non zero oputput node is defined
                        if (c==on)
			{
				 // replace the output column with the input one 
                                 //while ( GetElement(lVertices+1,r,&(++ind)) )
                                //{
                                    if ((ie->role == erGi1) && (r == ie->vout))
                                    {
                                        s = M->Get(ie->vout,c);
                                        t = s->AddTree();
                                        ConvertToTree(ie,0,ie->vout,t);
                                    }

                                    if ((ie->role == erGj1) && (r == ie->vin))
                                    {
                                        s = M->Get(ie->vin,c);
                                        t = s->AddTree();
                                        ConvertToTree(ie,ie->vin,0,t);
                                    }

                                //}
			 continue;
			} // if c == output_node



			// DIAGONAL ELEMENTS
			if (r==c)
                        {
                                ind=-1;
                                // grounded Ci
                                while (GetElement(erCi,r,0,&(++ind)))
				{					
					s = M->Get(r,c);
					t = s->AddTree();
					ConvertToTree(&Elements[ind],r,c,t);
				}
                                ind=-1;
                                // grounded Cj
                                while (GetElement(erCj,r,0,&(++ind)))
                                {
                                        s = M->Get(r,c);
                                        t = s->AddTree();
                                        ConvertToTree(&Elements[ind],r,c,t);
                                }
                                ind=-1;
                                //  Cf
                                while (GetElement(erCf,r,0,&(++ind)))
                                {
                                        s = M->Get(r,c);
                                        t = s->AddTree();
                                        ConvertToTree(&Elements[ind],r,c,t);
                                }
                                ind=-1;
                                //  Cf
                                while (GetElement(erCf,0,r,&(++ind)))
                                {
                                        s = M->Get(r,c);
                                        t = s->AddTree();
                                        ConvertToTree(&Elements[ind],r,c,t);
                                }
                                ind=-1;
                                // input Gi1
                                while (GetElement(erGi1,0,r,&(++ind)))
				{
					s = M->Get(r,c);
					t = s->AddTree();
                                        ConvertToTree(&Elements[ind],0,r,t);
				}
                                ind=-1;
                                // input Gj1
                                while (GetElement(erGj1,r,0,&(++ind)))
                                {
                                        s = M->Get(r,c);
                                        t = s->AddTree();
                                        ConvertToTree(&Elements[ind],r,0,t);
                                }
                                ind=-1;
                                // output Gi2
                                while (GetElement(erGi2,r,0,&(++ind)))
                                {
                                        s = M->Get(r,c);
                                        t = s->AddTree();
                                        ConvertToTree(&Elements[ind],r,0,t);
                                }
                                ind=-1;
                                // output Gj2
                                while (GetElement(erGj2,r,0,&(++ind)))
                                {
                                        s = M->Get(r,c);
                                        t = s->AddTree();
                                        ConvertToTree(&Elements[ind],r,0,t);
                                }

                            } // c==r

		   // CROSS ELEMENTS ABOVE DIAGONAL
		   ind=-1;
                   while (GetElement(erIg,r,c,&(++ind)))
		   {				
			   	s = M->Get(r,c);
				t = s->AddTree();
				ConvertToTree(&Elements[ind],r,c,t);
		   }
                   ind=-1;
                   while (GetElement(erCf,r,c,&(++ind)))
                   {
                                s = M->Get(r,c);
                                t = s->AddTree();
                                ConvertToTree(&Elements[ind],r,c,t);
                   }

                   ind=-1;
		   // CROSS ELEMENTS BELOW DIAGONAL
                   while (GetElement(erIg,c,r,&(++ind)))
		   {
			   	s = M->Get(r,c);
				t = s->AddTree();
				ConvertToTree(&Elements[ind],r,c,t);
		   }
                   ind=-1;
                   while (GetElement(erCf,c,r,&(++ind)))
                   {
                                s = M->Get(r,c);
                                t = s->AddTree();
                                ConvertToTree(&Elements[ind],r,c,t);
                   }
	   } // for c
   } //for r
  
return 0;
}
//-------------------------------------------------------------
void CCircuit::NodeEquations()
{

}
//-------------------------------------------------------------
// solves the circuit and prints SN
void CCircuit::Solve(char *filename)
{

FILE *fil = NULL;

time_t past = time(NULL);

// try to open the file 
gc_fopen(&fil,filename,"w");
if (fil == NULL) throw new gcError("Unable to open %s",filename); // exception

//print the blocks
ListBlocks(fil);

// storage for output nodes 
short *o_nodes = new short[lVertices];
short n_cnt; 
GetOutputNodes(&n_cnt,o_nodes);

	try
	{
		SM *M = new SM(lVertices);

                fprintf(stdout,"Analysing admittance matrix:\t\t");
		NodePotentials(M);

		fprintf(stdout,"DONE\n");
		fflush(stdout);
                fprintf(stdout,"Calculating determinant:\t\t");
		M->Det(fil);
		fprintf(stdout,"DONE\n");
		fflush(stdout);

                for (int i=0; i < lElements; i++)
                {
                    if ((Elements[i].role != erGi2) && (Elements[i].role != erGj2)) continue;
                    for (int j = 0; j < lElements; j++)
                    {
                        if ((Elements[j].role != erGi1) && (Elements[j].role != erGj1)) continue;

                        fprintf(stdout,"Analysing signal path from %s to %s:\t",Elements[j].sig,Elements[i].sig);
                        NodePotentials(M,Elements[i].vin,&Elements[j]);
                        fprintf(stdout,"DONE\n");
                        fflush(stdout);
                        fprintf(stdout,"Calculating determinant:\t\t");
                        M->Det(fil,Elements[j].sig,Elements[i].sig);
                        fprintf(stdout,"DONE\n");
                        fflush(stdout);
                    }
                }

		fprintf(stdout,"Finished in %d second(s).\n",int(time(NULL) - past));
		fprintf(stdout,"\nResults were saved as: %s \n",filename);			
		fflush(stdout);
		delete M;
		
	}
	catch(SM_Exception &e)
	{
		e.Show();
	}

fclose(fil);
}
//-------------------------------------------------------------

int CCircuit::ExportToMatlab(char *filename)
{

FILE *fil;
if (filename == NULL) fil = stdout; else gc_fopen(&fil,filename,"w");

SM *M = new SM(lVertices);

// print block definitions
ListBlocks(fil);

// print symbol definitions 
fprintf(fil,"syms s ");
// print symbolic variables 
for (int i=0; i<lElements; i++) fprintf(fil,"%s ",Elements[i].alias);
fprintf(fil,";\n");

 fprintf(fil,"%% -------------------------------------------------\n");
 fprintf(fil,"%% Admittance matrix\n");
 fprintf(fil,"%% -------------------------------------------------\n");
 fprintf(fil,"Y =");
 NodePotentials(M);
 M->Display(fil);
 fprintf(fil,"%% -------------------------------------------------\n");

for (int i=0; i < lElements; i++)
{
    if ((Elements[i].role != erGi2) && (Elements[i].role != erGj2)) continue;    
    for (int j = 0; j < lElements; j++)
    {
        if ((Elements[j].role != erGi1) && (Elements[j].role != erGj1)) continue;

            fprintf(fil,"%%INPUT %s versus OUTPUT %s\n",Elements[j].sig,Elements[i].sig);
            fprintf(fil,"%% -------------------------------------------------\n");
            fprintf(fil,"%s_%s =",Elements[j].sig,Elements[i].sig);
            NodePotentials(M,Elements[i].vin,&Elements[j]);
            M->Display(fil);
            fprintf(fil,"%% -------------------------------------------------\n");        
    }
}

fclose(fil);

delete M;
return 0;
}
//-------------------------------------------------------------
// loads parameter values from given file 
void CCircuit::LoadParams(const char *filename)
{
	printf("This functionality is disabled in circuit.cpp\n");
	
/*
	FILE *fil = NULL;

        fil = fopen(filename,"r");
        if (fil == NULL) {throw new gcError("Unable to open %s ",filename); return;}
	fclose(fil);
	
	CParamFile *pf = new CParamFile();
	
	int cnt; 

	pf->LoadFromFile(filename);        
        pf->Get("lparam",&cnt,1);
	
	printf("%d parameters loaded from %s \n",cnt,filename);
	printf("The circuit has %d parameters\n",lElements);
	
        if (cnt != lElements) { throw new gcError("Invalid number of circuit parameters.",NULL); return;}
	
        Params = (double *) realloc(Params,lElements*sizeof(double));

        pf->Get("params",&cnt,&Params,1);
	
	for (int i = 0; i < cnt; i++)
	{
            Elements[i].value = Params[i];
	}
        delete pf;
		*/
}
//-------------------------------------------------------------
// prints the circuit as VHDL-AMS file 
void CCircuit::ExportToVHDLAMS(const char *filename)
{

//----
FILE *fil = NULL;

fil = fopen(filename,"w");

if (fil == NULL) throw new gcError("Unable to open: ",filename);

int *_in;
_in = (int *) malloc(sizeof(int)*lElements);
int _in_cnt = 0;

int *_out;
_out = (int *) malloc(sizeof(int)*lElements);
int _out_cnt = 0;


// recognize indices of input and output elements
for (int i=0; i < lElements; i++)
{
    if ((Elements[i].role == erGi1) || (Elements[i].role == erGj1))    
        _in[_in_cnt++] = i;
    if ((Elements[i].role == erGi2) || (Elements[i].role == erGj2))
      _out[_out_cnt++] = i;
}

fprintf(fil,"-- generated by gc_analyser v%d --\n\n",VERSION_NR);

// header
//---------
fprintf(fil,"library VLSI;\n");
fprintf(fil,"use VLSI.SI.all;\n");

// entity
//---------
fprintf(fil,"------------- ENTITY -------------\n");
fprintf(fil,"entity filter is  \n");
fprintf(fil,"port(\n");


// print inputs
for (int i=0; i < _in_cnt; i++)
    fprintf(fil,"\t terminal input %s : electrical;\n",Elements[_in[i]].sig);

// print outputs
for (int i=0; i < _out_cnt; i++)
   fprintf(fil,"\t terminal output %s : electrical;\n",Elements[_out[i]].sig);

// print ground
fprintf(fil,"\t terminal ground gnd : electrical\n");
fprintf(fil,"\t);\nend entity filter;\n");

fprintf(fil,"------------- ARCHITECTURE ------------- \n");

// architecture part 
//------------------
fprintf(fil,"architecture GC of filter is\n\n");

// print vertices 
//---------------
fprintf(fil,"-- vertices\n");
fprintf(fil,"variable ");
for (unsigned char i = 0; i < lVertices; i++)
{
	fprintf(fil,"v%d",i+1);
	if (i < lVertices -1) fprintf(fil,",");
}
fprintf(fil,": real;\n\n");

fprintf(fil,"-- input vertices\n");
fprintf(fil,"variable ");

for (int i = 0; i < _in_cnt; i++)
{    
        fprintf(fil,"vi_%d",_in[i]);
        if (i < _in_cnt-1) fprintf(fil,",");
}

fprintf(fil,": real;\n\n");

fprintf(fil,"-- values\n");

// loop through all elements
for (int i=0;i<lElements;i++)
{
	fprintf(fil,"constant ");
	switch (Elements[i].type)
	{
                case etCapacitor: fprintf(fil,"cap_%d:\t",Elements[i].index); break;
                case etGyrator: fprintf(fil,"ig%d:\t",Elements[i].index); break;
                case etConductance: fprintf(fil,"g%d:\t",Elements[i].index); break;
                default: break;
	} // end of switch 
	fprintf(fil," real := %f;",Elements[i].value);
	fprintf(fil,"\n");
} // end of loop

fprintf(fil,"\n\n");
fprintf(fil,"-- components\n");
fprintf(fil,"component IG is\n");
fprintf(fil,"generic( transcon : real );\n");
fprintf(fil,"port( terminal input x : electrical;\n");
fprintf(fil,"terminal output y : electrical;\n");
fprintf(fil,"terminal ground gnd : electrical );\n");
fprintf(fil,"end component IG;\n");

fprintf(fil,"\n\n");
fprintf(fil,"component C is\n");
fprintf(fil,"generic( capacity : real );\n");
fprintf(fil,"port( terminal x : electrical;\n");
fprintf(fil,"terminal y : electrical );\n");
fprintf(fil,"end component C;\n");  

fprintf(fil,"\n\n");
fprintf(fil,"component G is\n");
fprintf(fil,"generic( conduct : real );\n");
fprintf(fil,"port( terminal x : electrical;\n");
fprintf(fil,"terminal y : electrical );\n");
fprintf(fil,"end component G;\n");

fprintf(fil,"-----------------------------------------\n");

// begin architecture
//--------------------

fprintf(fil,"begin\n");

fprintf(fil,"-- input map\n");

// print inputs and outputs
for (int i=0; i < _in_cnt; i++) fprintf(fil,"\t %s => vi_%d;\n",Elements[_in[i]].sig,_in[i]);

fprintf(fil,"-- output map\n");
for (int i=0; i < _out_cnt; i++) fprintf(fil,"\t %s <= v%d;\n",Elements[_out[i]].sig,Elements[_out[i]].vin);

fprintf(fil,"\n");

fprintf(fil,"\n-- elements\n");

// loop through all elements
	for (int i=0;i<lElements;i++)
	{		
		switch (Elements[i].type)
		{
                        case etConductance:
			{
				fprintf(fil,"G%d :\t",Elements[i].index);
				fprintf(fil,"G \tgeneric map( conduct=>g%d )\n",Elements[i].index);


                                switch(Elements[i].role)
				{
                                        case erGi1 : fprintf(fil,"\t\tport map( x=>vi_%d, y=>v%d );",i,Elements[i].vout); break;
                                        case erGi2 : fprintf(fil,"\t\tport map( x=>v%d, y=>gnd );",Elements[i].vin); break;
                                        case erGj1 : fprintf(fil,"\t\tport map( x=>vi_%d, y=>v%d );",i,Elements[i].vout); break;
                                        case erGj2 : fprintf(fil,"\t\tport map( x=>v%d, y=>gnd );",Elements[i].vin); break;
					default: fprintf(fil,"CONNECTION NOT SUPPORTED BY GC ANALYSER"); break;
				} // end of switch

				break;
			}
                        case etGyrator:
			{
				fprintf(fil,"IG%d :\t",Elements[i].index);
				fprintf(fil,"IG \tgeneric map( transcon=>ig%d )\n",Elements[i].index);
				fprintf(fil,"\t\tport map( x=>v%d, y=>v%d, gnd=>gnd );",Elements[i].vin,Elements[i].vout);
				break;
			}

                        case etCapacitor:
			{
				fprintf(fil,"C%d :\t",Elements[i].index);
				fprintf(fil,"C \tgeneric map( capacity=>cap_%d )\n",Elements[i].index);
                                fprintf(fil,"\t\tport map( x=>v%d,",Elements[i].vin);
                                if (Elements[i].vout)
                                    fprintf(fil," y=>v%d );",Elements[i].vout);
                                else
                                    fprintf(fil," y=>gnd );");
				break;
			}
                    default:
                    break;
		}
		fprintf(fil,"\n");
	}

fprintf(fil,"end architecture GC;\n");

fprintf(fil,"------------- THE END ------------- ");

fclose(fil);

if (_in) free(_in);
if (_out) free(_out);

}

//-------------------------------------------------------------
// exports to HSpice model
char * SpiceNode(int i)
{
    char *name;
    name = (char *) calloc(4,sizeof(char));
    if ( i == 0)
      sprintf(name,"gnd");
    else
      sprintf(name,"v%d",i);

    return name;
}
//-------------------------------------------------------------
void CCircuit::ExportToHSpice(const char *filename)
{

    int *_in = NULL;
    int *_out = NULL;
    _in = (int *) malloc(sizeof(int)*lElements);
    _out = (int *) malloc(sizeof(int)*lElements);

    int _in_cnt = 0;
    int _out_cnt = 0;

    for (int i=0; i < lElements; i++)
    {
        if ((Elements[i].role == erGi1) || (Elements[i].role == erGj1)) {_in[_in_cnt++] = i; Elements[i].vin = _in_cnt;}
        if ((Elements[i].role == erGi2) || (Elements[i].role == erGj2)) _out[_out_cnt++] = i;
    }

    
    FILE *fil = fopen(filename,"w");

    fprintf(fil,"gc_model\n");
    fprintf(fil,"**************************\n");
    fprintf(fil,"* created by gc_analyser\n");
    fprintf(fil,"* version %d\n",VERSION_NR);

    fprintf(fil,".subckt filter ");

    for (int i=0; i<_in_cnt; i++) fprintf(fil,"x%d ",i+1);
    fprintf(fil,"gnd ");
    for (int i=0; i<_out_cnt; i++) fprintf(fil,"v%d ",Elements[_out[i]].vin);
    fprintf(fil,"\n");

    fprintf(fil,"**************************\n");
    fprintf(fil,"*Components\n");
    fprintf(fil,"**************************\n");

    // loop through all elements
    for (int i=0;i<lElements;i++)
    {                   
            switch (Elements[i].type)
            {
                    case etCapacitor: fprintf(fil,"C%d:\t %s\t %s\t %f",Elements[i].index,SpiceNode(Elements[i].vin),SpiceNode(Elements[i].vout),Elements[i].value); break;
                    case etGyrator: fprintf(fil,"xg%d:\t %s\t %s\t gyrator g=%f",Elements[i].index,SpiceNode(Elements[i].vin),SpiceNode(Elements[i].vout),Elements[i].value); break;
                    case etConductance:
                        {
                            if (Elements[i].role == erGi1)
                            {
                                fprintf(fil,"R%d:\t x%d\t %s\t %f\n",Elements[i].index,Elements[i].vin,SpiceNode(Elements[i].vout),1.0/Elements[i].value);
                                continue;
                            }
                            if (Elements[i].role == erGj1)
                            {
                                fprintf(fil,"R%d:\t x%d\t %s\t %f\n",Elements[i].index,Elements[i].vout,SpiceNode(Elements[i].vin),1.0/Elements[i].value);
                                continue;
                            }

                            fprintf(fil,"R%d:\t %s\t %s\t %f\n",Elements[i].index,SpiceNode(Elements[i].vin),SpiceNode(Elements[i].vout),1.0/Elements[i].value);
                            break;
                        }

            default: break;
            } // end of switch            
        fprintf(fil,"\n");
    } // end of loop

    fprintf(fil,".ends filter\n");

    fprintf(fil,"**************************\n");
    fprintf(fil,"*Subcircuits\n");
    fprintf(fil,"**************************\n");
    fprintf(fil,".subckt gyrator in out g=1\n");    
    fprintf(fil,"G1 gnd out poly(1) in gnd 0 g\n");
    fprintf(fil,"G2 in gnd poly(1) out gnd 0 g\n");
    fprintf(fil,".ends gyrator\n");

    if (_in) free(_in);
}
//-------------------------------------------------------------

//The End
//-------------------------------------------------------------
