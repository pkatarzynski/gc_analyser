#include "complex.h"

//-----------------------------------------------------------------
double Re(complex c) {return c.re;}
//-----------------------------------------------------------------
double Im(complex c) {return c.im;}
//-----------------------------------------------------------------
double Abs(complex c) {return sqrt(c.re*c.re+c.im*c.im);}
//-----------------------------------------------------------------
double Angle(complex c) {return atan2(c.im,c.re);}
//-----------------------------------------------------------------
double Magnitude(complex c){return Abs(c);}
//-----------------------------------------------------------------
double Db(complex c){return 20*log10(Abs(c));}
//-----------------------------------------------------------------
void Display(complex c,FILE *fil)
{
	fprintf(fil,"%f",c.re);
	if (c.im >= 0) fprintf(fil,"+");
	fprintf(fil,"%fi",c.im);
}
//-----------------------------------------------------------------
void SelfSum(complex &a, complex &b){ a.im+=b.im; a.re+=b.re;}
//-----------------------------------------------------------------
void Clear(complex &a){a.re = 0.0; a.im = 0.0;}
//-----------------------------------------------------------------
complex Sum(complex &a, double &b)
{
	complex result;
	result.im = a.im;
	result.re = a.re+b;
	return result;
}
//-----------------------------------------------------------------
complex Sum(complex &a, complex &b)
{
	complex result;
	result.re = a.re + b.re;	
	result.im = a.im + b.im;
	return result;	
}
//-----------------------------------------------------------------
void SelfDiff(complex &a, complex &b) {a.im -= b.im; a.re -= b.re;}
//-----------------------------------------------------------------
complex Diff(complex &a, complex &b)
{
	complex result;
	result.re = a.re - b.re;	
	result.im = a.im - b.im;
	return result;	
}
//-----------------------------------------------------------------
complex Diff(complex &a, double &b)
{
	complex result;
	result.im = a.im;
	result.re = a.re-b;
	return result;
}
//-----------------------------------------------------------------
complex Mult(complex &a, double &b)
{
	complex result;
	result.re = a.re*b;	
	result.im = a.im*b;
	return result;	
}
//-----------------------------------------------------------------
void SelfMult(complex &a, complex &b)
{
	double f_a = Angle(a);
	double f_b = Angle(b);
	double m_a = Abs(a);
	double m_b = Abs(b);

	complex result;

	result.re = m_a*m_b*cos(f_a+f_b);	
	result.im = m_a*m_b*sin(f_a+f_b);

	a.im = result.im;
	a.re = result.re;

}
//-----------------------------------------------------------------
void SelfMult(complex &a, double &b)
{
	a.re *= b;
	a.im *= b;
}
//-----------------------------------------------------------------
complex Mult(complex &a, complex &b)
{
	double f_a = Angle(a);
	double f_b = Angle(b);
	double m_a = Abs(a);
	double m_b = Abs(b);

	complex result;

	result.re = m_a*m_b*cos(f_a+f_b);	
	result.im = m_a*m_b*sin(f_a+f_b);

	return result;	
}
//-----------------------------------------------------------------
complex Div(complex &a, complex &b)
{
	double f_a = Angle(a);
	double f_b = Angle(b);
	double m_a = Abs(a);
	double m_b = Abs(b);

	complex result;
	result.re = (m_a/m_b)*cos(f_a-f_b);	
	result.im = (m_a/m_b)*sin(f_a-f_b);

	return result;	
}
//-----------------------------------------------------------------
void SelfDiv(complex &a, complex &b)
{
	double f_a = Angle(a);
	double f_b = Angle(b);
	double m_a = Abs(a);
	double m_b = Abs(b);

	complex result;
	result.re = (m_a/m_b)*cos(f_a-f_b);	
	result.im = (m_a/m_b)*sin(f_a-f_b);

	a.im = result.im;
	a.re = result.re;
}
//-----------------------------------------------------------------
complex Div(complex &a, double &b)
{
	complex result;
	result.re = a.re/b;	
	result.im = a.im/b;
	return result;	
}
//-----------------------------------------------------------------
complex Pow(complex &a, double b)
{
	complex result;
	result.re = pow(Abs(a),b)*cos(Angle(a)*b);	
	result.im = pow(Abs(a),b)*sin(Angle(a)*b);	
	return result;	
}
//-----------------------------------------------------------------
void SelfPow(complex &a, double b)
{
	complex result;
	result.re = pow(Abs(a),b)*cos(Angle(a)*b);	
	result.im = pow(Abs(a),b)*sin(Angle(a)*b);	
	a.im = result.im;
	a.re = result.re;
}
//-----------------------------------------------------------------
