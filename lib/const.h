#ifndef CONST_H
#define CONST_H

// version number

#define RESULT_OK 0
#define _CRT_SECURE_NO_DEPRECATE 1

#define UNIX 1

// definitions for constant arrays 

// the buffer size for reading lines from textfile 
#define MAX_FILE_BUFFER 500


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

//definitions for error types
#define ERR_FILE_NOT_FOUND 1

#endif
