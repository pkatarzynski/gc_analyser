#ifndef AMS_PARSER_HPP
#define AMS_PARSER_HPP

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "const.h"

//------------------------------------------------------------------------------
enum TAMSErr {amsDefault,amsNoFile,amsMemoryStorage,amsOutOfBounds};
enum TAMSType {amsUnknown,amsReal,amsElectrical};
enum TAMSDirection {pdInput,pdOutput,pdGround,pdUnknown};
//------------------------------------------------------------------------------
class AMSSymbol;
//------------------------------------------------------------------------------
class AMSVariable
{
    public:
    AMSVariable();
    void Display(FILE *fil = stdout);
    void Empty();
    TAMSType type;
    char name[30];
    int index;
    char used;
    TAMSDirection dir;
};
//------------------------------------------------------------------------------
#define bit_Ci 7
#define bit_Cj 6
#define bit_Cf 5
#define bit_ig 4
#define bit_Gi1 3
#define bit_Gi2 2
#define bit_Gj1 1
#define bit_Gj2 0
//------------------------------------------------------------------------------
class GCBlock
{
    public:
        GCBlock();
        void SetName(const char *new_name);
        void SetCnf(const char *term);
        void ResetCnf(const char *term);
        void SetNode(const char *term,AMSVariable *n);
        void Display(FILE *fil);
        void WriteToFile(FILE *fil);
        void WriteCnfToFile(FILE *fil);
        char name[30];
        unsigned char cnf; // configuration as a bit vector [Gi1,Gi2,Go2,Go2,ig1,Ci,Co,Cf]
        char inv; // is the gyrator inverted
        AMSVariable Ni;
        AMSVariable Nj;
        AMSVariable i_in;
        AMSVariable i_out;
        AMSVariable j_in;
        AMSVariable j_out;
};
//------------------------------------------------------------------------------
class AMSError
{
    public:
        AMSError(unsigned char new_code);
        void Display();
    private:
        unsigned char code;
};
//------------------------------------------------------------------------------
class ParseError
{
    public:
        ParseError(const char *new_msg,AMSSymbol *smb);
        ~ParseError();
        void Display(FILE *output);
    private:
        char *msg;
        int row;
        int col;
        char data[200];
};
//------------------------------------------------------------------------------
class AMSSymbol
{
    public:
    AMSSymbol(const char *origin, unsigned int new_row, unsigned int new_col);
    ~AMSSymbol();
    inline char Is(const char *c);
    inline char Not(const char *c);
    char IsReserved(); // determines is a given phrase is a reserved word
    char IsBlockGeneric(); // determintes if the phrase is a generic from block    
    char IsBlockPort();// determines if the phrase is a block port (i_in,i_out,j_in,j_out)

    char IsComponentGeneric();// determines if the phrase is a component's generic (dim)
    char IsComponentPort();// determines if the phrase is a component's generic (dim)

    char data[200];    
    void Display(FILE *fil);
    unsigned int row;
    unsigned int col;
    unsigned char IsSep;
};
//------------------------------------------------------------------------------
class AMSParser
{
public:

    AMSParser(); // default constructor
    ~AMSParser(); // default destructor

    void MakeBLO(const char *input_file, const char *output_file);

    void ShowSymbols(); // makes a list of the symbols
    void ShowVariables(); // makes a list of variables
    void ShowBlocks(); // makes a list of GC blocks    
    char CountVariables(TAMSDirection d);

    int IsAMSModel();

    int ValidateEntity();
    int FindArchitecture(int start_s);
    int FindEndArchitecture(int start_s);

    void SaveAsBLO(const char *filename);
    void CopyAsBLO(const char *in_file,const char *out_file);

    int Parse(const char *filename); // parses the input file

    AMSSymbol* AddSymbol(char *data,unsigned int new_row, unsigned int new_col);
    AMSSymbol* AddSymbol(char c,unsigned int new_row, unsigned int new_col);

    AMSSymbol* GetSmb(int s);

    int FindSymbol(const char *data,int start);
    void SplitSymbols(const char *b,unsigned int the_row); // splits a text line into symbols

    int FindVariables(int start_s);

    int IndexUsedVariables(); // determines how many variables were used in blocks

    char VariableDefined(const char *name); // determines if a given variable is already defined

    char PortDefined(const char *name); // determines if a given port name is already defined
    char GroundDefined(); // determines if the ground terminal is already defined

    int HasBlocks(); // returns 1 when parsed AMS file contains blocks
    int HasComponents(); // returns 1 when parsed AMS file contains componnts (i.e. discreete elements)

    // Fixme: make it private and dynamic
    TElement el[100]; // elements
    int el_cnt; // elements' count
    int v_cnt; // number of vertices
    private:

    struct TComponentDefinition
    {
        int start; // start of definition
        int type_loc; // location of component's type
        int end; // end of definition
    };
    int ParseBlock(int start_s);
    int ParseComponent(int start_s);
    int FindComponent(const char *type, int start_s);
    int ParseComponents(int start_s);

    char EntityName[50];
    char ArchitectureName[50];

    AMSSymbol **smb;
    int smb_cnt;

    AMSVariable vrs[100];
    int vrs_cnt;

    GCBlock blc[100];
    int blc_cnt;



    TComponentDefinition cmp_loc[500]; // components' locations
    int cmp_cnt; // compontents' count
    int has_blocks;
    int has_components;

protected:

};

#endif // AMS_PARSER_HPP

