#include "utils.h"
#include "const.h"
//-------------------------------------------------------------
gcError::gcError(const char *new_msg,const char *p1)
{
    msg = (char *) calloc(strlen(new_msg)+strlen(p1)+8,sizeof(char));
    strncpy(msg,"ERROR: ",8);
    sprintf(msg+7,new_msg,p1);
    return;
}
//-------------------------------------------------------------
// wrapper code for safe fopen utility 
int gc_fopen(FILE **fil, const char *filename,const char *mode)
{
	#ifdef UNIX 
		*fil = fopen(filename,mode);			
	#else 
		fopen_s(*fil,filename,mode);	
	#endif
	return 0;
}
//-------------------------------------------------------------
int gc_unlink(char *filename)
{
	#ifdef UNIX 
		char cmd[200];
		cmd[0] = 0x00;
		gc_strcat(cmd,200,(char *)"rm ");
		gc_strcat(cmd,200,filename);		
		system(cmd);
	#else
		_unlink(filename);
	#endif
	return 0;
}
//-------------------------------------------------------------
// tries to open the file 
int gc_FileExists(const char *filename)
{
   FILE *fil;
   gc_fopen(&fil,filename,(char *)"r");
   if (fil == NULL) return 0;
   fclose(fil);
   return 1;
}
//-------------------------------------------------------------
// check if file end of line occured
int gc_feoln(FILE *f)
{
unsigned char c;

	#ifdef UNIX		
		fscanf(f, "%c", &c);
	#else
		fscanf_s(f, "%c", &c);
	#endif 
   
   if (feof(f) || (c == '\n') || (c == '\r'))
          return(1);
   ungetc(c, f);
   return(0);
}
//-------------------------------------------------------------
// wrapper code for srtcat utility 
int gc_strcat(char *dest, int len, char *src)
{
	#ifdef UNIX	
		strncat(dest,src,len);		
	#else
		strcat_s(dest,len,src);
	#endif	
	return 0;
}
//-------------------------------------------------------------
// wrapper code for srtcat utility 
int gc_strncat(char *dest, int dest_len, char *src, int src_len)
{
	#ifdef UNIX	
		strncat(dest,src,dest_len);		
	#else
		strncat_s(dest,dest_len,src,src_len);
	#endif	
	return 0;
}
//-------------------------------------------------------------
int gc_strncpy(char *dest,int dest_len,char *src,int src_len)
{
	#ifdef UNIX
		strncpy(dest,src,dest_len);
	#else
		strncpy_s(dest,dest_len,src,src_len);
	#endif
	return 0;
}
//-------------------------------------------------------------
int gc_strcpy(char *dest,int len,char *src)
{
	#ifdef UNIX
		strncpy(dest,src,len);
	#else
		strcpy_s(dest,len,src);
	#endif
	return 0;
}
//-------------------------------------------------------------
// wrapper code for itoa utility 
int gc_itoa(int val,char *buf,int radix)
{
	#ifdef UNIX
		sprintf(buf,"%d",val);
	#else
		itoa(val,buf,10);
	#endif	
	return 0;
}
//-------------------------------------------------------------
// Universal so far exception class
ErrorClass::ErrorClass(int type,void *wparam)
{
	this->type=type;
	this->wparam=wparam;
}
//-------------------------------------------------------------
// Prints a message denoted by the errors type 
void ErrorClass::PrintMessage()
{
	switch(type)
	{
	case 1: {printf("ERROR: Unable to open file %s\n",(char *)this->wparam); break;}
	default: {printf("Undefined error\n"); break;}
	} // switch
}
//-------------------------------------------------------------
void CenterStr(char *str, int width)
{
	int size = width - strlen(str);
	int stl = strlen(str);

	if (size < 0) {return;}

	for (int i=stl;i<=width; i++)
	{
		if (str[i]==0x00) str[i]=' ';
		if (i>stl) str[i]=' ';		
	}	
	str[width] = 0x00;
}
//-------------------------------------------------------------



//-------------------------------------------------------------
