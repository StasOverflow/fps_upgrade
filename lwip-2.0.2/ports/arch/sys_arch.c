/**
 * @file - sys_arch.c
 * System Architecture support routines for Stellaris devices.
 *
 */

/*
 * Copyright (c) 2001-2003 Swedish Institute of Computer Science.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT
 * SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT
 * OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
 * IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY
 * OF SUCH DAMAGE.
 *
 * This file is part of the lwIP TCP/IP stack.
 *
 * Author: Adam Dunkels <adam@sics.se>
 *
 */

/* Copyright (c) 2012 Impuls ltd. */



/* lwIP includes. */
#include "lwip/opt.h"
#include "lwip/sys.h"
//#include "lwip/timers.h"

#include "arch/sys_arch.h"


#define SYS_SEM_MAX_CNT    1
#define SYS_LWIP_MBOX_SIZE (sizeof(void *))

u32_t
sys_now()
{
  return (u32_t)xTaskGetTickCount();  /* this works on condition that portTICK_RATE_MS is 1 */
}

void
sys_init(void)
{
  //  Is called to initialize the sys_arch layer.
  return;
}

err_t
sys_sem_new(sys_sem_t *sem, u8_t count)
{
 #if 0
  sem->semaphore = xSemaphoreCreateBinary()
		   if (sem->semaphore == PORT_SEM_INVALID)
    return ERR_MEM;
  if (count)
    xSemaphoreGive(sem->semaphore);
 #else
  sem->semaphore = xSemaphoreCreateCounting(SYS_SEM_MAX_CNT, count);
  if (sem->semaphore == PORT_SEM_INVALID)
    return ERR_MEM;
 #endif
  return ERR_OK;
}


void
sys_sem_free(sys_sem_t *sem)
{
#if !CONFIG_LWIP_WITHOUT_FREE
  vSemaphoreDelete(sem->semaphore);
#endif
  sem->semaphore = PORT_SEM_INVALID;
}

void
sys_sem_signal(sys_sem_t *sem)
{
  xSemaphoreGive(sem->semaphore);
}

void
sys_sem_signal_from_ISR(sys_sem_t *sem)
{
  xSemaphoreGiveFromISR(sem->semaphore, NULL);
}

u32_t
sys_arch_sem_wait(sys_sem_t *sem, u32_t timeout)
{
  portTickType start_time = xTaskGetTickCount();
  portTickType wake_time;

  if (timeout == 0) {
    while (xSemaphoreTake(sem->semaphore, portMAX_DELAY) == pdFALSE) ;
  } else {
    portTickType ticks;
    ticks = (timeout + portTICK_RATE_MS - 1) / portTICK_RATE_MS;
    if (ticks == 0)
      ticks = 1;
    if (xSemaphoreTake(sem->semaphore, portMAX_DELAY) == pdFALSE)
      return SYS_ARCH_TIMEOUT;
  }

  wake_time = xTaskGetTickCount();

  return (wake_time - start_time) * portTICK_RATE_MS;
}

int
sys_sem_valid(sys_sem_t *sem)
{
  return sem->semaphore == PORT_SEM_INVALID ? 0 : 1;
}

void
sys_sem_set_invalid(sys_sem_t *sem)
{
  sem->semaphore = PORT_SEM_INVALID;
}

err_t
sys_mbox_new(sys_mbox_t *mbox, int size)
{
  mbox->mailbox = xQueueCreate(size, SYS_LWIP_MBOX_SIZE);
  if (mbox->mailbox == PORT_MBOX_INVALID)
    return ERR_MEM;
  return ERR_OK;
}

void
sys_mbox_free(sys_mbox_t *mbox)
{
#if !CONFIG_LWIP_WITHOUT_FREE
  vQueueDelete(mbox->mailbox);
#endif
  mbox->mailbox = PORT_MBOX_INVALID;
}

void
sys_mbox_post(sys_mbox_t *mbox, void *msg)
{
  while (xQueueSendToBack(mbox->mailbox, &msg, portMAX_DELAY) == errQUEUE_FULL) ;
}

err_t
sys_mbox_trypost(sys_mbox_t *mbox, void *msg)
{
  if (xQueueSendToBack(mbox->mailbox, &msg, 0) == errQUEUE_FULL)
    return ERR_MEM;
  return ERR_OK;
}

u32_t
sys_arch_mbox_fetch(sys_mbox_t *mbox, void **msg, u32_t timeout)
{
  portTickType start_time = xTaskGetTickCount();
  portTickType wake_time;

  if (timeout == 0) {
    while (xQueueReceive(mbox->mailbox, msg, portMAX_DELAY) == pdFALSE) ;
  } else {
    portTickType ticks;
    ticks = (timeout + portTICK_RATE_MS - 1) / portTICK_RATE_MS;
    if (ticks == 0)
      ticks = 1;
    if (xQueueReceive(mbox->mailbox, msg, timeout / portTICK_RATE_MS) == pdFALSE)
      return SYS_ARCH_TIMEOUT;
  }
  wake_time = xTaskGetTickCount();

  return (wake_time - start_time) * portTICK_RATE_MS;
}

u32_t
sys_arch_mbox_tryfetch(sys_mbox_t *mbox, void **msg)
{
  if (xQueueReceive(mbox->mailbox, msg, 0) == pdFALSE)
    return SYS_MBOX_EMPTY;
  return 0; /* we waited 0ms */
}

int
sys_mbox_valid(sys_mbox_t *mbox)
{
  return mbox->mailbox == PORT_MBOX_INVALID ? 0 : 1;
}

void
sys_mbox_set_invalid(sys_mbox_t *mbox)
{
  mbox->mailbox = PORT_MBOX_INVALID;
}

sys_thread_t
sys_thread_new(const char *name, lwip_thread_fn function, void *arg, int stack_size, int prio)
{
  xTaskHandle task_handle;

  if (xTaskCreate(function, name, (unsigned short)stack_size,
		  arg, prio, &task_handle) != pdPASS)
    return NULL;  /* lwIP doesn't specify, how should be informed that thread creation failed */
  return task_handle;
}

err_t
sys_mutex_new(sys_mutex_t *mutex)
{
  mutex->mutex = xSemaphoreCreateMutex();
  if (mutex->mutex == NULL)
    return ERR_MEM;
  return ERR_OK;
}

/** Lock a mutex
 * @param mutex the mutex to lock */
void
sys_mutex_lock(sys_mutex_t *mutex)
{
  while (xSemaphoreTake(mutex->mutex, portMAX_DELAY) == pdFALSE) ;
}

/** Unlock a mutex
 * @param mutex the mutex to unlock */
void
sys_mutex_unlock(sys_mutex_t *mutex)
{
  xSemaphoreGive(mutex->mutex);
}

/** Delete a mutex
 * @param mutex the mutex to delete */
void
sys_mutex_free(sys_mutex_t *mutex)
{
#if !CONFIG_LWIP_WITHOUT_FREE
  vSemaphoreDelete(mutex->mutex);
#endif
  mutex->mutex = PORT_MUTEX_INVALID;
}

void
sys_arch_delay(unsigned int timeout)
{
  portTickType ticks = (timeout + portTICK_RATE_MS - 1) / portTICK_RATE_MS;

  if (ticks == 0)
    ticks = 1;
  vTaskDelay(ticks);
}

/** Ticks/jiffies since power up. */
u32_t
sys_jiffies(void)
{
  return (u32_t)xTaskGetTickCount();
}


