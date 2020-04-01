/*
 * types.h
 *
 *  Created on: 10 янв. 2013
 *      Author: iotrokha
 */

#ifndef TYPES_H_
#define TYPES_H_

#include <stm32f2xx.h>
#include <stm32f2xx_conf.h>

#if 0
typedef unsigned char       u08;
typedef signed   char       s08;
typedef unsigned short      u16;
typedef signed   short      s16;
typedef unsigned long       u32;
typedef signed   long       s32;
typedef unsigned long long  u64;
typedef signed   long long  s64;
#endif

//typedef unsigned char       bool;
typedef unsigned char       tBoolean;

enum Boolean { False, True };

#ifndef true
#define true ((unsigned char)1)
#endif

#ifndef false
#define false ((unsigned char)0)
#endif

#endif /* TYPES_H_ */
