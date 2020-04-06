/*
	FreeRTOS V8.0.1 - Copyright (C) 2014 Real Time Engineers Ltd.
	All rights reserved
*/

#ifndef FREERTOS_CONFIG_H
#define FREERTOS_CONFIG_H


#define configENABLE_BACKWARD_COMPATIBILITY      1

#define configUSE_PREEMPTION                     1
#define configUSE_IDLE_HOOK                      0
#define configUSE_TICK_HOOK                      0
#define configCPU_CLOCK_HZ                       ( ( unsigned long ) 120000000 )
#define configTICK_RATE_HZ                       ( ( TickType_t ) 1000 )
#define configMINIMAL_STACK_SIZE                 ( ( unsigned short ) 256 )
#define configTOTAL_HEAP_SIZE                    ( ( size_t ) ( 60000 ) )
#define configMAX_TASK_NAME_LEN                  ( 16 )
#define configUSE_TRACE_FACILITY                 1
#define configUSE_16_BIT_TICKS                   0
#define configIDLE_SHOULD_YIELD                  0
#define configUSE_TASK_NOTIFICATIONS             1
#define configUSE_CO_ROUTINES                    0
#define configUSE_MUTEXES                        1
#define configUSE_COUNTING_SEMAPHORES            1
#define configCHECK_FOR_STACK_OVERFLOW           0
#define configUSE_RECURSIVE_MUTEXES              1
//#define configQUEUE_REGISTRY_SIZE                10

#define configUSE_TIMERS                         1
#define configTIMER_TASK_PRIORITY                3
#define configTIMER_QUEUE_LENGTH                 10
#define configTIMER_TASK_STACK_DEPTH             configMINIMAL_STACK_SIZE

/* Set the following definitions to 1 to include the API function, or zero
to exclude the API function. */

#define configMAX_PRIORITIES            ( 5 )
#define configMAX_CO_ROUTINE_PRIORITIES ( 2 )

/* Set the following definitions to 1 to include the API function, or zero
to exclude the API function. */

#define INCLUDE_vTaskPrioritySet            1
#define INCLUDE_uxTaskPriorityGet           1
#define INCLUDE_vTaskDelete                 1
#define INCLUDE_vTaskCleanUpResources       0
#define INCLUDE_vTaskSuspend                1
#define INCLUDE_vTaskDelayUntil             1
#define INCLUDE_vTaskDelay                  1
#define INCLUDE_uxTaskGetStackHighWaterMark 1
#define INCLUDE_eTaskGetState               1

/* This demo makes use of one or more example stats formatting functions.  These
format the raw data provided by the uxTaskGetSystemState() function in to human
readable ASCII form.  See the notes in the implementation of vTaskList() within
FreeRTOS/Source/tasks.c for limitations. */
#define configUSE_STATS_FORMATTING_FUNCTIONS     0
#define configGENERATE_RUN_TIME_STATS            0


#define configKERNEL_INTERRUPT_PRIORITY         ( ( unsigned char ) 15 << ( unsigned char ) 4 )
#define configMAX_SYSCALL_INTERRUPT_PRIORITY    ( ( unsigned char )  7 << ( unsigned char ) 4 )

#endif /* FREERTOS_CONFIG_H */
