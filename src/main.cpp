// Includes
//-------------------------------------------------------------
#include "main.h"

#include "sn.h"
#include "sn_tree.h"

// Settings 
//-------------------------------------------------------------
//#define diag 1 // diagnostic mode

#define MAX_FILE_PATH 1024
//-------------------------------------------------------------

//-------------------------------------------------------------
// Global buffers
//-------------------------------------------------------------
char inputfile[MAX_FILE_PATH];
char outputfile[MAX_FILE_PATH];
char paramfile[MAX_FILE_PATH];
int option = 0;
//-------------------------------------------------------------
int PrintMenu()
{
	fprintf(stdout,"Enter block file name:");	
	fflush(stdout);	
	
	#ifdef UNIX
		scanf("%s",inputfile);
	#else
		scanf_s("%s",inputfile);
	#endif

	if (!gc_FileExists(inputfile))
	{
		fprintf(stdout,"Unable to access file %s",inputfile);
		return -1;
	}

	fprintf(stdout,"\nOptions:\n");
	fprintf(stdout,"\t1: compute SN solution\n");
	fprintf(stdout,"\t2: export to Matlab script\n");	
	/*fprintf(stdout,"\t3: export to VHDL-AMS file\n");*/

	fprintf(stdout,"\nYour choice:"); 
	fflush(stdout);
	#ifdef UNIX
		scanf("%d",&option);	
	#else 
		scanf_s("%d",&option);
	#endif
	
	return 1;
}

//-------------------------------------------------------------
int main(int argc,char *argv[])
{

    // diagnostic mode goes here
#ifdef diag

        printf("Diagnostic mode\n");

        try
            {

        CCircuit *Cir = new CCircuit();
        Cir->LoadAMSModel("G:\\programy\\multi_dim\\gc_analyser\\bin\\example_2d.vhdla");
        Cir->ListElements();
        Cir->ExportToMatlab("G:\\programy\\multi_dim\\gc_analyser\\bin\\outcome2d.m");
        Cir->Solve("G:\\programy\\multi_dim\\gc_analyser\\bin\\outcome2d.SN");
        Cir->ListBlocks(stdout);
        delete Cir;
    }
        catch (gcError *e)
          {
            fprintf(stdout,"%s\n",e->msg);
           }
          catch (...)
          {
            fprintf(stdout,"An unhandled exception occured\n");
          }


	return 0;
#endif
	fprintf(stdout,"\nGC analyser version: %d\n",VERSION_NR);
	fprintf(stdout,"\n MULTI DIMENSIONAL \n");
	fprintf(stdout,"-----------------------------\n");    	
	fflush(stdout);
	
	inputfile[0] = 0x00;
	outputfile[0] = 0x00;
	paramfile[0] = 0x00;

	switch (argc)
	{
		case 1: PrintMenu(); break;

		case 3: 
		{
			if (strcmp(argv[1],"SN") == 0) 
			{
				strncpy(inputfile,argv[2],strlen(argv[2]));
				option = 1;
			}		

            if (strcmp(argv[1],"MAT") == 0)
            {
				strncpy(inputfile,argv[2],strlen(argv[2]));
                option = 2;
            }

			break;
		}
		/*
		case 4:
		{
			if (strcmp(argv[1],"AMS") == 0) 
			{
				strncpy(inputfile,argv[2],strlen(argv[2]));
				strncpy(paramfile,argv[3],strlen(argv[3]));
				option = 3;
				break;
			}

            if (strcmp(argv[1],"SP") == 0)
            {
                strncpy(inputfile,argv[2],strlen(argv[2]));
                strncpy(paramfile,argv[3],strlen(argv[3]));
                option = 4;
                break;
            }
		
		}
		*/
		default: 
		{
			fprintf(stdout,"Invalid syntax\n");
			fflush(stdout);
			option = 0;
			break;
		}
	} // end of switch 
	
	try
	{
		CCircuit *Cir = new CCircuit();
                if (option)
                {

                    if(Cir->LoadAMSModel(inputfile))
                    {
                        fprintf(stdout,"An error occured during block file analysis. Program will terminate.");
                        fflush(stdout);
                        delete Cir;
                        exit(2);
                    }
                }

	switch(option)
	{
		case 0: // do nothing
		{
			break;
		}
		case 1: // compute SN solution
		{
				Cir->ListElements();
				outputfile[0] = 0x00;
				gc_strcpy(outputfile,MAX_FILE_PATH,inputfile);	
				if (strlen(outputfile) > 4) outputfile[strlen(outputfile)-4] = 0x00;
				gc_strncat(outputfile,MAX_FILE_PATH,(char *)".sn",4);
                time_t seconds;
                seconds = time(NULL);
				Cir->Solve(outputfile);
				Cir->ListBlocks(stdout);
                fprintf(stdout,"Finished in %ld seconds\n",time(NULL)-seconds);
			break;
		}
		case 2: // export to Matlab script 
		{
			outputfile[0] = 0x00;
			gc_strcpy(outputfile,200,inputfile);	
			if (strlen(outputfile) > 4) outputfile[strlen(outputfile)-4] = 0x00;
			gc_strncat(outputfile,200,(char *)".m",3);			
			fprintf(stdout,"\nExporting to Matlab file: %s\n",outputfile);
			Cir->ListElements();			
			Cir->ExportToMatlab(outputfile);
			break;
		}
		/*
		case 3: // export to VHDL-AMS
		{
			if (strlen(paramfile) < 1)
			{
				fprintf(stdout,"Enter parameter file name:");
				fflush(stdout);
				#ifdef UNIX			
					scanf("%s",paramfile);
				#else
					scanf_s("%s",paramfile);			
				#endif
			}

			if (!gc_FileExists(paramfile))
			{
				fprintf(stdout,"Unable to access file %s",paramfile);
				return -1;
			}

			outputfile[0] = 0x00;
			gc_strcpy(outputfile,MAX_FILE_PATH,inputfile);	
			if (strlen(outputfile) > 4) outputfile[strlen(outputfile)-4] = 0x00;
			gc_strncat(outputfile,MAX_FILE_PATH,(char *)".vhdla",7);			
			fprintf(stdout,"\nExporting to VHDL-AMS file: %s\n",outputfile);
			fflush(stdout);
                        Cir->SortElements();
                        Cir->LoadParams(paramfile);                        
                        Cir->ExportToVHDLAMS(outputfile);
                        break;
		}

                case 4:  // export to HSpice model
                {

                        if (!gc_FileExists(paramfile))
                        {
                                fprintf(stdout,"Unable to access file %s",paramfile);
                                return -1;
                        }

                        outputfile[0] = 0x00;

                        gc_strcpy(outputfile,MAX_FILE_PATH,inputfile);
                        if (strlen(outputfile) > 4) outputfile[strlen(outputfile)-4] = 0x00;
                        gc_strncat(outputfile,MAX_FILE_PATH,(char *)".sp",4);

                        fprintf(stdout,"\nExporting to HSpice file: %s\n",outputfile);

                        fflush(stdout);

                        Cir->SortElements();
                        Cir->LoadParams(paramfile);                        
                        Cir->ExportToHSpice(outputfile);
                        break;
                }
		*/
		default:
		{
			fprintf(stdout,"Unknown option\n");
			fflush(stdout);
			break;
		}

	}// end for switch option 
	
        delete Cir;

	}
        catch (gcError *e)
           {
            fprintf(stdout,"%s\n",e->msg);
           }
          catch (...)
          {
            fprintf(stdout,"An unhandled exception occured\n");
          }


	return RESULT_OK;
}
//-------------------------------------------------------------
