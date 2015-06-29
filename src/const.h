#ifndef CONST_H
#define CONST_H

// version number
#define VERSION_NR 20150620

#define RESULT_OK 0
#define _CRT_SECURE_NO_DEPRECATE 1

#define UNIX 1

// definitions for constant arrays 

// the buffer size for reading lines from textfile 
#define MAX_FILE_BUFFER 500



//-----------------------------------------------------------------

//definitions for element types
/*#define ANY_TYPE    0x0F
#define CAPACITOR   1
#define GYRATOR     2
#define CONDUCTANCE 4
  */

enum TElementType {etAny,etCapacitor,etGyrator,etConductance};
enum TElementRole {erNone,erCi,erCj,erCf,erIg,erGi1,erGi2,erGj1,erGj2};

// definitions for element classes 
/*#define CLASS_DEFAULT  0
#define CLASS_CI  1
#define CLASS_CO  2
#define CLASS_CF  4
#define CLASS_GI1 8
#define CLASS_GI2 16
#define CLASS_GJ1 32
#define CLASS_GO2 64
#define CLASS_IG  128*/

struct TElement // single element
{
        TElementType type;  // type of the element
        char name[20];  // name of the element (used in symbolic analysis)
        char sig[20]; // name of input/output signal connected to the element
        char alias[20]; // unified symbolic alias for the element (eg. x12)
        unsigned short index;	// numerical index
        double value;	// value assigned to the element
        short vin;		// input vertex index
        short vout;		// output vertex index
        short sign;		// sign denoting vertex match
        TElementRole role; // flags for element class
        unsigned short block_id; // block index
        unsigned short dim; // dimension
};

//definitions for error types
#define ERR_FILE_NOT_FOUND 1

#endif
