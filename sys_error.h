/*
 * sys_error.h
 *
 *  Created on: May 27, 2015
 *      Author: i.otrokha
 */

#ifndef SYS_ERROR_H_
#define SYS_ERROR_H_


enum LedsErrorState
{
	StatusLed_Ok = 0,
};

typedef unsigned long sys_msg_t;

extern QueueHandle_t      xGrnLedQueue;


#endif /* SYS_ERROR_H_ */
