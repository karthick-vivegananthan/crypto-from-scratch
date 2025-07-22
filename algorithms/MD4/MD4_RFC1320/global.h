/* GLOBAL.H - RSAREF types and constants
 */
/* PROTOTYPES should be set to one if and only if the compiler supports
 function argument prototyping.
 The following makes PROTOTYPES default to 0 if it has not already
 been defined with C compiler flags.
 */
#include <stdint.h> /* For uint16_t and uint32_t types */

#ifndef PROTOTYPES
#define PROTOTYPES 0
#endif
/* POINTER defines a generic pointer type */
typedef unsigned char *POINTER;
/* UINT2 defines a two byte word */
//typedef unsigned short int UINT2;
typedef uint16_t UINT2;
/* UINT4 defines a four byte word */
//typedef unsigned long int UINT4; // Since unsigned long int in 64 bit systems is 8 bytes, we use uint32_t for 4 bytes
typedef uint32_t UINT4;
/* PROTO_LIST is defined depending on how PROTOTYPES is defined above.
 If using PROTOTYPES, then PROTO_LIST returns the list, otherwise it
 returns an empty list.
 */
#if PROTOTYPES
#define PROTO_LIST(list) list
#else
#define PROTO_LIST(list) ()
#endif