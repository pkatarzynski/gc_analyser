#ifndef complex_H
#define complex_H
// Includes
//-----------------------------------------------------------------
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

// The structure
//-----------------------------------------------------------------
struct complex
{	
	double re;
	double im;
};
// Functions
//-----------------------------------------------------------------
double Re(complex c); // real part

double Im(complex c); // imagined part

double Abs(complex c); // absolute value (module)

double Angle(complex c); // phase 

double Magnitude(complex c); // magnitude (linear)

double Db(complex c); // magnitude db

void Display(complex c,FILE *fil=stdout); // print 

void Clear(complex &a); // resets a to 0+j0

complex Sum(complex &a, complex &b); // sum 

void SelfSum(complex &a, complex &b); // sum 

complex Sum(complex &a, double &b); // sum  

void SelfDiff(complex &a, complex &b); // difference

complex Diff(complex &a, complex &b); // difference

complex Diff(complex &a, double &b); // difference

complex Mult(complex &a, complex &b); // multiplication

void SelfMult(complex &a, complex &b); // multiplication

void SelfMult(complex &a, double &b); // multiplication

complex Mult(complex &a, double &b); // multiplication 

void SelfDiv(complex &a, complex &b); // division 

complex Div(complex &a, complex &b); // division 

complex Div(complex &a, double &b); // division 

complex Pow(complex &a, double b); // power

void SelfPow(complex &a, double b); // power
#endif
