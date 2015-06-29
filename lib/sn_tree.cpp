#include "sn_tree.h"
//-----------------------------------------------------------------
// overloaded operator A<B
bool operator < (sn_tree A, sn_tree B)
{
	if (A.GetMult()<B.GetMult()) return true;
	if (A.GetCnt() != B.GetCnt()) return false;	
	for (unsigned char i=0; i<A.GetCnt(); i++)
	{
		if (A.GetItem(i) > B.GetItem(i)) return false;
	}
	return true; 
}
//-----------------------------------------------------------------
// overloaded operator A>B
bool operator > (sn_tree A, sn_tree B)
{
	if (A.GetMult()<B.GetMult()) return true;
	if (A.GetCnt() != B.GetCnt()) return false;	
	for (unsigned short i=0; i<A.GetCnt(); i++)
	{
		if (A.GetItem(i) < B.GetItem(i)) return false;
	}
	return true; 
}
//-----------------------------------------------------------------
// overloaded operator 'equal'
bool operator ==(sn_tree A, sn_tree B)
{
	if (A.GetMult()!=B.GetMult()) return false;
	if (A.GetCnt() != B.GetCnt()) return false;
	if (A.GetRank() != B.GetRank()) return false;
	
	for (unsigned int i=0; i<A.GetCnt(); i++)
	{
		if (A.GetItem(i)!=B.GetItem(i)) return false; 
	}
	return true;
}
//-----------------------------------------------------------------
//performs initialization
void sn_tree::Init()
{
	data = NULL;
	cnt = 0; 
	rank = 0; 
        dim = 0;
        drp = 0;
	mult = 0;
	sum = 0;        
}
//-----------------------------------------------------------------
//destroys allocated resources
void sn_tree::Destroy()
{
	if (data != NULL) free(data);
	cnt = 0; 
	rank = 0; 
	mult = 0;
	sum  = 0;
}
//-----------------------------------------------------------------
//prints a single tree
void sn_tree::Print(FILE *dst)
{
	fprintf(dst,"%3d",mult);
	for (int i=0; i<cnt; i++)
	{
                fprintf(dst,"%4d",data[i]);
	}
}
//-----------------------------------------------------------------
// prints the tree in binary format 
void sn_tree::BinaryPrint(FILE *dst)
{    
	fwrite(&rank,sizeof(rank),1,dst);
        fwrite(&dim,sizeof(dim),1,dst); //FIXME: Remove comment after testing
        fwrite(&drp,sizeof(drp),1,dst); //FIXME: Remove comment after testing
	fwrite(&mult,sizeof(mult),1,dst);
	fwrite(&sum,sizeof(sum),1,dst);
	fwrite(&cnt,sizeof(cnt),1,dst);
	fwrite(data,sizeof(data[0]),cnt,dst);
}
//-----------------------------------------------------------------
// prints the tree in polynominal style
void sn_tree::PlainPrint(char operator_chr, char variable_chr)
{	
	char buf[2048];
	buf[0] = 0x00;
	ToString(buf,2048,operator_chr,variable_chr);
	printf("%s",buf);
}
//-----------------------------------------------------------------
// converts the tree into string
unsigned int sn_tree::ToString(char *str,unsigned int str_size,char operator_chr, char variable_chr)
{	
	// print multiplier
	if (mult == 0) return strlen(str);

	#ifdef UNIX
		if (mult == -1)	sprintf(str+strlen(str),"-");		
		if (mult < -1)	sprintf(str+strlen(str),"%d*",mult);
		if (mult > 1)	sprintf(str+strlen(str),"%d*",mult);	
	#else
		if (mult == -1)	sprintf_s(str+strlen(str),str_size,"-");		
		if (mult < -1)	sprintf_s(str+strlen(str),str_size,"%d*",mult);
		if (mult > 1)	sprintf_s(str+strlen(str),str_size,"%d*",mult);
	#endif
	
	// print products
	for (int i = 0 ; i<cnt; i++)
	{
		#ifdef UNIX
			if (i>0) sprintf(str+strlen(str),"*");
			sprintf(str+strlen(str),"%c%d",variable_chr,data[i]);				
		#else
			if (i>0) sprintf_s(str+strlen(str),str_size,"*");
			sprintf_s(str+strlen(str),str_size,"%c%d",variable_chr,data[i]);		
		#endif
	}

    // print rank with operator 
	if (rank)
	{
		#ifdef UNIX
			if (cnt) sprintf(str+strlen(str),"*");
			if (rank == 1)sprintf(str+strlen(str),"%c",operator_chr);
			if (rank > 1)sprintf(str+strlen(str),"%c^%d",operator_chr,rank);		
		#else
			if (cnt) sprintf_s(str+strlen(str),str_size,"*");
			if (rank == 1)sprintf_s(str+strlen(str),str_size,"%c",operator_chr);
			if (rank > 1)sprintf_s(str+strlen(str),str_size,"%c^%d",operator_chr,rank);				
		#endif
	}
	return strlen(str);
}
//-----------------------------------------------------------------
// raises given exception class
void sn_tree::RaiseException(unsigned char ex_code)
{
	E.code = ex_code;
	throw E;
}
//-----------------------------------------------------------------
// gets a given item 
unsigned char sn_tree::GetItem(unsigned char ind)
{
	if (ind >= cnt) RaiseException(stINVALID_INDEX);
	return data[ind];
}
//-----------------------------------------------------------------
//returns current rank
unsigned int sn_tree::GetRank(){ return rank;}
//-----------------------------------------------------------------
// return current dimension-rank pattern
unsigned int sn_tree::GetDimRankPattern(){ return drp;}
//-----------------------------------------------------------------
//returns current dimension
unsigned int sn_tree::GetDim(){ return dim;}
//-----------------------------------------------------------------
//returns item count 
unsigned int sn_tree::GetCnt(){return cnt;}
//-----------------------------------------------------------------
//returns tree data
unsigned char *sn_tree::GetData(){return data;}
//-----------------------------------------------------------------
//returns current multiplicant
inline char sn_tree::GetMult(){ return mult;}
//-----------------------------------------------------------------
//returns current checksum
inline unsigned short sn_tree::GetSum(){ return sum;}
//-----------------------------------------------------------------
// adds a new item 
void sn_tree::AddItem(char item)
{
	// prepare storage 
	cnt++;

	if (cnt >= MAX_CNT) RaiseException(stTREE_TOO_BIG);

	data = (unsigned char *)realloc(data,cnt*sizeof(unsigned char)); 
	data[cnt-1] = 0;

	unsigned char i=cnt-1;
	for (i = cnt-1; i >= 1; i--)
	{
		data[i] = data[i-1];			
		if (data[i-1] < abs(item)) break;	
	}
	data[i] = abs(item);	
	sum += abs(item);

	if (mult == 0) mult = 1; // put the default multiplier
	if (item < 0) mult *= -1;// change sign
	if (item == 0)
	{
		mult = 0;
	}	
}
//-----------------------------------------------------------------
void sn_tree::CopyFrom(sn_tree *src)
{
	cnt = src->cnt;
	mult = src->mult;
	rank  = src->rank;
        dim  = src->dim;
	sum = src->sum;
        drp = src->drp;
	data = (unsigned char *) realloc(data,sizeof(unsigned char)*cnt);
	if (data == NULL) RaiseException(stCOPY_MEM_FAULT);
	memcpy(data,src->data,sizeof(unsigned char)*cnt);	
}
//-----------------------------------------------------------------
// multiplies two trees
void sn_tree::Multiply(sn_tree *src)
{
	mult *= src->mult;
        drp  += src->drp; // FIXME: make sure terms will fit 4bit representation i.e.
        if (dim < src->dim) dim = src->dim; // adjust dimensions

        SetRankFromDRP();

	for (int i = 0; i < src->cnt; i++)
	{
		AddItem(src->data[i]);
	}
}
//-----------------------------------------------------------------
// multiplication by a constant 
void sn_tree::Multiply(char a)
{
	mult *= a;
}
//-----------------------------------------------------------------
// tries to reduce current entry with given src
// returns 1 for successfull attempt
unsigned short sn_tree::ReduceWith(sn_tree *src)
{
// check for conformity:
	if (sum != src->sum) return 0; // different checksums
	if (src->mult == 0) return 0; // entry already reduced
	if (rank != src->rank) return 0; // different rank 
        if (dim != src->dim) return 0; // different dimension
	if (cnt != src->cnt) return 0; // different element count
        if (drp != src->drp) return 0; // different dimension-rank-pattern

	for (unsigned int i = 0; i < cnt; i++)
	{
		if (data[i] != src->data[i]) return 0; // different entry set
	}

// perform the reduction:
	mult += src->mult;
	src->mult = 0;
	src->cnt = 0;

return 1;
}
//-----------------------------------------------------------------
void sn_tree::SetRank(unsigned int new_rank, unsigned char new_dimension)
{

    if (new_dimension > MAX_DIM) throw new gcError("Max dimension exceeded","");
    if (new_dimension <= 0) throw new gcError("Dimension below 1","");
    if (new_dimension > dim) dim = new_dimension;
    if (new_rank > rank) rank = new_rank;

    drp &= ~(0x0000000f << 4*(dim-1));
    drp |= (new_rank & 0x0000000f)<< 4*(dim-1);

}
//-----------------------------------------------------------------
void sn_tree::SetDim(unsigned char new_dimension)
{
    if (new_dimension > MAX_DIM) throw new gcError("Max dimension exceeded","");
    if (new_dimension <= 0) throw new gcError("Dimension below 1","");
    if (new_dimension > dim) dim = new_dimension;
}
//-----------------------------------------------------------------
int sn_tree::SetRankFromDRP()
{

    unsigned int result =0;
    unsigned int buf =0;
    int i = 0;
    for (i=0; i<dim; i++)
    {
        buf = (drp & (0x0000000f << DIM_BITS*i)) >> DIM_BITS*i;
        if (buf > result) result = buf;
    }    
    rank = result;
    return result;
}
//-----------------------------------------------------------------
void sn_tree::SetDRP(unsigned int new_drp)
{
    //TODO: implement data checking
    drp = new_drp;
}
//-----------------------------------------------------------------
void sn_tree::SetMult(char new_mult){mult = new_mult;}
//-----------------------------------------------------------------
// cleans the entry 
void sn_tree::Clean()
{
	cnt = 0;
	mult = 0;
	rank = 0;
	sum = 0;
        dim = 1;
        drp = 0;
	free(data);
	data = NULL;
}
//-----------------------------------------------------------------
// checks whether c is a digit 
unsigned short sn_tree::isdigit(char c)
{
	if ((c >= 48) && (c <= 57)) return 1;
	return 0;
}
//-----------------------------------------------------------------
// Parses series of digits as a single entry 
void sn_tree::ParseFromString(char *str)
{
 unsigned int len = strlen(str);	
 
 char buf[200];
 int j=0;

 short has_digit = 0; 
 short first_numeric = 1;

 short symbol;

 // make sure there will be a separator after last character in buf 
 
 str[len-1]=' ';
 str[len]=0x00;

 for (unsigned int i=0; i<len; i++)
 {
   
   if (str[i]==' ') // space separator was found 
   {
	   if (!has_digit) continue;
	   if (j<1) continue;
	   buf[j++] = 0x00; // add termination char
	   symbol = atoi(buf); // convert to integer
	   
	   if (first_numeric) // treat the number as multiplicant
	   {
		 mult = symbol;
		 first_numeric = 0;
	   }
	   else // treat the number as symbolic 
	   {
	     AddItem(symbol);		 
	   }

	   j=0;
	   has_digit=0;
   }

   if (isdigit(str[i])) has_digit = 1;
   if (str[i]=='-') has_digit = 1;

   if (has_digit) buf[j++] = str[i];
 }

}
//-----------------------------------------------------------------
// loads the tree data from binary stream
void sn_tree::LoadFromStream(FILE *fil)
{


	fread(&rank,sizeof(rank),1,fil);
        fread(&dim,sizeof(dim),1,fil);
        fread(&drp,sizeof(drp),1,fil);
	fread(&mult,sizeof(mult),1,fil);
	fread(&sum,sizeof(sum),1,fil);

	fread(&cnt,sizeof(cnt),1,fil);

	// prepare storage 
	data = (unsigned char *)realloc(data,cnt*sizeof(unsigned char)); 
	
	// read remaining data
	fread(data,sizeof(data[0]),cnt,fil);
}
// The End
//-----------------------------------------------------------------

