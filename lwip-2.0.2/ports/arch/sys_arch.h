
#ifndef __ARCH_SYS_ARCH_H__
#define __ARCH_SYS_ARCH_H__

/* Scheduler includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"

/* Typedefs for the various port-specific types. */
#if defined(NO_SYS) && NO_SYS
  #error "FreeRTOS SYS_ARCH cannot be compiled in NO_SYS variant"
#endif

typedef unsigned int sys_prot_t;

/* The values for an unallocated entities. */
#define PORT_SEM_INVALID        NULL
#define PORT_MBOX_INVALID       NULL
#define PORT_THREAD_INVALID     NULL
#define PORT_MUTEX_INVALID      NULL

typedef struct {
  xQueueHandle mailbox;
} port_mailbox_t;

typedef struct {
  xSemaphoreHandle semaphore;
} port_sem_t;

typedef struct {
  xSemaphoreHandle mutex;
} port_mutex_t;

typedef port_mailbox_t sys_mbox_t;
typedef port_sem_t sys_sem_t;
typedef port_mutex_t sys_mutex_t;
typedef xTaskHandle sys_thread_t;

void
sys_arch_delay(unsigned int x);
void
sys_sem_signal_from_ISR(sys_sem_t *sem);


static inline sys_prot_t
sys_arch_protect(void)
{
	taskENTER_CRITICAL();

	return 0;
}

static inline void
sys_arch_unprotect(sys_prot_t pval)
{
	(void) pval;

	taskEXIT_CRITICAL();
}


#endif /* __ARCH_SYS_ARCH_H__ */
