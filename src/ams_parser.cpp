#include "ams_parser.hpp"

//------------------------------------------------------------------------------
AMSError::AMSError(unsigned char new_code)
{
    code = new_code;
}
//------------------------------------------------------------------------------
void AMSError::Display()
{
    fprintf(stdout,"An error occured within AMS parser. Code is %d\n",code);
    fflush(stdout);
}
//------------------------------------------------------------------------------
ParseError::ParseError(const char *new_msg,AMSSymbol *smb)
{
    msg = (char *) calloc(strlen(new_msg)+1,sizeof(char));
    strncpy(msg,new_msg,strlen(new_msg)+1);

    if (smb != NULL)
    {
        row = smb->row;
        col = smb->col;
        strncpy(data,smb->data,strlen(smb->data)+1);
    }
}
//------------------------------------------------------------------------------
void ParseError::Display(FILE *output = stdout)
{
    if (row == 0)
        fprintf(output,"ERROR: %s\n",msg);
    else
        fprintf(output,"ERROR: While processing '%s' in line %d, column %d: %s\n",data,row,col,msg);
    fflush(output);
}
//------------------------------------------------------------------------------
ParseError::~ParseError()
{
    if (msg) free(msg);
}
//------------------------------------------------------------------------------
AMSVariable::AMSVariable()
{
    Empty();
}
//------------------------------------------------------------------------------
void AMSVariable::Display(FILE *fil)
{
    printf("%s : ",name);
    switch (type)
    {
        case amsReal: printf("real"); break;
        case amsElectrical: printf("electrical"); break;
        default : printf("unknown"); break;
    } // end of switch
}
//------------------------------------------------------------------------------
void AMSVariable::Empty()
{
    name[0] = 0x00;
    type = amsUnknown;
    index = 0;
    used = 0;
    dir = pdUnknown;
}
//------------------------------------------------------------------------------
// default constructor
AMSSymbol::AMSSymbol(const char *origin, unsigned int new_row, unsigned int new_col)
{    
    row = new_row;
    col = new_col;
    strncpy(data,origin,strlen(origin)+1);
    IsSep = 0;
}
//------------------------------------------------------------------------------
char AMSSymbol::IsBlockGeneric()
{
    if (strcmp(data,"Ci") == 0) return 1;
    if (strcmp(data,"Cj") == 0) return 1;
    if (strcmp(data,"Cf") == 0) return 1;    
    if (strcmp(data,"ig") == 0) return 1;    
    return 0;
}
//------------------------------------------------------------------------------
char AMSSymbol::IsBlockPort()
{
    if (strcmp(data,"Ni") == 0) return 1;
    if (strcmp(data,"Nj") == 0) return 1;
    if (strcmp(data,"i_in") == 0) return 1;
    if (strcmp(data,"j_in") == 0) return 1;
    if (strcmp(data,"i_out") == 0) return 1;
    if (strcmp(data,"j_out") == 0) return 1;
    return 0;
}
//------------------------------------------------------------------------------
char AMSSymbol::IsComponentGeneric()
{
    if (strcmp(data,"dim") == 0) return 1;
    return 0;
}
//------------------------------------------------------------------------------
char AMSSymbol::IsComponentPort()
{
    if (strcmp(data,"Ni") == 0) return 1;
    if (strcmp(data,"Nj") == 0) return 1;
    if (strcmp(data,"Vo") == 0) return 1;
    return 0;
}
//------------------------------------------------------------------------------
// default destructor
AMSSymbol::~AMSSymbol()
{

}
//------------------------------------------------------------------------------
void AMSSymbol::Display(FILE *fil = stdout)
{
    if (IsSep)
        fprintf(fil,"|%s|",data);
    else
        fprintf(fil,"%s",data);
}
//------------------------------------------------------------------------------
inline char AMSSymbol::Is(const char *c)
{
    if (strcmp(data,c) == 0) return 1; else return 0;
}
//------------------------------------------------------------------------------
inline char AMSSymbol::Not(const char *c)
{
    if (strcmp(data,c) != 0) return 1; else return 0;
}
//------------------------------------------------------------------------------
// default constructor
GCBlock::GCBlock()
{
    name[0] = 0x00;
    cnf = 0x00;
    inv = 0;
    Ni.Empty();
    Nj.Empty();
    i_in.Empty();
    i_out.Empty();
    j_in.Empty();
    j_out.Empty();
}
//------------------------------------------------------------------------------
void GCBlock::SetName(const char *new_name)
{
    strncpy(name,new_name,strlen(new_name)+1);    
}
//------------------------------------------------------------------------------
// displays the block content
void GCBlock::Display(FILE *fil=stdout)
{    
    fprintf(fil,"%s ",name);
    for (unsigned char i = 0x00; i < 8; i++)
    {        
        fprintf(fil,"%d ",cnf & (0x80 >> i)?1:0);
    }
      fprintf(fil,"ni: %s nj: %s ",Ni.name,Nj.name);
      if (i_in.used) fprintf(fil,"i_in : %s ",i_in.name);
      if (j_in.used) fprintf(fil,"j_in : %s ",j_in.name);
      if (i_out.used) fprintf(fil,"i_out : %s ",i_out.name);
      if (j_out.used) fprintf(fil,"j_out : %s ",j_out.name);
      fprintf(fil,"\n");
}
//------------------------------------------------------------------------------
// writes the configuration string to a file
void GCBlock::WriteCnfToFile(FILE *fil)
{
    for (unsigned char i = 0x00; i < 8; i++)
    {
        if ((i == 7-bit_ig)) fprintf(fil,inv?"-":" ");
        if ((i == 7-bit_Gi1)) fprintf(fil," ");
        fprintf(fil,"%d ",cnf & (0x80 >> i)?1:0);
    }
}
//------------------------------------------------------------------------------
void GCBlock::SetCnf(const char *term)
{
    if (strcmp(term,"Ci") == 0) cnf |= (0x01 << bit_Ci);
    if (strcmp(term,"Cj") == 0) cnf |= (0x01 << bit_Cj);
    if (strcmp(term,"Cf") == 0) cnf |= (0x01 << bit_Cf);
    if (strcmp(term,"Gi1") == 0) cnf |= (0x01 << bit_Gi1);
    if (strcmp(term,"Gi2") == 0) cnf |= (0x01 << bit_Gi2);
    if (strcmp(term,"Gj1") == 0) cnf |= (0x01 << bit_Gj1);
    if (strcmp(term,"Gj2") == 0) cnf |= (0x01 << bit_Gj2);
    if (strcmp(term,"ig") == 0) cnf |= (0x01 << bit_ig);
    
}
//------------------------------------------------------------------------------
void GCBlock::ResetCnf(const char *term)
{
    // FIXME: currently there is no need to implement this routine
}
//------------------------------------------------------------------------------
// sets a variable for input and output node
void GCBlock::SetNode(const char *term,AMSVariable *n)
{    
    n->used = 1;
    if (strcmp(term,"Ni") == 0)
    {
        if (Ni.used) throw new ParseError("Node Ni already assigned",NULL);
        strncpy(Ni.name,n->name,strlen(n->name)+1);
        Ni.type = n->type;
        Ni.used = 1;
        Ni.index = n->index;
    }

    if (strcmp(term,"Nj") == 0)
    {
        if (Nj.used) throw new ParseError("Node Nj already assigned",NULL);
        strncpy(Nj.name,n->name,strlen(n->name)+1);
        Nj.type = n->type;
        Nj.used = 1;
        Nj.index = n->index;
    }

    if (strcmp(term,"i_in") == 0)
    {
        if (i_in.used) throw new ParseError("Port i_in already assigned",NULL);
        strncpy(i_in.name,n->name,strlen(n->name)+1);
        i_in.type = n->type;
        i_in.used = 1;
        cnf |= (1 << bit_Gi1);
    }

    if (strcmp(term,"i_out") == 0)
    {
        if (i_out.used) throw new ParseError("Port i_out already assigned",NULL);
        strncpy(i_out.name,n->name,strlen(n->name)+1);
        i_out.type = n->type;
        i_out.used = 1;
        cnf |= (1 << bit_Gi2);
    }

    if (strcmp(term,"j_in") == 0)
    {
        if (j_in.used) throw new ParseError("Port j_in already assigned",NULL);
        strncpy(j_in.name,n->name,strlen(n->name)+1);
        j_in.type = n->type;
        j_in.used = 1;
        cnf |= (1 << bit_Gj1);
    }

    if (strcmp(term,"j_out") == 0)
    {
        if (j_out.used) throw new ParseError("Port j_out already assigned",NULL);
        strncpy(j_out.name,n->name,strlen(n->name)+1);
        j_out.type = n->type;
        j_out.used = 1;
        cnf |= (1 << bit_Gj2);
    }

}
//------------------------------------------------------------------------------
// default constructor
AMSParser::AMSParser()
{
    smb_cnt = 0;
    cmp_cnt = 0;
    blc_cnt = 0;
    has_blocks = 0;
    has_components = 0;
    smb = NULL;
}
//------------------------------------------------------------------------------
// default destructor
AMSParser::~AMSParser()
{
 // empty so far
}
//------------------------------------------------------------------------------
// recognizes the variables
int AMSParser::FindVariables(int start_s)
{    
    int s = start_s;
    int v_init = 0;
    int node_ind = 0;

    int result;
    do
    {
        result = s;
        s = FindSymbol("variable",s);

        if (s < 0) return result;

        if (GetSmb(++s)->Not(" ")) throw new ParseError("Space expected",GetSmb(s));

        v_init = vrs_cnt;

        while (strcmp(GetSmb(s++)->data,";")!=0)
        {            
            if (GetSmb(s)->Is(" ")) continue; // skip spaces
            if (GetSmb(s)->Is(",")) continue; // skip commas

            if (GetSmb(s)->Is(":")) // type qualifier is found
            {
                if (GetSmb(++s)->Is(" ")) s++; // skip possible space
                if (GetSmb(s)->Is("real")) // variable type MUST be real
                {
                    for (int j = v_init; j < vrs_cnt; j++) vrs[j].type = amsReal;             
                }
                else
                {
                    throw new ParseError("A real type is expected",GetSmb(s));
                }
                if (GetSmb(s++)->Is(" ")) s++; // skip possible space
                if (GetSmb(s)->Not(";")) throw new ParseError("Syntax error",GetSmb(s)); // statment MUST be ended with ;
                continue;
            }

            // assume the symbol is a variable name
            if (VariableDefined(GetSmb(s)->data)) throw new ParseError("symbol already defined",GetSmb(s));

            // check if the name isn't reserved
            if (GetSmb(s)->IsReserved()) throw new ParseError("variable name is a reserved word",GetSmb(s));

            // create a new variable
            strncpy(vrs[vrs_cnt].name,GetSmb(s)->data,strlen(GetSmb(s)->data)+1);
            vrs[vrs_cnt].index = ++node_ind;
            vrs_cnt++;

        } // parsing for the semicolon
    }
     while (s >= 0);
return 0;
}
//-----------------------------------------------------------------------------
 // determines if a given variable is already defined
char AMSParser::VariableDefined(const char *name)
{
    for (int i = 0; i < vrs_cnt; i++) if(strcmp(vrs[i].name,name) == 0) return i+1;
    return 0;
}
//-----------------------------------------------------------------------------
// determines if the ground is already defined
char AMSParser::GroundDefined()
{
    for (int i = 0; i < vrs_cnt; i++) if(vrs[i].dir == pdGround) return i+1;
    return 0;
}
int AMSParser::HasBlocks() {return has_blocks;}
int AMSParser::HasComponents() { return has_components;}
//------------------------------------------------------------------------------
// checks if the given symbol is a reserved word
char AMSSymbol::IsReserved()
{
    if (strcmp(data,"abs") == 0) return 1;
    if (strcmp(data,"access") == 0) return 1;
    if (strcmp(data,"across") == 0) return 1;
    if (strcmp(data,"after") == 0) return 1;
    if (strcmp(data,"alias") == 0) return 1;
    if (strcmp(data,"all") == 0) return 1;
    if (strcmp(data,"and") == 0) return 1;
    if (strcmp(data,"architecture") == 0) return 1;
    if (strcmp(data,"array") == 0) return 1;
    if (strcmp(data,"assert") == 0) return 1;
    if (strcmp(data,"attribute") == 0) return 1;
    //
    if (strcmp(data,"begin") == 0) return 1;
    if (strcmp(data,"block") == 0) return 1;
    if (strcmp(data,"body") == 0) return 1;
    if (strcmp(data,"break") == 0) return 1;
    if (strcmp(data,"buffer") == 0) return 1;
    if (strcmp(data,"bus") == 0) return 1;
    //
    if (strcmp(data,"case") == 0) return 1;
    if (strcmp(data,"component") == 0) return 1;
    if (strcmp(data,"configuration") == 0) return 1;
    if (strcmp(data,"constant") == 0) return 1;
    //
    if (strcmp(data,"disconnect") == 0) return 1;
    if (strcmp(data,"downto") == 0) return 1;
    //
    if (strcmp(data,"else") == 0) return 1;
    if (strcmp(data,"elsif") == 0) return 1;
    if (strcmp(data,"end") == 0) return 1;
    if (strcmp(data,"entity") == 0) return 1;
    if (strcmp(data,"exit") == 0) return 1;
    //
    if (strcmp(data,"file") == 0) return 1;
    if (strcmp(data,"for") == 0) return 1;
    if (strcmp(data,"function") == 0) return 1;
    //
    if (strcmp(data,"generate") == 0) return 1;
    if (strcmp(data,"generic") == 0) return 1;
    if (strcmp(data,"group") == 0) return 1;
    if (strcmp(data,"guarded") == 0) return 1;
    //
    if (strcmp(data,"if") == 0) return 1;
    if (strcmp(data,"impure") == 0) return 1;
    if (strcmp(data,"in") == 0) return 1;
    if (strcmp(data,"inertial") == 0) return 1;
    if (strcmp(data,"inout") == 0) return 1;
    if (strcmp(data,"is") == 0) return 1;
    //
    if (strcmp(data,"label") == 0) return 1;
    if (strcmp(data,"library") == 0) return 1;
    if (strcmp(data,"limit") == 0) return 1;
    if (strcmp(data,"linkage") == 0) return 1;
    if (strcmp(data,"literal") == 0) return 1;
    if (strcmp(data,"loop") == 0) return 1;
    //
    if (strcmp(data,"map") == 0) return 1;
    if (strcmp(data,"mod") == 0) return 1;
    //
    if (strcmp(data,"nand") == 0) return 1;
    if (strcmp(data,"nature") == 0) return 1;
    if (strcmp(data,"new") == 0) return 1;
    if (strcmp(data,"next") == 0) return 1;
    if (strcmp(data,"noise") == 0) return 1;
    if (strcmp(data,"nor") == 0) return 1;
    if (strcmp(data,"not") == 0) return 1;
    if (strcmp(data,"null") == 0) return 1;
    //
    if (strcmp(data,"of") == 0) return 1;
    if (strcmp(data,"open") == 0) return 1;
    if (strcmp(data,"or") == 0) return 1;
    if (strcmp(data,"others") == 0) return 1;
    if (strcmp(data,"out") == 0) return 1;
    //
    if (strcmp(data,"package") == 0) return 1;
    if (strcmp(data,"port") == 0) return 1;
    if (strcmp(data,"postponed") == 0) return 1;
    if (strcmp(data,"procedural") == 0) return 1;
    if (strcmp(data,"procedure") == 0) return 1;
    if (strcmp(data,"process") == 0) return 1;
    if (strcmp(data,"pure") == 0) return 1;
    //
    if (strcmp(data,"quantity") == 0) return 1;
    //
    if (strcmp(data,"range") == 0) return 1;
    if (strcmp(data,"record") == 0) return 1;
    if (strcmp(data,"reference") == 0) return 1;
    if (strcmp(data,"register") == 0) return 1;
    if (strcmp(data,"reject") == 0) return 1;
    if (strcmp(data,"rem") == 0) return 1;
    if (strcmp(data,"report") == 0) return 1;
    if (strcmp(data,"return") == 0) return 1;
    if (strcmp(data,"rol") == 0) return 1;
    if (strcmp(data,"ror") == 0) return 1;
    //
    if (strcmp(data,"select") == 0) return 1;
    if (strcmp(data,"severity") == 0) return 1;
    if (strcmp(data,"shared") == 0) return 1;
    if (strcmp(data,"signal") == 0) return 1;   
    if (strcmp(data,"sla") == 0) return 1;
    if (strcmp(data,"sll") == 0) return 1;
    if (strcmp(data,"spectrum") == 0) return 1;
    if (strcmp(data,"sra") == 0) return 1;
    if (strcmp(data,"srl") == 0) return 1;
    if (strcmp(data,"subnature") == 0) return 1;
    if (strcmp(data,"subtype") == 0) return 1;
    //
    if (strcmp(data,"terminal") == 0) return 1;
    if (strcmp(data,"then") == 0) return 1;
    if (strcmp(data,"through") == 0) return 1;
    if (strcmp(data,"to") == 0) return 1;
    if (strcmp(data,"tolerance") == 0) return 1;
    if (strcmp(data,"transport") == 0) return 1;
    if (strcmp(data,"type") == 0) return 1;
    //
    if (strcmp(data,"unaffected") == 0) return 1;
    if (strcmp(data,"units") == 0) return 1;
    if (strcmp(data,"until") == 0) return 1;
    if (strcmp(data,"use") == 0) return 1;
    //
    if (strcmp(data,"variable") == 0) return 1;
    //
    if (strcmp(data,"wait") == 0) return 1;
    if (strcmp(data,"when") == 0) return 1;
    if (strcmp(data,"while") == 0) return 1;
    if (strcmp(data,"with") == 0) return 1;
    //
    if (strcmp(data,"xnor") == 0) return 1;
    if (strcmp(data,"xor") == 0) return 1;

    return 0;
}
//------------------------------------------------------------------------------
// makes a list of variables
void AMSParser::ShowVariables()
{
    for (int i = 0; i < vrs_cnt; i++)
    {
        vrs[i].Display();
        printf("\n");
    }
}
//------------------------------------------------------------------------------

int AMSParser::FindComponent(const char *type, int start_s)
{    
    int s = start_s;
    int found = 0;
    do // look for components...
    {
        s = FindSymbol(type,s);
        if (s < 0) return found;
        found = 1;
        cmp_loc[cmp_cnt++].type_loc=s;

        if (strcmp(type,"GC_BLOCK")==0)
        {
            ParseBlock(s);
        }
        else
        {
            ParseComponent(s);
        }

        s++;
    }
    while (s > 0); // ...until there are any.
    return found;
}

//------------------------------------------------------------------------------
// looks up for component definitions
int AMSParser::ParseComponents(int start_s)
{    
    cmp_cnt = 0;
    el_cnt = 0;
    v_cnt = 0;

    has_blocks = 0;
    has_components = 0;
    int s = start_s;
    if (GetSmb(++s)->Is(" ")) s++;
    if (GetSmb(s)->Not("begin")) throw new ParseError("'begin' expected",GetSmb(s));
    s++;
    has_blocks = FindComponent("GC_BLOCK",s);
    has_components += FindComponent("GC_CAP",s);
    has_components += FindComponent("GC_GYR",s);
    has_components += FindComponent("GC_CON",s);
    if (has_components) has_components = 1;

    printf("%d components found\n",cmp_cnt);

    return s;
}
//------------------------------------------------------------------------------
// finds the blocks defined as component instantiation
int AMSParser::ParseBlock(int start_s)
{
    int s = start_s;
    int bs = 0;
    blc_cnt = 0;
    char blc_term[20];
    short term_cnt = 0;
    int result;

        result = s;
        s = FindSymbol("GC_BLOCK",s);
        if (s < 0) return result;

        bs = 1;

        // obtain the block name
        if (smb[s-bs]->Is(" ")) bs++; // skip space
        if (smb[s-bs]->Not(":")) throw new ParseError("expecting : in block definition",GetSmb(s));
        bs++;
        if (smb[s-bs]->Is(" ")) bs++; // skip space

        // current symbol must hold the name for the block
        if (smb[s-bs]->IsReserved()) throw new ParseError("given block name is a reserved word",smb[s-bs]);

        blc_cnt++;
        strncpy(blc[blc_cnt-1].name,smb[s-bs]->data,strlen(smb[s-bs]->data));

        //printf("Block %s\n",); // obtain the name

        // check if there are redundant name symbols
        bs++;
        if (smb[s-bs]->Is(" ")) bs++; // skip space
        if ((smb[s-bs]->Not(";"))&&(smb[s-bs]->Not("begin"))) throw new ParseError("Invalid syntax between block definitions",smb[s-bs]);

        //check the generic

        while (GetSmb(s++)->Not(";"))
        {
            if (GetSmb(s)->Is(" ")) continue; // skip space

            term_cnt = 0;

            if (GetSmb(s)->Is("generic")) // generic phrase was found
            {
                if (GetSmb(s++)->Is(" ")) s++; // skip space
                if (GetSmb(s++)->Not("map")) throw new ParseError(" map expected",GetSmb(s));
                if (GetSmb(s)->Is(" ")) s++; // skip space
                if (GetSmb(s++)->Not("(")) throw new ParseError("( expected",GetSmb(s));
                if (GetSmb(s)->Not(" ")) s--; // skip space

                while (GetSmb(s++)->Not(")"))
                {
                    if (GetSmb(s)->Is(" ")) continue; // skip spaces

                    if (GetSmb(s)->Is(")")) continue;

                    if (GetSmb(s)->Is(","))
                    {
                        if (smb[s+1]->Is(")"))  throw new ParseError(" syntax error ','",GetSmb(s));
                        if (term_cnt == 0) throw new ParseError("extra comma ",GetSmb(s));
                        continue; // skip commas
                    }

                    if (GetSmb(s)->IsBlockGeneric()) // generic item found
                    {
                        strncpy(blc_term,GetSmb(s)->data,strlen(GetSmb(s)->data)+1);
                        if (term_cnt) // next term was found, check if there is a comma between
                        {
                            if ((smb[s-1]->Is(" ")) && (smb[s-2]->Not(","))) throw new ParseError("comma expected",GetSmb(s));
                            if ((smb[s-1]->Not(" ")) && (smb[s-1]->Not(","))) throw new ParseError("comma expected",GetSmb(s));
                        }
                        term_cnt++;

                        //printf("%s ",GetSmb(s)->data); // print the generic item name

                        if (GetSmb(++s)->Is(" ")) s++; // skip space
                        if (GetSmb(s++)->Not("=")) throw new ParseError(" '=>' expected",GetSmb(s));
                        if (GetSmb(s++)->Not(">")) throw new ParseError(" '=> expected",GetSmb(s));
                        if (GetSmb(s)->Is(" ")) s++; // skip space

                        char set = 0;

                        if (GetSmb(s)->Is("1")) {blc[blc_cnt-1].SetCnf(blc_term); set = 1;}
                        if (GetSmb(s)->Is("0")) {blc[blc_cnt-1].ResetCnf(blc_term); set = 1;}

                        if (strcmp(blc_term,"ig") == 0) // gyrator with -1 assigned
                        {

                            blc[blc_cnt-1].inv = 0;
                            if ((GetSmb(s)->Is("-"))&&(GetSmb(s+1)->Is("1")))
                            {                                
                                blc[blc_cnt-1].inv = 1;
                                blc[blc_cnt-1].SetCnf(blc_term);
                                s++;
                                set = 1;
                            }
                        }

                        if (set == 0) throw new ParseError(" '0' or '1' eventually '-1' expected",GetSmb(s));

                        s++;
                        continue;
                    }
                    throw new ParseError(" syntax exxor",GetSmb(s));
                    s++;
                } // while parsing the generic map
            } // if generic statement is found

            if (GetSmb(s)->Is("port")) // port phrase was found
            {
                if (GetSmb(s++)->Is(" ")) s++; // skip space
                if (GetSmb(s++)->Not("map")) throw new ParseError(" map expected",GetSmb(s));
                if (GetSmb(s)->Not("(")) throw new ParseError("( expected",GetSmb(s));

                while (GetSmb(s++)->Not(")"))
                {
                    if (GetSmb(s)->Is(" ")) continue; // skip spaces

                    if (GetSmb(s)->Is(")")) continue;

                    if (GetSmb(s)->Is(","))
                    {
                        if (smb[s+1]->Is(")"))  throw new ParseError(" syntax error ','",GetSmb(s));
                        if (term_cnt == 0) throw new ParseError("extra comma ",GetSmb(s));
                        continue; // skip commas
                    }

                    if (GetSmb(s)->IsBlockPort()) // port item found
                    {
                       if (term_cnt) // next term was found, check if there is a comma between
                       {
                            if ((smb[s-1]->Is(" ")) && (smb[s-2]->Not(","))) throw new ParseError("comma expected",GetSmb(s));
                            if ((smb[s-1]->Not(" ")) && (smb[s-1]->Not(","))) throw new ParseError("comma expected",GetSmb(s));
                        }
                        term_cnt++;

                        // copy the term name (Ni,No,i_in,i_out,j_in,j_out)
                        strncpy(blc_term,GetSmb(s)->data,strlen(GetSmb(s)->data)+1);
                        //printf("term is %s\n",blc_term);

                        if (GetSmb(s++)->Is(" ")) s++; // skip space
                        if (GetSmb(s++)->Not("=")) throw new ParseError(" '=>' expected",GetSmb(s));
                        if (GetSmb(s++)->Not(">")) throw new ParseError(" '=> expected",GetSmb(s));

                        if (GetSmb(s)->Is(" ")) s++; // skip space

                      //  printf("Key is %s ",blc_term);
                        //printf("Variable is %s\n",GetSmb(s)->data);

                        if (VariableDefined(GetSmb(s)->data))
                        {
                            AMSVariable *v = &vrs[VariableDefined(GetSmb(s)->data)-1];
                            if ((v->type == amsElectrical) && (v->used)) throw new ParseError("Port already assigned",GetSmb(s));

                            blc[blc_cnt-1].SetNode(blc_term,v);
                        }
                        else
                        {
                            throw new ParseError("Undefined variable",GetSmb(s));
                        }

                        continue;
                    }
                    throw new ParseError(" syntax error",GetSmb(s));
                } // while parsing the port map

            } // port phrase is found

     } // while parsing for semicolon
        s++;
    return 0;
}
//------------------------------------------------------------------------------
// finds the component defined as component instantiation
int AMSParser::ParseComponent(int start_s)
{
    int s = start_s;
    int bs = 0;    
    char blc_term[20];
    short term_cnt = 0;

        bs = 1;

        // obtain the block name
        if (smb[s-bs]->Is(" ")) bs++; // skip space
        if (smb[s-bs]->Not(":")) throw new ParseError("expecting : in block definition",GetSmb(s));
        bs++;
        if (smb[s-bs]->Is(" ")) bs++; // skip space

        // current symbol must hold the name for the block
        if (smb[s-bs]->IsReserved()) throw new ParseError("given block name is a reserved word",smb[s-bs]);

        // Component name is here
        // Instantiate the component
        el_cnt++;
        strncpy(el[el_cnt-1].name,smb[s-bs]->data,strlen(smb[s-bs]->data)+1);
        el[el_cnt-1].dim = 0; // default dimension
        el[el_cnt-1].sign = 1; // default sign
        el[el_cnt-1].index = el_cnt;

        el[el_cnt-1].alias[0] = 'x';
	sprintf(el[el_cnt-1].alias+1,"%d",el_cnt);
//        itoa(el_cnt,el[el_cnt-1].alias+1,10);

        el[el_cnt-1].type = etAny;
        if (strcmp(smb[s]->data,"GC_CAP") == 0) el[el_cnt-1].type = etCapacitor;
        if (strcmp(smb[s]->data,"GC_CON") == 0) el[el_cnt-1].type = etConductance;
        if (strcmp(smb[s]->data,"GC_GYR") == 0) el[el_cnt-1].type = etGyrator;
        if (el[el_cnt-1].type == etAny) throw new ParseError("unsupported element type",smb[s-bs]);

        // check if there are redundant name symbols
        bs++;
        if (smb[s-bs]->Is(" ")) bs++; // skip space
        if ((smb[s-bs]->Not(";"))&&(smb[s-bs]->Not("begin"))) throw new ParseError("Invalid syntax between element definitions",smb[s-bs]);

        //check the generic
        while (GetSmb(s++)->Not(";"))
        {
            if (GetSmb(s)->Is(" ")) continue; // skip space

            term_cnt = 0;

            if (GetSmb(s)->Is("generic")) // generic phrase was found
            {
                if (el[el_cnt-1].type != etCapacitor) throw new ParseError(" generic statement supported for capacitors only",GetSmb(s));

                if (GetSmb(s++)->Is(" ")) s++; // skip space
                if (GetSmb(s++)->Not("map")) throw new ParseError(" map expected",GetSmb(s));
                if (GetSmb(s)->Is(" ")) s++; // skip space
                if (GetSmb(s++)->Not("(")) throw new ParseError("( expected",GetSmb(s));
                if (GetSmb(s)->Not(" ")) s--; // skip space

                while (GetSmb(s++)->Not(")"))
                {
                    if (GetSmb(s)->Is(" ")) continue; // skip spaces

                    if (GetSmb(s)->Is(")")) continue;

                    if (GetSmb(s)->Is(","))
                    {
                        if (smb[s+1]->Is(")"))  throw new ParseError(" syntax error ','",GetSmb(s));
                        if (term_cnt == 0) throw new ParseError("extra comma ",GetSmb(s));
                        continue; // skip commas
                    }

                    if (GetSmb(s)->IsComponentGeneric()) // generic item found
                    {

                        strncpy(blc_term,GetSmb(s)->data,strlen(GetSmb(s)->data)+1);
                        if (term_cnt) // next term was found, check if there is a comma between
                        {
                            if ((smb[s-1]->Is(" ")) && (smb[s-2]->Not(","))) throw new ParseError("comma expected",GetSmb(s));
                            if ((smb[s-1]->Not(" ")) && (smb[s-1]->Not(","))) throw new ParseError("comma expected",GetSmb(s));
                        }
                        term_cnt++;

                        //printf("%s ",GetSmb(s)->data); // print the generic item name

                        if (GetSmb(++s)->Is(" ")) s++; // skip space
                        if (GetSmb(s++)->Not("=")) throw new ParseError(" '=>' expected",GetSmb(s));
                        if (GetSmb(s++)->Not(">")) throw new ParseError(" '=> expected",GetSmb(s));
                        if (GetSmb(s)->Is(" ")) s++; // skip space

                        // assign dimension
                        el[el_cnt-1].dim=atoi(GetSmb(s)->data);

                        s++;
                        continue;
                    }
                    throw new ParseError(" syntax error",GetSmb(s));
                    s++;
                } // while parsing the generic map
            } // if generic statement is found

            if (GetSmb(s)->Is("port")) // port phrase was found
            {
                if (GetSmb(s++)->Is(" ")) s++; // skip space
                if (GetSmb(s++)->Not("map")) throw new ParseError(" map expected",GetSmb(s));
                if (GetSmb(s)->Not("(")) throw new ParseError("( expected",GetSmb(s));

                while (GetSmb(s++)->Not(")"))
                {
                    if (GetSmb(s)->Is(" ")) continue; // skip spaces

                    if (GetSmb(s)->Is(")")) continue;

                    if (GetSmb(s)->Is(","))
                    {
                        if (smb[s+1]->Is(")"))  throw new ParseError(" syntax error ','",GetSmb(s));
                        if (term_cnt == 0) throw new ParseError("extra comma ",GetSmb(s));
                        continue; // skip commas
                    }

                    if (GetSmb(s)->IsComponentPort()) // port item found
                    {
                       if (term_cnt) // next term was found, check if there is a comma between
                       {
                            if ((smb[s-1]->Is(" ")) && (smb[s-2]->Not(","))) throw new ParseError("comma expected",GetSmb(s));
                            if ((smb[s-1]->Not(" ")) && (smb[s-1]->Not(","))) throw new ParseError("comma expected",GetSmb(s));
                        }
                        term_cnt++;

                        // copy the term name (Ni,No,i_in,i_out,j_in,j_out)
                        strncpy(blc_term,GetSmb(s)->data,strlen(GetSmb(s)->data)+1);
                        //printf("term is %s\n",blc_term);

                        if (GetSmb(s++)->Is(" ")) s++; // skip space
                        if (GetSmb(s++)->Not("=")) throw new ParseError(" '=>' expected",GetSmb(s));
                        if (GetSmb(s++)->Not(">")) throw new ParseError(" '=> expected",GetSmb(s));

                        if (GetSmb(s)->Is(" ")) s++; // skip space

                        if (VariableDefined(GetSmb(s)->data))
                        {

                            AMSVariable *v = &vrs[VariableDefined(GetSmb(s)->data)-1];
                            if ((v->type == amsElectrical) && (v->used)) throw new ParseError("Port already assigned",GetSmb(s));

                            //printf("Key is %s ",blc_term);
                            //printf("Index is %d ",v->index);
                            //printf("Variable is %s\n",GetSmb(s)->data);

                            if (strcmp(blc_term,"Ni")==0) // input node
                            {
                                if (v->dir == pdGround) throw new ParseError("Input node cannot be grounded",GetSmb(s));

                                if (el[el_cnt-1].type == etConductance)
                                {
                                    // external signal connected
                                    if (v->type == amsElectrical)
                                    {
                                        // for Ni of conductance only input is allowed
                                        if (v->dir == pdInput)
                                        {
                                         strncpy(el[el_cnt-1].sig,v->name,strlen(v->name)+1);
                                         el[el_cnt-1].role=erGi1;
                                        }
                                        else
                                          throw new ParseError("Only input signals may stimulate node",GetSmb(s));
                                    }
                                }

                                el[el_cnt-1].vin=v->index;

                            } // Ni

                            if (strcmp(blc_term,"Nj")==0) // output node
                            {                                    
                                el[el_cnt-1].vout=v->index;
                                if ((el[el_cnt-1].type == etGyrator) && (v->index==0))
                                    throw new ParseError("grounded output of gyrator!",GetSmb(s));
                            } // Nj

                            if (strcmp(blc_term,"Vo")==0)
                            {
                                if (el[el_cnt-1].type == etConductance)                                
                                {
                                   strncpy(el[el_cnt-1].sig,v->name,strlen(v->name)+1);
                                   el[el_cnt-1].role=erGj2;
                                }
                                else
                                   throw new ParseError("Output signal may be acquired from conductance only",GetSmb(s));
                            }

                            // formal rules for assigning roles
                            switch(el[el_cnt-1].type)
                            {
                            case etCapacitor:
                                {                                        
                                    if (el[el_cnt-1].vin != el[el_cnt-1].vout) el[el_cnt-1].role=erCf;
                                    if ((el[el_cnt-1].vin != 0) &&  (el[el_cnt-1].vout == 0)) el[el_cnt-1].role=erCi;
                                    break;
                                }
                            case etGyrator:
                                {
                                    el[el_cnt-1].role=erIg;
                                    break;
                                }                            
                                default:
                                break;
                            }

                        }
                        else
                        {
                            throw new ParseError("Undefined variable",GetSmb(s));
                        }

                        continue;
                    }
                    throw new ParseError(" syntax error",GetSmb(s));
                } // while parsing the port map

            } // port phrase is found

     } // while parsing for semicolon
        s++;
    return 0;
}
//------------------------------------------------------------------------------
// makes a list of blocks
void AMSParser::ShowBlocks()
{
    for (int i=0; i < blc_cnt; i++)
    {
        blc[i].Display();
    } // for
}
//------------------------------------------------------------------------------
// returns the number of ports with a given direction d
char AMSParser::CountVariables(TAMSDirection d)
{
    char result = 0;
    for (int i=0; i < vrs_cnt; i++)
    {
        if (vrs[i].dir == d) result++;
    } // for
    return result;
}
//------------------------------------------------------------------------------
int AMSParser::FindSymbol(const char *data,int start = 0)
{
    if (start < 0) throw new AMSError(amsOutOfBounds);

    for (int i = start; i < smb_cnt; i++)
    {
        if (strcmp(smb[i]->data,data) == 0) return i;
    }
    return -1;
}
//------------------------------------------------------------------------------
AMSSymbol* AMSParser::AddSymbol(char *data,unsigned int new_row, unsigned int new_col)
{
    smb = (AMSSymbol **) realloc(smb,(smb_cnt+1)*sizeof(AMSSymbol*));
    if (smb == NULL) throw new AMSError(amsMemoryStorage);
    smb_cnt++;

    AMSSymbol *sp = new AMSSymbol(data,new_row,new_col);
    smb[smb_cnt-1] = sp;
    return sp;
}
//------------------------------------------------------------------------------
AMSSymbol* AMSParser::AddSymbol(char c,unsigned int new_row, unsigned int new_col)
{
    if (smb_cnt >=1)
    {
        if ( (c == ' ') && (smb[smb_cnt-1]->data[0] == ' '))
        {
            return smb[smb_cnt-1];
        }
    }

    smb = (AMSSymbol **) realloc(smb,(smb_cnt+1)*sizeof(AMSSymbol*));
    if (*smb == NULL) throw new AMSError(amsMemoryStorage);
    smb_cnt++;

    char buf[] ={c,0x00};

    AMSSymbol *sp = new AMSSymbol(buf,new_row,new_col);
    sp->IsSep = 1;
    smb[smb_cnt-1] = sp;
    return sp;
}
//------------------------------------------------------------------------------
AMSSymbol* AMSParser::GetSmb(int s)
{
    if (s >= smb_cnt) throw new ParseError("Unexpected end of file",smb[smb_cnt-1]);
    return smb[s];
}
//------------------------------------------------------------------------------
// makes a list of the symbol
void AMSParser::ShowSymbols()
{
    for ( int i = 0; i < smb_cnt; i++)
    {
        if (smb[i]) smb[i]->Display(stdout);
        printf("\n");
    }
}
//------------------------------------------------------------------------------
// splits a text line into symbols
void AMSParser::SplitSymbols(const char *b,unsigned int the_row)
{
    char c = 0x00;
    unsigned int j=0;
    char buf[1024];

    unsigned char had_spaces = 0;

    for (unsigned int i = 0; i < strlen(b)+1; i++)
    {
        c = b[i];

        if (c == 9) continue; // tab

        if ((c == '"')||
            (c == '#')||
            (c == '&')||
            (c == 39)||
            (c == '(')||
            (c == ')')||
            (c == '*')||
            (c == '+')||
            (c == '-')||
            (c == ',')||
            (c == '.')||
            (c == '/')||
            (c == ':')||
            (c == ';')||
            (c == '<')||
            (c == '=')||
            (c == '>')||
            (c == '[')||
            (c == ']')||
            (c == '|')||
            (c == ' ')||
            (c == 10)
           )
        {

            if ((c =='-') && (b[i+1] == '-')) // comment start
            {
                break;
            }

            if (j)
            {
                buf[j] = 0x00;
               // printf("|%s|\n",buf);
                AddSymbol(buf,the_row,i-j);
            }
            j=0;

            if (c != ' ') had_spaces = 0;

            if ((c == ' ')&&(had_spaces == 0)) // space was found
            {
                AddSymbol(c,the_row,i-j);
                had_spaces = 1;
            }

            if (had_spaces) continue;

            if (c != 10)  // separator was found
            {
              // printf("separator [%c]\n",c);
                AddSymbol(c,the_row,i-j);
            }

        continue;
        }

        if (c == 13) break; // CR
        if (c == 10) break; // LF

       buf[j++] = c;

    } // for

}
//------------------------------------------------------------------------------
// searches for the basic elements of an ams model
int AMSParser::IsAMSModel()
{
    int result = 3;
    if (FindSymbol("entity",0) == -1) {fprintf(stdout,"Found no entity declaration\n"); result--;}
    if (FindSymbol("entity",0) == -1) {fprintf(stdout,"Found no architecture declaration\n"); result--;}
    if (FindSymbol("architecture",0) == -1) {fprintf(stdout,"Found no begin statement\n"); result--;}
    return result;
}
//------------------------------------------------------------------------------
// checks the proper form of the entity
int AMSParser::ValidateEntity()
{
    vrs_cnt = 0;  // reset number of used ports

    int s = FindSymbol("entity",0);

    // check if entity exists
    if (s == -1) throw new ParseError("No entity found",smb[smb_cnt-1]);

    if (GetSmb(++s)->Not(" ")) throw new ParseError("space expected",GetSmb(s));

    s++;
    strncpy(EntityName,GetSmb(s)->data,strlen(GetSmb(s)->data)+1); // here comes the entity name

    if (GetSmb(s++)->Is(" ")) s++;

    if (GetSmb(s)->Not("is")) throw new ParseError("'is' expected",GetSmb(s));

    s++;

    if (GetSmb(s)->Is(" ")) s++;

    if (GetSmb(s)->Not("port")) throw new ParseError("'port' expected",GetSmb(s));

    if (GetSmb(++s)->Is(" ")) s++;

    if (GetSmb(s)->Not("(")) throw new ParseError("'(' expected",GetSmb(s));

    if (GetSmb(++s)->Is(" ")) s++;

    // parse the ports
    do
    {
        if (GetSmb(s++)->Not("terminal")) throw new ParseError("'terminal' expected",GetSmb(s));       
        if (GetSmb(s)->Is(" ")) s++;        

        if (GetSmb(s)->Not("input") && GetSmb(s)->Not("output") && GetSmb(s)->Not("ground")) throw new ParseError("bad port direction",GetSmb(s));

        if (GetSmb(s)->Is("input")) vrs[vrs_cnt].dir=pdInput;
        if (GetSmb(s)->Is("output")) vrs[vrs_cnt].dir=pdOutput;
        if (GetSmb(s)->Is("ground"))
        {
            if (GroundDefined()) throw new ParseError("Ground terminal already defined",GetSmb(s));
            vrs[vrs_cnt].dir = pdGround;
        }
        s++;
        if (GetSmb(s)->IsReserved()) throw new ParseError("expected identifier",GetSmb(s));

        if (VariableDefined(smb[s]->data)) throw new ParseError("name already used",GetSmb(s));

        strncpy(vrs[vrs_cnt].name,smb[s]->data,strlen(smb[s]->data)+1);
        s++;
        if (GetSmb(s++)->Not(":"))throw new ParseError("':' expected",GetSmb(s));
        if (GetSmb(s)->Is(" ")) s++;
        if (GetSmb(s)->Not("electrical"))throw new ParseError("'electrical' expected",GetSmb(s));
        vrs[vrs_cnt].type = amsElectrical;
        s++;
        vrs_cnt++;
        if (GetSmb(s)->Is(")")) break;
        if (GetSmb(s)->Is(" ")) s++;
        if (GetSmb(s)->Not(";"))throw new ParseError("';' expected",GetSmb(s));
        s++;
        if (GetSmb(s)->Is(" ")) s++;
    }
    while (GetSmb(s)->Is("terminal"));
    // end of port parsing block    

    if (CountVariables(pdGround) == 0) throw new ParseError("No grounded terminal defined",GetSmb(s));
    if (CountVariables(pdInput) == 0) throw new ParseError("No input terminals defined",GetSmb(s));
    if (CountVariables(pdOutput) == 0) throw new ParseError("No output terminals defined",GetSmb(s));

    if (GetSmb(s)->Not(")")) throw new ParseError("')' expected",GetSmb(s));

    if (GetSmb(s++)->Is(" ")) s++;

    if (GetSmb(s)->Not(";")) throw new ParseError("';' expected",GetSmb(s));

    if (GetSmb(++s)->Is(" ")) s++;

    if (GetSmb(s)->Not("end")) throw new ParseError("'end' expected",GetSmb(s));

    if (GetSmb(++s)->Is(" ")) s++;

    if (GetSmb(s)->Not("entity")) throw new ParseError("'entity' expected",GetSmb(s));

    if (GetSmb(++s)->Is(" ")) s++;

    if (strcmp(GetSmb(s)->data,EntityName)!=0) throw new ParseError("previous entity name expected",GetSmb(s));

    if (GetSmb(s++)->Is(" ")) s++;

    if (GetSmb(s)->Not(";")) throw new ParseError("';' expected",GetSmb(s));

    // check for multiple entities

    if (FindSymbol("entity",++s) != -1) throw new ParseError("Found multiple entities",GetSmb(s));

    return s;
}
//------------------------------------------------------------------------------
// finds the architecture associated with the entity
int AMSParser::FindArchitecture(int start_s)
{
    int s = start_s;

    if (GetSmb(s)->Is(" ")) s++;

    if (GetSmb(s)->Not("architecture")) throw new ParseError("Architecture expected",GetSmb(s));

    if (GetSmb(s++)->Is(" ")) s++;
    s++;
    strncpy(ArchitectureName,GetSmb(s)->data,strlen(GetSmb(s)->data)+1);// here comes the architecture name

    if (GetSmb(s++)->Is(" ")) s++;


    if (GetSmb(s)->Not("of")) throw new ParseError("'of' expected",GetSmb(s));
    if (GetSmb(s++)->Is(" ")) s++;

    if (strcmp(GetSmb(s)->data,EntityName)!=0) throw new ParseError("valid entity name expected",GetSmb(s));

    if (GetSmb(s++)->Is(" ")) s++;

    if (GetSmb(s)->Not("is")) throw new ParseError("'is' expected",GetSmb(s));

    return s;
}
//------------------------------------------------------------------------------
int AMSParser::FindEndArchitecture(int start_s)
{
    int s = start_s;

    if (GetSmb(s)->Is(" ")) s++;

    if (GetSmb(s)->Not("end")) throw new ParseError("End of architecture expected",GetSmb(s));

    if (GetSmb(s)->Is(" ")) s++;

    if (GetSmb(s++)->Not("architecture")) throw new ParseError("End of architecture expected",GetSmb(s));

    if (GetSmb(s)->Is(" ")) s++;

    if (GetSmb(s++)->Not(";")) throw new ParseError("';' expected",GetSmb(s));

    return s;
}
//------------------------------------------------------------------------------
// main parsing routine
int AMSParser::Parse(const char *filename)
{
    FILE *fil = NULL;

    fil = fopen(filename,"r");
    if (fil == NULL) throw new AMSError(amsNoFile);

    fprintf(stdout,"\nParsing file %s ... \n",filename);
    fflush(stdout);
    char lbuf[1024];

    unsigned int row_cnt = 0;
    while (!feof(fil))
    {        
        fgets(lbuf,sizeof(lbuf),fil);
        strcat(lbuf," ");
        SplitSymbols(lbuf,++row_cnt);
    }

    if (fil) fclose(fil);

	
	
    if(IsAMSModel() == 0) return 0;

    int s = ValidateEntity();

    s = FindVariables(s++);
	
    s  = ParseComponents(s++);
	
	printf("WARNING: FindEndArchitecture routine is now omitted\n");
    //s = FindEndArchitecture(s++);	
    
    // count vertices for elements
    for (int i=0; i<el_cnt; i++)
    {
        if (el[i].vin > v_cnt) v_cnt = el[i].vin;
        if (el[i].vout > v_cnt) v_cnt = el[i].vout;
    }


    fprintf(stdout,"done!\n");
    fflush(stdout);

    return 1;
}
//------------------------------------------------------------------------------
// determines how many variables were used for block description
int AMSParser::IndexUsedVariables()
{
    int result = 0;
    for (int v = 0; v < vrs_cnt; v++)
    {
        if (vrs[v].used)
        {
            if (vrs[v].type == amsReal) result++;
            continue;
        }

        if (vrs[v].dir == pdGround) continue;        
        printf("NOTICE: Variable %s declared but not used\n",vrs[v].name);        

        if (vrs[v].type == amsElectrical)
            printf("WARNING: Unconnected port %s\n",vrs[v].name);

    }
    return result;
}
//------------------------------------------------------------------------------
// produces the BLO file either from generic BLO or from the AMS file
void AMSParser::MakeBLO(const char *input_file, const char *output_file)
{

    if (Parse(input_file))
    {

        //ShowSymbols();
        //ShowVariables();
        //ShowBlocks();

        SaveAsBLO(output_file);
    }
    else
    {
        fprintf(stdout,"WARNING: No AMS entities found. Trying to parse file as generic BLO\n");
        fflush(stdout);
        CopyAsBLO(input_file,output_file);
    }
}
//------------------------------------------------------------------------------
// stores the list of blocks into a BLO file
void AMSParser::SaveAsBLO(const char *filename)
{
    FILE *fil = NULL;
    fil = fopen(filename,"w");
    if (fil == NULL) throw new AMSError(amsNoFile);

    fprintf(fil,"%% Created by AMSParser v %d\n",VERSION_NR);
    fprintf(fil,"%% Source file:  %s\n",filename);
    fprintf(fil,"%% -----------------------\n");

    fprintf(fil,"%d %d\n",blc_cnt,IndexUsedVariables());

    // write raw block configuration
    for (int b = 0; b < blc_cnt; b++)
    {
        blc[b].WriteCnfToFile(fil);
        fprintf(fil," %d %d %d 0 ",b+1,blc[b].Ni.index,blc[b].Nj.index);        
        fprintf(fil,"\n");
    }
    fprintf(fil,"%% -----------------------\n");
    // write additional block information
    for (int b = 0; b < blc_cnt; b++)
    {
        fprintf(fil,"%%! B:%d Bl:%s Ni:%s Nj:%s",b+1,blc[b].name,blc[b].Ni.name,blc[b].Nj.name);
        if (blc[b].i_in.used) fprintf(fil," i_in:%s",blc[b].i_in.name);
        if (blc[b].i_out.used) fprintf(fil," i_out:%s",blc[b].i_out.name);
        if (blc[b].j_in.used) fprintf(fil," j_in:%s",blc[b].j_in.name);
        if (blc[b].j_out.used) fprintf(fil," j_out:%s",blc[b].j_out.name);
        fprintf(fil," \n");
    }
    fprintf(fil,"%% -----------------------\n");
    if (fil) fclose(fil);

    fprintf(stdout,"\nStored as %s\n",filename);
    fflush(stdout);
}
//------------------------------------------------------------------------------
// stores the list of blocks into a BLO file
void AMSParser::CopyAsBLO(const char *in_file,const char *out_file)
{
    FILE *ofil = NULL;
    ofil = fopen(out_file,"w");
    if (ofil == NULL) throw new AMSError(amsNoFile);

    FILE *ifil = NULL;
    ifil = fopen(in_file,"r");
    if (ifil == NULL) throw new AMSError(amsNoFile);

    fprintf(ofil,"%% Created by AMSParser v %d\n",VERSION_NR);
    fprintf(ofil,"%% Source file:  %s\n",in_file);
    fprintf(ofil,"%% -----------------------\n");

    char buf[1024];
    while (!feof(ifil))
    {
        fgets(buf,sizeof(buf),ifil);
        fprintf(ofil,"%s",buf);
    }

    if (ofil) fclose(ofil);
    if (ifil) fclose(ifil);

    fprintf(stdout,"\nStored as %s\n",out_file);
    fflush(stdout);
}
//------------------------------------------------------------------------------
