#include "ctff.h"

//------------------------------------------------------------------------------
CTff::CTff()
{
 sn_cnt  = 0;
 sn_rank = 0;
 Clear();
}
//------------------------------------------------------------------------------
int CTff::Clear()
{
    sn_cnt = 0;
    sn_rank = 0;
    tff_file[0] = 0x00;
    sn_file[0] = 0x00;    
    return 0;
}
//------------------------------------------------------------------------------
void CTff::SetCoeff( int p,int q,double value)
{
    if (p >= sn_cnt) return;
    if (p < 0) return;
    if (q > sn_rank) return;
    if (q < 0) return;
    params[p][q] = value;
}
//------------------------------------------------------------------------------
double CTff::GetCoeff( int p,int q)
{
    return params[p][q];    
}
//------------------------------------------------------------------------------
void CTff::SetBlank( int p,int q,int value)
{
    if (p >= sn_cnt) return;
    if (p < 0) return;
    if (q > sn_rank) return;
    if (q < 0) return;
    blanks[p][q] = value;
}
//------------------------------------------------------------------------------
int CTff::GetSNCnt() { return sn_cnt;}
int CTff::GetSNOrder() {return sn_rank;}
//------------------------------------------------------------------------------
char *CTff::GetTFFFile(){ return sn_file;}
//------------------------------------------------------------------------------
char *CTff::GetSNFile(){ return tff_file;}
//------------------------------------------------------------------------------
sn_entry *CTff::GetEntry(int ind)
{
    return &items[ind];
}
//------------------------------------------------------------------------------
int CTff::ScanSN(const char *new_sn_file)
{
    SN *sn = new SN();

    strncpy(sn_file,new_sn_file,strlen(new_sn_file)+1);

    sn->LoadFromFile(sn_file,0);

    sn_cnt = 0;
    while(sn->LoadFromFile(sn_file,sn_cnt)) sn_cnt++; // calculate number of sn in a file

    sn_rank = 0;

    if (items) delete items;

    items = new sn_entry[sn_cnt];

    for (short s=0; s < sn_cnt; s++)
    {
        sn->LoadFromFile(sn_file,s);
        if (sn->GetRank() > sn_rank) sn_rank = sn->GetRank();
        strncpy(items[s].name,sn->GetName(),strlen(sn->GetName())+1);
        strncpy(items[s].in,sn->GetIN(),strlen(sn->GetIN())+1);
        strncpy(items[s].out,sn->GetOUT(),strlen(sn->GetOUT())+1);
        items[s].rank = sn->GetRank();
        items[s].type = sn->GetType();
        items[s].width = sn->GetWidth();
    }
    if (sn) delete sn;
    return 0;
}
//------------------------------------------------------------------------------
int CTff::LoadFromFile(char *new_tff_file, char *new_sn_file)
{    
    Clear();

    if (gc_FileExists(new_sn_file) == 0) throw new gcError("Unable to load ",new_sn_file);
    if (gc_FileExists(new_tff_file) == 0) throw new gcError("Unable to load ",new_tff_file);

    strncpy(sn_file,new_sn_file,strlen(new_sn_file)+1);    
    strncpy(tff_file,new_tff_file,strlen(new_tff_file)+1);

    ScanSN(sn_file);

    CParamFile *pf = new CParamFile();
    pf->LoadFromFile(tff_file);

    int size;
    double *buf = NULL;
    buf = new double[200];
    int *intbuf  = NULL;
    intbuf = new int[200];
    char namebuf[1024];
    for (int s = 0; s < sn_cnt; s++)
    {
        for (int r = 0; r <= sn_rank; r++) params[s][r] = 0.0; // clear the values
        for (int r = 0; r <= sn_rank; r++) blanks[s][r] = 0; // clear the values

        if (pf->Get_dv(items[s].name,&size,&buf,0) >= 0)
        {
            for (int r = 0; r <= size; r++) params[s][r] = buf[r]; // fill the values with read ones
        }

        namebuf[0] = 0x00;
        strncpy(namebuf,items[s].name,strlen(items[s].name)+1);
        strcat(namebuf,"BL");
        if (pf->Get_iv(namebuf,&size,&intbuf,0) >= 0)
        {
            for (int r = 0; r <= size; r++) blanks[s][r] = intbuf[r]; // fill the values with read ones
        }

    }

    delete buf;

    // load design parameters
    double dbl_dummy = 0.0;
    int int_dummy = 0;

    Design.type = 0;
    if (pf->Get_i("type",&int_dummy,0) >=0 ) {Design.type = int_dummy;}

    Design.Ap = 0.1;
    if (pf->Get_d("Ap",&dbl_dummy,0) >= 0) {Design.Ap = dbl_dummy;}

    Design.As = 20.0;
    if (pf->Get_d("As",&dbl_dummy,0) >= 0) {Design.As = dbl_dummy;}

    Design.wp = 0.5;
    if (pf->Get_d("wp",&dbl_dummy,0) >= 0) {Design.wp = dbl_dummy;}

    Design.ws = 1.0;
    if (pf->Get_d("ws",&dbl_dummy,0) >= 0) {Design.ws = dbl_dummy;}

    Design.scale = 1.0;
    if (pf->Get_d("scale",&dbl_dummy,0) >= 0) {Design.scale = dbl_dummy;}

    delete pf;

    return 1;
}
//------------------------------------------------------------------------------
// makes a PN input file for ParamCalc
void CTff::MakePNFile(const char *pn_file)
{
    FILE *fil = NULL;

    SN *sn = new SN;
    int s_rank = 0;
    int *data = NULL;

    fil = fopen(pn_file,"w");
    //fprintf(fil,"%%Created by GCStudio %d\n",VERSION_NR);
    fprintf(fil,"%%! BEGIN PN\n");
    fprintf(fil,"%%! COEFFICIENTS\n");
    //fprintf(fil,"%%---------------------------------------------\n");
    for (short s = 0; s < sn_cnt; s++)
    {
        s_rank = items[s].rank;
        printf("s_rank is %d\n",s_rank);

        sn->LoadFromFile(sn_file,s);
        data = new int[s_rank+1];
        sn->GetTreeCount(data);

        for (int r = 0; r <= sn_rank; r++)
        {
            if (blanks[s][r])
            {
                fprintf(fil,"x\n");
                continue;
            }

            if (sn_rank-r <= s_rank)
            {
                if (data[sn_rank-r] > 0) fprintf(fil,"%3.12f\n",params[s][r]);
                //fprintf(fil,"%3.12f %% %s s^%d ",params[s][r],items[s].name,sn_rank-r);
                //fprintf(fil,"data[%d]=%d \n",sn_rank-r,data[sn_rank-r]);
            }
            else
                continue;
        }
      //  fprintf(fil,"%%---------------------------------------------\n");
    }
    fprintf(fil,"%%! END PN");
    if (fil) fclose(fil);
    if (sn) delete sn;
}
//------------------------------------------------------------------------------
// stores the actual transfer function into a file
void CTff::SaveToFile(const char *filename)
{
    CParamFile *pf = new CParamFile();
    pf->Clear();    
    pf->Put_i("sn_cnt",sn_cnt);
    pf->Put_i("sn_rank",sn_rank);

    for (short s = 0; s < sn_cnt; s++)
    {
        pf->Put_dv(items[s].name,sn_rank+1,params[s]);
    }

    char buf[1024];

    for (short s = 0; s < sn_cnt; s++)
    {
        buf[0] = 0x00;
        strncpy(buf,items[s].name,strlen(items[s].name)+1);
        strcat(buf,"BL");
        pf->Put_iv(buf,sn_rank+1,blanks[s]);
    }


    pf->SaveToFile(filename);
    delete pf;
}
//------------------------------------------------------------------------------
void CTff::Display(FILE *fil)
{
    for (short s = 0; s < sn_cnt; s++)
    {
        fprintf(fil,"SN %s\n",items[s].name);
        fprintf(fil,"\tIN %s\n",items[s].in);
        fprintf(fil,"\tOUT %s\n",items[s].out);
        for (short r = 0; r <sn_rank ; r++)
        {
            fprintf(fil,"%2.4fs^%d ",params[s][r],sn_rank-r);
        }
        fprintf(fil,"\n");
    }
}
//------------------------------------------------------------------------------
