

#ifndef PROPERTY_H_
#define PROPERTY_H_

#include "../comm/inter_comm.h"
#include "types.h"
#include "modbus_register_table.h"
#include "ustdlib.h"

//*****************************************************************************
//
// If building with a C++ compiler, make all of the definitions in this header
// have a C binding.
//
//*****************************************************************************
#ifdef __cplusplus
extern "C"
{
#endif

typedef struct {
    unsigned short          id;
    const char *            status;
} status_lookup_t;


typedef struct
{
	long           lMin;
	long           lMax;
	unsigned short usStep;

} tRange;


typedef struct
{
	unsigned short   usProperty;
	tRange           sPropertyRange;

} tPropertyRange;

//*****************************************************************************
//
// Mark the end of the C bindings section for C++ compilers.
//
//*****************************************************************************
#ifdef __cplusplus
}
#endif

#endif /* PROPERTY_H_ */
