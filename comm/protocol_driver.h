/*
 * protocol_driver.h
 *
 *  Created on: 22 нояб. 2018 г.
 *      Author: V.Striukov
 */

#ifndef COMM_PROTOCOL_DRIVER_H_
#define COMM_PROTOCOL_DRIVER_H_

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

typedef enum {
    Complete_Package_Received,
    Package_Receiving_in_Progress,
} t_package_status;

unsigned char protocol_driver_message_len_get( void );

unsigned char protocol_driver_payload_len_get( void );

unsigned char * protocol_driver_message_data_get( void );
void protocol_driver_proc( void );

void protocol_driver_init( void );

//*****************************************************************************
//
// Mark the end of the C bindings section for C++ compilers.
//
//*****************************************************************************
#ifdef __cplusplus
}
#endif

#endif /* COMM_PROTOCOL_DRIVER_H_ */
