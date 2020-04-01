/*
 * string_utils.h
 *
 *  Created on: 07.07.2014
 *      Author: V.Dudnik
 */

#ifndef STRING_UTILS_H_
#define STRING_UTILS_H_

#include <string.h>


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

void string_space_align_center( char *str, long new_strlen );
void string_space_align_left( char *str, long new_strlen );
void string_insert( char *src_str, char *sub_str, long pos );

//*****************************************************************************
//
// Mark the end of the C bindings section for C++ compilers.
//
//*****************************************************************************
#ifdef __cplusplus
}
#endif


#endif /* STRING_UTILS_H_ */
