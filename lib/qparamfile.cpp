#include "qparamfile.h"

//---------------------------------------------------------------
ParamFile::ParamFile()
{
  lEntries = 0;
  Clear();
}
//---------------------------------------------------------------
ParamFile::~ParamFile()
{
    lEntries = 0;
    Clear();
}

//---------------------------------------------------------------

void ParamFile::Put(QString label,int data) { this->PutInt(label.toAscii().data(),data);}

void ParamFile::Put(QString label,float data){ this->Put(label.toAscii().data(),data);}

void ParamFile::Put(QString label,int cnt, float *data){this->Put(label.toAscii().data(),cnt,data);}

void ParamFile::Put(QString label,QString data){ this->PutLStr(label.toAscii().data(),data.toAscii().data(),data.size());}

int ParamFile::Get(QString label,int *data,int mandatory=0) {return this->GetInt(label.toAscii().data(),data,mandatory);}

int ParamFile::Get(QString label,QString *data)
{
    char buf[1024];
    int idx = GetLStr(label.toAscii().data(),&buf[0]);
    if (idx >= 0) data->sprintf("%s",buf);
    return idx;
}

int ParamFile::Get(QString label,float *data){return this->Get(label.toAscii().data(),data);}
int ParamFile::Get(QString label,int *cnt, float **data){return this->Get(label.toAscii().data(),cnt,data);}
int ParamFile::Get(QString label,int *cnt, int **data){return this->Get(label.toAscii().data(),cnt,data);}

int  ParamFile::SaveToFile(QString file_name)
{
    QDir dummy;
    dummy.setPath(file_name);
    QString native_path = dummy.toNativeSeparators(file_name);
    CParamFile::SaveToFile(native_path.toAscii().data());
    return 0;
}

int  ParamFile::LoadFromFile(QString file_name)
{    
    QDir dummy;
    dummy.setPath(file_name);
    QString native_path = dummy.toNativeSeparators(file_name);
    CParamFile::LoadFromFile(native_path.toAscii().data());
    return 0;
}
//-----------------------------------------------------------------
