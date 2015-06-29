#ifndef QPARAMFILE_H
#define QPARAMFILE_H

#include <QMainWindow>
#include <QDir>
#include "param_file.hpp"

class ParamFile:  public CParamFile
{
public:

    ParamFile();
    ~ParamFile();

    void Put(QString label,int data);	 			// put integer
    void Put(QString label,int cnt, int *data); 	// put integer vector
    void Put(QString label,float data);			// put float
    void Put(QString label,int cnt, float *data); // put float vector    
    void Put(QString label,QString data);			// put string


    int Get(QString label,int *data,int mandatory);				// get integer
    int Get(QString label,QString *data);			// get string
    int Get(QString label,float *data);			// get float
    int Get(QString label,int *cnt, float **data);	// get float vector
    int Get(QString label,int *cnt, int **data);	// get int vector
    int SaveToFile(QString file_name);
    int LoadFromFile(QString file_name);

private:


};

#endif // QPARAMFILE_H
