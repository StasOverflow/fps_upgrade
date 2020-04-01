
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __STM32F2xx_CONF_H
#define __STM32F2xx_CONF_H

#include "board.h"

#include <system_stm32f2xx.h>

/* Includes ------------------------------------------------------------------*/
/* Uncomment/Comment the line below to enable/disable peripheral header file inclusion */
#include "stm32f2xx_adc.h"
#include "stm32f2xx_can.h"
#include "stm32f2xx_crc.h"
#include "stm32f2xx_cryp.h"
#include "stm32f2xx_dac.h"
#include "stm32f2xx_dbgmcu.h"
#include "stm32f2xx_dcmi.h"
#include "stm32f2xx_dma.h"
#include "stm32f2xx_exti.h"
#include "stm32f2xx_flash.h"
#include "stm32f2xx_fsmc.h"
#include "stm32f2xx_gpio.h"
#include "stm32f2xx_hash.h"
#include "stm32f2xx_i2c.h"
#include "stm32f2xx_iwdg.h"
#include "stm32f2xx_pwr.h"
#include "stm32f2xx_rcc.h"
#include "stm32f2xx_rng.h"
#include "stm32f2xx_rtc.h"
#include "stm32f2xx_sdio.h"
#include "stm32f2xx_spi.h"
#include "stm32f2xx_syscfg.h"
#include "stm32f2xx_tim.h"
#include "stm32f2xx_usart.h"
#include "stm32f2xx_wwdg.h"
#include "misc.h" /* High level functions for NVIC and SysTick (add-on to CMSIS functions) */


#define PIN_SET(ARGS)                  __SET_PIN(ARGS)
#define PIN_CLR(ARGS)                  __CLR_PIN(ARGS)
#define PIN_TOGGLE(ARGS)               __TOGGLE_PIN(ARGS)
#define PIN_STATE(ARGS)                __PIN_STATE(ARGS)
#define PIN_OUTPUT(ARGS)               __PIN_OUTPUT(ARGS)
#define PIN_INPUT(ARGS)                __PIN_INPUT(ARGS)
#define __SET_PIN(PORT_LETTER, N)      GPIO ## PORT_LETTER->BSRRL = GPIO_Pin_ ## N
#define __CLR_PIN(PORT_LETTER, N)      GPIO ## PORT_LETTER->BSRRH = GPIO_Pin_ ## N
#define __TOGGLE_PIN(PORT_LETTER, N)   GPIO ## PORT_LETTER->ODR ^= GPIO_Pin_ ## N
#define __PIN_STATE(PORT_LETTER, N)    ( GPIO ## PORT_LETTER->IDR & GPIO_Pin_ ## N )
#define __PIN_OUTPUT(PORT_LETTER, N)   GPIO ## PORT_LETTER->MODER &= ~(GPIO_MODER_MODER0 << (N * 2)); \
                                       GPIO ## PORT_LETTER->MODER |=  (0x01UL << (N * 2))
#define __PIN_INPUT(PORT_LETTER, N)    GPIO ## PORT_LETTER->MODER &= ~(GPIO_MODER_MODER0 << (N * 2)); \
                                       GPIO ## PORT_LETTER->MODER |=  (0x00UL << (N * 2))


/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Uncomment the line below to expanse the "assert_param" macro in the 
   Standard Peripheral Library drivers code */
/* #define USE_FULL_ASSERT    1 */

/* Exported macro ------------------------------------------------------------*/
#ifdef  USE_FULL_ASSERT

/**
  * @brief  The assert_param macro is used for function's parameters check.
  * @param  expr: If expr is false, it calls assert_failed function which reports 
  *         the name of the source file and the source line number of the call 
  *         that failed. If expr is true, it returns no value.
  * @retval None
  */
  #define assert_param(expr) ((expr) ? (void)0 : assert_failed((uint8_t *)__FILE__, __LINE__))
/* Exported functions ------------------------------------------------------- */
  void assert_failed(uint8_t* file, uint32_t line);
#else
  #define assert_param(expr) ((void)0)
#endif /* USE_FULL_ASSERT */

#endif /* __STM32F2xx_CONF_H */

