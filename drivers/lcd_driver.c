/* Scheduler includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"

#include <stm32f2xx.h>
#include <stm32f2xx_conf.h>

#include "lcd_driver.h"
#include "spi.h"
#include "fsmc.h"
#include "sram.h"

#include "timers.h"


#define LCD_R01                     0x01
#define LCD_R02                     0x02
#define LCD_R03                     0x03
#define LCD_R04                     0x04
#define LCD_R05                     0x05
#define LCD_R0A                     0x0A
#define LCD_R0B                     0x0B
#define LCD_R0D                     0x0D
#define LCD_R0E                     0x0E
#define LCD_R0F                     0x0F
#define LCD_R16                     0x16
#define LCD_R17                     0x17
#define LCD_R1E                     0x1E
#define LCD_R30                     0x30
#define LCD_R31                     0x31
#define LCD_R32                     0x32
#define LCD_R33                     0x33
#define LCD_R34                     0x34
#define LCD_R35                     0x35
#define LCD_R36                     0x36
#define LCD_R37                     0x37
#define LCD_R3A                     0x3A
#define LCD_R3B                     0x3B

/* For SPI Data Configuration */
#define LCD_ID                      0x1C
#define LCD_RW                      0x01
#define LCD_RS                      0x02

#define SPI_TFT_CS_HIGH()           GPIO_SetBits(GPIOA, GPIO_Pin_12)
#define SPI_TFT_CS_LOW()            GPIO_ResetBits(GPIOA, GPIO_Pin_12)
#define TFT_RESET_HIGH()            GPIO_SetBits(GPIOA, GPIO_Pin_11)
#define TFT_RESET_LOW()             GPIO_ResetBits(GPIOA, GPIO_Pin_11)

#define LCD_H_SIZE                  ( 320 )
#define LCD_V_SIZE                  ( 240 )

#define LCD_V_FRONT_PORCH           ( 19 )
#define LCD_V_BACK_PORCH            (  4 )
#define LCD_V_BACK_PORCH_START      ( LCD_V_FRONT_PORCH + LCD_V_SIZE )
#define LCD_V_END                   ( LCD_V_BACK_PORCH_START + LCD_V_BACK_PORCH )

#define LCD_H_FRONT_PORCH           ( 68 )
#define LCD_H_BACK_PORCH            ( 20 )

#define LCD_HW_ADDR                 ( 0x64000000UL )
#define LCD_BUFFER_0                ( SRAM_BASE_ADDR )
#define LCD_BUFFER_1                ( LCD_BUFFER_0 + (LCD_H_SIZE * LCD_V_SIZE * 2) )

/* HSYNC and VSYNC */
#define TFT_HSYNC_HIGH()            GPIOC->BSRRL = GPIO_Pin_6
#define TFT_HSYNC_LOW()             GPIOC->BSRRH = GPIO_Pin_6
#define TFT_VSYNC_HIGH()            GPIOA->BSRRL = GPIO_Pin_8
#define TFT_VSYNC_LOW()             GPIOA->BSRRH = GPIO_Pin_8

static unsigned long  ulPixelOffset;
static unsigned long  ulLineCounter;
static unsigned short usDummyPixelData;

static uint32_t     output_buffer = LCD_BUFFER_0;
static uint32_t     reserv_buffer = LCD_BUFFER_1;
volatile unsigned char  frame_loading_done;

static uint16_t     lcd_fps_counter;
static uint16_t     lcd_refresh_rate;

volatile uint16_t     lcd_frames_rendered;
volatile uint16_t     lcd_unique_frames_rendered;

volatile uint8_t        frame_draw_available;

volatile uint8_t        lcd_rendering_done;

static void     lcd_renderer_init( void );


static unsigned short usWidth;
static unsigned short usHeight;
static unsigned short usOrientation;
static          short sHorizontalInc;
static          short sVerticalInc;

static uint16_t       lcd_back_light_state;


extern TaskHandle_t interface_handler;


uint16_t
lcd_refresh_rate_get( void )
{
    return lcd_refresh_rate;
}


uint16_t
lcd_fps_get( void )
{
    return lcd_fps_counter;
}


void DMA2_Stream0_IRQHandler(void)      /* Front porch IT */
{
    TFT_HSYNC_HIGH();

    /*  PAR[31:0]: Peripheral addres
     *
     * Base address of the peripheral data register from/to which the data
     * will be read/written. These bits are write-protected and can be
     * written only when bit EN = '0' in the DMA_SxCR register
     */
    DMA2_Stream1->PAR  = output_buffer + ulPixelOffset;

    DMA2_Stream1->CR  |= DMA_SxCR_EN;  /* Display data stream enable */


    /* DMA low interrupt flag clear register (DMA_LIFCR) */
    DMA2->LIFCR = DMA_FLAG_TCIF0  | /* Clr transfer complete */
                  DMA_FLAG_HTIF0  | /* Clr half transfer */
                  DMA_FLAG_TEIF0  | /* CLr transfer error */
                  DMA_FLAG_DMEIF0 | /* Clr direct mode error */
                  DMA_FLAG_FEIF0;   /* Clr FIFO error */
}


void DMA2_Stream1_IRQHandler(void)      /* Display Data IT */
{
	DMA2_Stream2->CR   |= DMA_SxCR_EN;  /* Back Porch Stream Enable */

	DMA2->LIFCR = DMA_FLAG_TCIF1  |
			      DMA_FLAG_HTIF1  |
			      DMA_FLAG_TEIF1  |
			      DMA_FLAG_DMEIF1 |
			      DMA_FLAG_FEIF1;
}


void DMA2_Stream2_IRQHandler(void)      /* Back porch data */
{
	unsigned long ulTemp;

	ulLineCounter++;

	TFT_HSYNC_LOW();

	if( (ulLineCounter >= LCD_V_FRONT_PORCH) &&
		(ulLineCounter <  LCD_V_BACK_PORCH_START) )
	{
		TFT_VSYNC_HIGH();

		ulPixelOffset += LCD_H_SIZE * 2;

//		frame_draw_available = 0;
	}
	else
	{
//	    frame_draw_available = 1;
	}

	if( ulLineCounter == LCD_V_END )
	{
		TFT_VSYNC_LOW();

		ulLineCounter = 0;
		ulPixelOffset = 0;

		if( frame_loading_done )
		{
			ulTemp        = output_buffer;
			output_buffer = reserv_buffer;
			reserv_buffer = ulTemp;

			frame_loading_done = 0;
			lcd_unique_frames_rendered++;

//			vTaskResume
//	        lcd_rendering_done = 1;
		}

//        PIN_CLR(LED_RED_PIN);
        lcd_frames_rendered++;
	}
	else
	{
	    DMA2_Stream0->CR   |= DMA_SxCR_EN;  /* Front porch stream Enable */
	}

	DMA2->LIFCR = DMA_FLAG_TCIF2  |
			      DMA_FLAG_HTIF2  |
			      DMA_FLAG_TEIF2  |
			      DMA_FLAG_DMEIF2 |
			      DMA_FLAG_FEIF2;

}


static
void __attribute__((naked))
SysCtlDelay(unsigned long ulCount)
{
    __asm("    subs    r0, #1       \n"
          "    bne     SysCtlDelay  \n"
          "    bx      lr           \n");
}


static
void LCD_WriteReg(unsigned char LCD_Reg, unsigned short LCD_RegValue)
{
	/* LCD Select */
	SPI_TFT_CS_LOW();

	/* Send device ID, R/S and RW bits */
	SPI_I2S_SendData(SPI1, LCD_ID << 2);
	while( SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET );

	/* Send the Register Address High Byte */
	SPI_I2S_SendData(SPI1, 0x00);
	while( SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET );

	/* Send the Register Address Low Byte */
	SPI_I2S_SendData(SPI1, LCD_Reg);
	while( SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_BSY) == SET );

	/* LCD Deselect */
	SysCtlDelay(SystemCoreClock / (3 * 10000000)); // 100 ns
	SPI_TFT_CS_HIGH();
	SysCtlDelay(SystemCoreClock / (3 * 10000000)); // 100 ns


	/* LCD Select */
	SPI_TFT_CS_LOW();

	/* Send device ID, R/S and RW bits */
	SPI_I2S_SendData(SPI1, (LCD_ID << 2) | LCD_RS);
	while( SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET ) ;

	/* Send the Command High Byte */
	SPI_I2S_SendData(SPI1, LCD_RegValue >> 8);
	while( SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET ) ;

	/* Send the Command Low Byte */
	SPI_I2S_SendData(SPI1, LCD_RegValue & 0x00FF);
	while( SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_BSY) == SET );

	/* LCD Deselect */
	SysCtlDelay(SystemCoreClock / (3 * 10000000)); // 100 ns
	SPI_TFT_CS_HIGH();
	SysCtlDelay(SystemCoreClock / (3 * 10000000)); // 100 ns
}


static
void LCD_GPIO_Init(void)
{
	GPIO_InitTypeDef  GPIO_InitStruct;

	/* Configure BKL */
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
	GPIO_StructInit(&GPIO_InitStruct);
	GPIO_InitStruct.GPIO_Pin   = GPIO_Pin_9;
	GPIO_InitStruct.GPIO_Mode  = GPIO_Mode_OUT;
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init(GPIOB, &GPIO_InitStruct);

	/* Configure DCS */
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
	GPIO_StructInit(&GPIO_InitStruct);
	GPIO_InitStruct.GPIO_Pin   = GPIO_Pin_12;
	GPIO_InitStruct.GPIO_Mode  = GPIO_Mode_OUT;
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStruct);

	/* Configure DRST */
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
	GPIO_StructInit(&GPIO_InitStruct);
	GPIO_InitStruct.GPIO_Pin   = GPIO_Pin_11;
	GPIO_InitStruct.GPIO_Mode  = GPIO_Mode_OUT;
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStruct);

	/* Configure VSYNC */
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
	GPIO_StructInit(&GPIO_InitStruct);
	GPIO_InitStruct.GPIO_Pin   = GPIO_Pin_8;
	GPIO_InitStruct.GPIO_Mode  = GPIO_Mode_OUT;
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStruct);

	/* Configure HSYNC */
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);
	GPIO_StructInit(&GPIO_InitStruct);
	GPIO_InitStruct.GPIO_Pin   = GPIO_Pin_6;
	GPIO_InitStruct.GPIO_Mode  = GPIO_Mode_OUT;
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOC, &GPIO_InitStruct);
}


static void LCD_Backlight_Init(void)
{
    TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct;
    GPIO_InitTypeDef        GPIO_InitStruct;
    TIM_OCInitTypeDef       TIM_OCInitStruct;

    /* Configure BKL GPIO */
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
    GPIO_StructInit(&GPIO_InitStruct);
    GPIO_InitStruct.GPIO_Pin   = GPIO_Pin_9;
    GPIO_InitStruct.GPIO_Mode  = GPIO_Mode_AF;
    GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_2MHz;
    GPIO_Init(GPIOB, &GPIO_InitStruct);
    GPIO_PinAFConfig(GPIOB, GPIO_PinSource9, GPIO_AF_TIM4);

    /* Configure the timer */
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4 , ENABLE);
    TIM_TimeBaseStructInit(&TIM_TimeBaseInitStruct);
    TIM_TimeBaseInitStruct.TIM_Prescaler = 4;
    TIM_TimeBaseInitStruct.TIM_Period = SystemCoreClock / 200000;
    TIM_TimeBaseInit(TIM4, &TIM_TimeBaseInitStruct);

    /* Configure the timer output compare output */
    TIM_OCStructInit(&TIM_OCInitStruct);
    TIM_OCInitStruct.TIM_OCMode       = TIM_OCMode_PWM1;
    TIM_OCInitStruct.TIM_OutputState  = TIM_OutputState_Disable;
    TIM_OCInitStruct.TIM_OutputNState = TIM_OutputNState_Disable;
    TIM_OCInitStruct.TIM_Pulse        = (SystemCoreClock / 200000) - 23; /* 2us low time */
    TIM_OCInitStruct.TIM_OCPolarity   = TIM_OCPolarity_High;
    TIM_OCInitStruct.TIM_OCNPolarity  = TIM_OCNPolarity_High;
    TIM_OCInitStruct.TIM_OCIdleState  = TIM_OCIdleState_Reset;
    TIM_OCInitStruct.TIM_OCNIdleState = TIM_OCNIdleState_Reset;
    TIM_OC4Init(TIM4, &TIM_OCInitStruct);

    /* Enable the timer */
    TIM_Cmd(TIM4, ENABLE);
}


static
void LCD_DMA_Init(void)
{
	DMA_InitTypeDef  DMA_InitStruct;
	NVIC_InitTypeDef NVIC_InitStructure;

	/* Configure clock for DMA Controller */
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2, ENABLE);

	/* DMA Configuration for line front porch */
	DMA_DeInit(DMA2_Stream0);
	DMA_StructInit(&DMA_InitStruct);
	DMA_InitStruct.DMA_Channel = DMA_Channel_0;
	DMA_InitStruct.DMA_PeripheralBaseAddr = (unsigned long)&usDummyPixelData;
	DMA_InitStruct.DMA_Memory0BaseAddr = LCD_HW_ADDR;
	DMA_InitStruct.DMA_DIR = DMA_DIR_MemoryToMemory;
	DMA_InitStruct.DMA_BufferSize = LCD_H_FRONT_PORCH;
	DMA_InitStruct.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	DMA_InitStruct.DMA_MemoryInc = DMA_MemoryInc_Disable;
	DMA_InitStruct.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
	DMA_InitStruct.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
	DMA_InitStruct.DMA_Mode = DMA_Mode_Normal;
	DMA_InitStruct.DMA_Priority = DMA_Priority_High;
	DMA_InitStruct.DMA_FIFOMode = DMA_FIFOMode_Enable;
	DMA_InitStruct.DMA_FIFOThreshold = DMA_FIFOThreshold_3QuartersFull;
	DMA_InitStruct.DMA_MemoryBurst = DMA_MemoryBurst_Single;
	DMA_InitStruct.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;
	DMA_Init(DMA2_Stream0, &DMA_InitStruct);
	DMA_ITConfig(DMA2_Stream0, DMA_IT_TC, ENABLE);

	/* Enable the DMA2 Stream0 Interrupt */
	NVIC_InitStructure.NVIC_IRQChannel = DMA2_Stream0_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	/* DMA Configuration for display data */
	DMA_DeInit(DMA2_Stream1);
	DMA_StructInit(&DMA_InitStruct);
	DMA_InitStruct.DMA_Channel = DMA_Channel_0;
	DMA_InitStruct.DMA_PeripheralBaseAddr = output_buffer;
	DMA_InitStruct.DMA_Memory0BaseAddr = LCD_HW_ADDR;
	DMA_InitStruct.DMA_DIR = DMA_DIR_MemoryToMemory;
	DMA_InitStruct.DMA_BufferSize = LCD_H_SIZE;
	DMA_InitStruct.DMA_PeripheralInc = DMA_PeripheralInc_Enable;
	DMA_InitStruct.DMA_MemoryInc = DMA_MemoryInc_Disable;
	DMA_InitStruct.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
	DMA_InitStruct.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
	DMA_InitStruct.DMA_Mode = DMA_Mode_Normal;
	DMA_InitStruct.DMA_Priority = DMA_Priority_High;
	DMA_InitStruct.DMA_FIFOMode = DMA_FIFOMode_Enable;
	DMA_InitStruct.DMA_FIFOThreshold = DMA_FIFOThreshold_3QuartersFull;
	DMA_InitStruct.DMA_MemoryBurst = DMA_MemoryBurst_Single;
	DMA_InitStruct.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;
	DMA_Init(DMA2_Stream1, &DMA_InitStruct);
	DMA_ITConfig(DMA2_Stream1, DMA_IT_TC, ENABLE);

	/* Enable the DMA2 Stream1 Interrupt */
	NVIC_InitStructure.NVIC_IRQChannel =  DMA2_Stream1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	/* DMA Configuration for line back porch */
	DMA_DeInit(DMA2_Stream2);
	DMA_StructInit(&DMA_InitStruct);
	DMA_InitStruct.DMA_Channel = DMA_Channel_0;
	DMA_InitStruct.DMA_PeripheralBaseAddr = (unsigned long)&usDummyPixelData;
	DMA_InitStruct.DMA_Memory0BaseAddr = LCD_HW_ADDR;
	DMA_InitStruct.DMA_DIR = DMA_DIR_MemoryToMemory;
	DMA_InitStruct.DMA_BufferSize = LCD_H_BACK_PORCH;
	DMA_InitStruct.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	DMA_InitStruct.DMA_MemoryInc = DMA_MemoryInc_Disable;
	DMA_InitStruct.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
	DMA_InitStruct.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
	DMA_InitStruct.DMA_Mode = DMA_Mode_Normal;
	DMA_InitStruct.DMA_Priority = DMA_Priority_High;
	DMA_InitStruct.DMA_FIFOMode = DMA_FIFOMode_Enable;
	DMA_InitStruct.DMA_FIFOThreshold = DMA_FIFOThreshold_3QuartersFull;
	DMA_InitStruct.DMA_MemoryBurst = DMA_MemoryBurst_Single;
	DMA_InitStruct.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;
	DMA_Init(DMA2_Stream2, &DMA_InitStruct);
	DMA_ITConfig(DMA2_Stream2, DMA_IT_TC, ENABLE);

	/* Enable the DMA2 Stream2 Interrupt */
	NVIC_InitStructure.NVIC_IRQChannel =  DMA2_Stream2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}


static
void LCD_Enable(void)
{
    TFT_VSYNC_LOW();
    TFT_HSYNC_LOW();

    lcd_renderer_init();
    DMA_Cmd(DMA2_Stream0, ENABLE);   /* Start the FRONT PORCH stream */
}


static
void LCD_Disable(void)
{
	TFT_VSYNC_LOW();
	TFT_HSYNC_LOW();

	DMA_Cmd(DMA2_Stream0, DISABLE);
	DMA_Cmd(DMA2_Stream1, DISABLE);
	DMA_Cmd(DMA2_Stream2, DISABLE);
}


static
void LCD_BacklightStateSet(unsigned short state)
{
    lcd_back_light_state = state;
    TIM_CCxCmd(TIM4, TIM_Channel_4, (lcd_back_light_state) ? TIM_CCx_Enable : TIM_CCx_Disable);
}


static
unsigned short LCD_BacklightStateGet(void)
{
    return lcd_back_light_state;
}


static
void LCD_OrientationSet(unsigned short usOrient)
{
	switch( usOrient )
	{
	case DISPLAY_ORIENTATION_LANDSCAPE:
		sHorizontalInc = 1;
		sVerticalInc   = LCD_H_SIZE;
		usWidth  = LCD_H_SIZE;
		usHeight = LCD_V_SIZE;
		break;

	case DISPLAY_ORIENTATION_LANDSCAPE_FLIP:
		sHorizontalInc = -1;
		sVerticalInc   = -LCD_H_SIZE;
		usWidth  = LCD_H_SIZE;
		usHeight = LCD_V_SIZE;
		break;

	case DISPLAY_ORIENTATION_PORTRAIT:
		sHorizontalInc = -LCD_H_SIZE;
		sVerticalInc   = 1;
		usWidth  = LCD_V_SIZE;
		usHeight = LCD_H_SIZE;
		break;

	case DISPLAY_ORIENTATION_PORTRAIT_FLIP:
		sHorizontalInc = LCD_H_SIZE;
		sVerticalInc   = -1;
		usWidth  = LCD_V_SIZE;
		usHeight = LCD_H_SIZE;
		break;
	}

	usOrientation = usOrient;
}


static
unsigned short LCD_OrientationGet(void)
{
	return usOrientation;
}


static
unsigned short LCD_WidthGet(void)
{
	return usWidth;
}


static
unsigned short LCD_HeightGet(void)
{
	return usHeight;
}


void TIM7_IRQHandler( void )
{
    TIM_ClearITPendingBit(TIM7, TIM_IT_Update);

    lcd_fps_counter = lcd_frames_rendered;
    lcd_refresh_rate = lcd_unique_frames_rendered;

    lcd_frames_rendered = 0;
    lcd_unique_frames_rendered = 0;
}


static void
lcd_fps_tracker_init( void )
{
    TIM_TimeBaseInitTypeDef  timer;
    NVIC_InitTypeDef  nvic;

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM7, ENABLE);

    //
    // Timer Base configuration
    //
    timer.TIM_Prescaler         = 59999;
    timer.TIM_CounterMode       = TIM_CounterMode_Up;
    timer.TIM_Period            = 1000;
    timer.TIM_RepetitionCounter = 0;
    TIM_TimeBaseInit(TIM7, &timer);

    nvic.NVIC_IRQChannel = TIM7_IRQn;
    nvic.NVIC_IRQChannelPreemptionPriority = 2;
    nvic.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&nvic);

    //
    // TIM5 enable
    //
    TIM_Cmd(TIM7, ENABLE);

    TIM_ITConfig(TIM7, TIM_IT_Update, ENABLE);

}

static uint16_t     tim_val = 200;

void tim_val_plus( uint8_t value )
{
    tim_val += value;
}

void tim_val_minus( uint8_t value )
{
    tim_val -= value;
}

void TIM2_IRQHandler( void )
{
    TIM2->ARR = tim_val;
    TIM2->CNT = 0;

    TIM_ClearITPendingBit(TIM2, TIM_IT_Update);

    if( ulLineCounter == 0 )
    {
//        vTaskSuspend(interface_handler);
//        PIN_SET(LED_RED_PIN);
//        if( frame_loading_done == 0)
//        if( lcd_rendering_done )
        {
//            lcd_rendering_done = 0;
        }

//        frame_loading_done = 0;
    }

//    TIM2->CNT = 0;
}


static void
lcd_renderer_init( void )
{
//    TIM_TimeBaseInitTypeDef  timer;
//    NVIC_InitTypeDef  nvic;
//
//    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
//
//    // Timer Base configuration
//    timer.TIM_Prescaler         = 5999;
//    timer.TIM_CounterMode       = TIM_CounterMode_Up;
//    timer.TIM_Period            = 100;
//    timer.TIM_RepetitionCounter = 0;
//    TIM_TimeBaseInit(TIM2, &timer);

//    TIM_ARRPreloadConfig(TIM2, 1);
//    TIM2->ARR = 40;
//    TIM2->EGR = TIM_PSCReloadMode_Immediate;

//    nvic.NVIC_IRQChannel = TIM2_IRQn;
//    nvic.NVIC_IRQChannelPreemptionPriority = 2;
//    nvic.NVIC_IRQChannelCmd = ENABLE;
//    NVIC_Init(&nvic);
//
//    TIM_Cmd(TIM2, ENABLE);
//    TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);
}


void  LCD_Init(void)
{
	/* Configure the LCD GPIO pins */
	LCD_GPIO_Init();

    /* Configure the LCD Backlight */
    LCD_Backlight_Init();

	/* Configure the SPI interface */
	LCD_SPI_Init();

	/* Deselect LCD */
	SPI_TFT_CS_HIGH();

	/* Reset LCD */
	TFT_RESET_LOW();
	SysCtlDelay(SystemCoreClock / (3 * 1000));

	TFT_RESET_HIGH();
	SysCtlDelay(SystemCoreClock / (3 * 1000));

	/* Configure LCD registers */
	LCD_WriteReg(LCD_R01, 0x6300); // Driver Output Control
	LCD_WriteReg(LCD_R02, 0x0200); // LCD-Driving-Waveform Control
	LCD_WriteReg(LCD_R03, 0x7166); // Power Control
	LCD_WriteReg(LCD_R04, 0x0447); // Input Data and Color Filter Control
	LCD_WriteReg(LCD_R05, 0xB4D7); // Function Control
	LCD_WriteReg(LCD_R0A, 0x2908); // Contrast/Brightness Control
	LCD_WriteReg(LCD_R0B, 0xD400); // Frame Cycle Control

	/* Gamma */
//	LCD_WriteReg(LCD_R30, 0x0000);
//	LCD_WriteReg(LCD_R31, 0x0400);
//	LCD_WriteReg(LCD_R32, 0x0106);
//	LCD_WriteReg(LCD_R33, 0x0700);
//	LCD_WriteReg(LCD_R34, 0x0002);
//	LCD_WriteReg(LCD_R35, 0x0702);
//	LCD_WriteReg(LCD_R36, 0x0707);
//	LCD_WriteReg(LCD_R37, 0x0203);
//	LCD_WriteReg(LCD_R3A, 0x1400);
//	LCD_WriteReg(LCD_R3B, 0x0F03);

	/* Configure the FSMC interface */
	FSMC_LCD_Init();

	/* Configure DMA to transfer data from SRAM to TFT-LCD */
	LCD_DMA_Init();

	/* Set the default display orientation */
	LCD_OrientationSet(DISPLAY_ORIENTATION_LANDSCAPE);

	lcd_fps_tracker_init();
}
//*****************************************************************************
//
// Remap X coordinate.
//
// \param lX is the X coordinate.
// \param lY is the Y coordinate.
//
// This function remaps X coordinate for various display orientation.
//
// \return Returns remaped X coordinate.
//
//*****************************************************************************
static
long LCD_RemapX(long lX, long lY)
{
	long lMappedX = 0;

	switch( usOrientation )
	{
	case DISPLAY_ORIENTATION_LANDSCAPE:
		lMappedX = lX;
		break;

	case DISPLAY_ORIENTATION_LANDSCAPE_FLIP:
		lMappedX = 319 - lX;
		break;

	case DISPLAY_ORIENTATION_PORTRAIT:
		lMappedX = lY;
		break;

	case DISPLAY_ORIENTATION_PORTRAIT_FLIP:
		lMappedX = 319 - lY;
		break;
	}

	return lMappedX;
}


//*****************************************************************************
//
// Remap Y coordinate.
//
// \param lX is the X coordinate.
// \param lY is the Y coordinate.
//
// This function remaps Y coordinate for various display orientation.
//
// \return Returns remaped Y coordinate.
//
//*****************************************************************************
static
long LCD_RemapY(long lX, long lY)
{
	long lMappedY = 0;

	switch( usOrientation )
	{
	case DISPLAY_ORIENTATION_LANDSCAPE:
		lMappedY = lY;
		break;

	case DISPLAY_ORIENTATION_LANDSCAPE_FLIP:
		lMappedY = 239 - lY;
		break;

	case DISPLAY_ORIENTATION_PORTRAIT:
		lMappedY = 239 - lX;
		break;

	case DISPLAY_ORIENTATION_PORTRAIT_FLIP:
		lMappedY = lX;
		break;
	}

	return lMappedY;
}


//*****************************************************************************
//
// Translates a 24-bit RGB color to a display driver-specific color.
//
// \param c is the 24-bit RGB color.  The least-significant byte is the blue
// channel, the next byte is the green channel, and the third byte is the red
// channel.
//
// This macro translates a 24-bit RGB color into a value that can be written
// into the display's frame buffer in order to reproduce that color, or the
// closest possible approximation of that color.
//
// \return Returns the display-driver specific color.
//
//*****************************************************************************
#define DPYCOLORTRANSLATE(c)    ((((c) & 0x00f80000) >> 8) |               \
                                 (((c) & 0x0000fc00) >> 5) |               \
                                 (((c) & 0x000000f8) >> 3))

//*****************************************************************************
//
//! Draws a pixel on the screen.
//!
//! \param lX is the X coordinate of the pixel.
//! \param lY is the Y coordinate of the pixel.
//! \param ulValue is the color of the pixel.
//!
//! This function sets the given pixel to a particular color.  The coordinates
//! of the pixel are assumed to be within the extents of the display.
//!
//! \return None.
//
//*****************************************************************************
static void
LCD_PixelDraw(long lX, long lY, unsigned long ulValue)
{
	unsigned short *pusData = (unsigned short *)reserv_buffer;

    //
    // Set the Y address of the display cursor.
    //
	pusData += LCD_RemapY(lX, lY) * LCD_H_SIZE;

    //
    // Set the X address of the display cursor.
    //
	pusData += LCD_RemapX(lX, lY);

    //
    // Write the pixel value.
    //
	*pusData = (unsigned short)ulValue;
}

//*****************************************************************************
//
//! Draws a horizontal sequence of pixels on the screen.
//!
//! \param lX is the X coordinate of the first pixel.
//! \param lY is the Y coordinate of the first pixel.
//! \param lX0 is sub-pixel offset within the pixel data, which is valid for 1
//! or 4 bit per pixel formats.
//! \param lCount is the number of pixels to draw.
//! \param lBPP is the number of bits per pixel; must be 1, 4, or 8.
//! \param pucData is a pointer to the pixel data.  For 1 and 4 bit per pixel
//! formats, the most significant bit(s) represent the left-most pixel.
//! \param pucPalette is a pointer to the palette used to draw the pixels.
//!
//! This function draws a horizontal sequence of pixels on the screen, using
//! the supplied palette.  For 1 bit per pixel format, the palette contains
//! pre-translated colors; for 4 and 8 bit per pixel formats, the palette
//! contains 24-bit RGB values that must be translated before being written to
//! the display.
//!
//! \return None.
//
//*****************************************************************************
static void
LCD_PixelDrawMultiple(long lX, long lY, long lX0, long lCount, long lBPP,
		              const unsigned char *pucData, const unsigned char *pucPalette)
{
	unsigned short *pusData = (unsigned short *)reserv_buffer;
    unsigned long   ulByte;

    //
    // Set the Y address of the display cursor.
    //
	pusData += LCD_RemapY(lX, lY) * LCD_H_SIZE;

    //
    // Set the X address of the display cursor.
    //
	pusData += LCD_RemapX(lX, lY);

    //
    // Determine how to interpret the pixel data based on the number of bits
    // per pixel.
    //
    switch(lBPP)
    {
        //
        // The pixel data is in 1 bit per pixel format.
        //
        case 1:
        {
            //
            // Loop while there are more pixels to draw.
            //
            while(lCount)
            {
                //
                // Get the next byte of image data.
                //
                ulByte = *pucData++;

                //
                // Loop through the pixels in this byte of image data.
                //
                for(; (lX0 < 8) && lCount; lX0++, lCount--)
                {
                    //
                    // Draw this pixel in the appropriate color.
                    //
                	*pusData = (unsigned short)((unsigned long *)pucPalette)[(ulByte >> (7 - lX0)) & 1];

            		//
            		// Move the data pointer.
            		//
            		pusData += sHorizontalInc;
                }

                //
                // Start at the beginning of the next byte of image data.
                //
                lX0 = 0;
            }

            //
            // The image data has been drawn.
            //
            break;
        }

        //
        // The pixel data is in 4 bit per pixel format.
        //
        case 4:
        {
            //
            // Loop while there are more pixels to draw.  "Duff's device" is
            // used to jump into the middle of the loop if the first nibble of
            // the pixel data should not be used.  Duff's device makes use of
            // the fact that a case statement is legal anywhere within a
            // sub-block of a switch statement.  See
            // http://en.wikipedia.org/wiki/Duff's_device for detailed
            // information about Duff's device.
            //
            switch(lX0 & 1)
            {
                case 0:
                    while(lCount)
                    {
                        //
                        // Get the upper nibble of the next byte of pixel data
                        // and extract the corresponding entry from the
                        // palette.
                        //
                        ulByte = (*pucData >> 4) * 3;
                        ulByte = (*(unsigned long *)(pucPalette + ulByte) &
                                  0x00ffffff);

                        //
                        // Translate this palette entry and write it to the
                        // screen.
                        //
                        *pusData = (unsigned short)DPYCOLORTRANSLATE(ulByte);

                		//
                		// Move the data pointer.
                		//
                		pusData += sHorizontalInc;

                        //
                        // Decrement the count of pixels to draw.
                        //
                        lCount--;

                        //
                        // See if there is another pixel to draw.
                        //
                        if(lCount)
                        {
                case 1:
                            //
                            // Get the lower nibble of the next byte of pixel
                            // data and extract the corresponding entry from
                            // the palette.
                            //
                            ulByte = (*pucData++ & 15) * 3;
                            ulByte = (*(unsigned long *)(pucPalette + ulByte) &
                                      0x00ffffff);

                            //
                            // Translate this palette entry and write it to the
                            // screen.
                            //
                            *pusData = (unsigned short)DPYCOLORTRANSLATE(ulByte);

                    		//
                    		// Move the data pointer.
                    		//
                    		pusData += sHorizontalInc;

                            //
                            // Decrement the count of pixels to draw.
                            //
                            lCount--;
                        }
                    }
            }

            //
            // The image data has been drawn.
            //
            break;
        }

        //
        // The pixel data is in 8 bit per pixel format.
        //
        case 8:
        {
            //
            // Loop while there are more pixels to draw.
            //
            while(lCount--)
            {
                //
                // Get the next byte of pixel data and extract the
                // corresponding entry from the palette.
                //
                ulByte = *pucData++ * 3;
                ulByte = *(unsigned long *)(pucPalette + ulByte) & 0x00ffffff;

                //
                // Translate this palette entry and write it to the screen.
                //
                *pusData = (unsigned short)DPYCOLORTRANSLATE(ulByte);

        		//
        		// Move the data pointer.
        		//
        		pusData += sHorizontalInc;
            }

            //
            // The image data has been drawn.
            //
            break;
        }

        //
        // We are being passed data in the display's native format.  Merely
        // write it directly to the display.  This is a special case which is
        // not used by the graphics library but which is helpful to
        // applications which may want to handle, for example, JPEG images.
        //
        case 16:
        {
            unsigned short usByte;

            //
            // Loop while there are more pixels to draw.
            //
            while(lCount--)
            {
                //
                // Get the next byte of pixel data and extract the
                // corresponding entry from the palette.
                //
                usByte = *((unsigned short *)pucData);
                pucData += 2;

                //
                // Translate this palette entry and write it to the screen.
                //
                *pusData = usByte;

        		//
        		// Move the data pointer.
        		//
        		pusData += sHorizontalInc;
            }
        }
    }
}

//*****************************************************************************
//
//! Draws a horizontal line.
//!
//! \param lX1 is the X coordinate of the start of the line.
//! \param lX2 is the X coordinate of the end of the line.
//! \param lY is the Y coordinate of the line.
//! \param ulValue is the color of the line.
//!
//! This function draws a horizontal line on the display.  The coordinates of
//! the line are assumed to be within the extents of the display.
//!
//! \return None.
//
//*****************************************************************************
static void
LCD_LineDrawH(long lX1, long lX2, long lY, unsigned long ulValue)
{
	unsigned short *pusData = (unsigned short *)reserv_buffer;

    //
    // Set the Y address of the display cursor.
    //
	pusData += LCD_RemapY(lX1, lY) * LCD_H_SIZE;

    //
    // Set the X address of the display cursor.
    //
	pusData += LCD_RemapX(lX1, lY);

    //
    // Loop through the pixels of this horizontal line.
    //
    while(lX1++ <= lX2)
    {
		//
		// Write the pixel value.
		//
		*pusData = (unsigned short)ulValue;

		//
		// Move the data pointer.
		//
		pusData += sHorizontalInc;
    }
}

//*****************************************************************************
//
//! Draws a vertical line.
//!
//! \param lX is the X coordinate of the line.
//! \param lY1 is the Y coordinate of the start of the line.
//! \param lY2 is the Y coordinate of the end of the line.
//! \param ulValue is the color of the line.
//!
//! This function draws a vertical line on the display.  The coordinates of the
//! line are assumed to be within the extents of the display.
//!
//! \return None.
//
//*****************************************************************************
static void
LCD_LineDrawV(long lX, long lY1, long lY2, unsigned long ulValue)
{
	unsigned short *pusData = (unsigned short *)reserv_buffer;

    //
    // Set the Y address of the display cursor.
    //
	pusData += LCD_RemapY(lX, lY1) * LCD_H_SIZE;

    //
    // Set the X address of the display cursor.
    //
	pusData += LCD_RemapX(lX, lY1);

    //
    // Loop through the pixels of this vertical line.
    //
    while(lY1++ <= lY2)
    {
        //
		// Write the pixel value.
		//
		*pusData  = (unsigned short)ulValue;

		//
		// Move the data pointer.
		//
		pusData += sVerticalInc;
    }
}

//*****************************************************************************
//
//! Fills a rectangle.
//!
//! \param pRect is a pointer to the rectangle to be filled.
//! \param ulValue is the color of the rectangle.
//!
//! This function fills a rectangle on the display.  The coordinates of the
//! rectangle are assumed to be within the extents of the display, and the
//! rectangle specification is fully inclusive (in other words, both sXMin and
//! sXMax are drawn, along with sYMin and sYMax).
//!
//! \return None.
//
//*****************************************************************************
static void
LCD_RectFill(tRectangle *pRect, unsigned long ulValue)
{
	unsigned short *pusData = (unsigned short *)reserv_buffer;
	short           sXMin = pRect->sXMin;
	short           sYMin = pRect->sYMin;
	short           sXMax = pRect->sXMax;
	short           sYMax = pRect->sYMax;
	long            lX = LCD_RemapX(sXMin, sYMin);
	long            lY = LCD_RemapY(sXMin, sYMin);
	long            lOffset;

    //
    // Set the Y address of the display cursor.
    //
	pusData += lY * LCD_H_SIZE;

	for(long lY = sYMin; lY <= sYMax; lY++)
	{
		lOffset = lX;

		for(long lX = sXMin; lX <= sXMax; lX++)
		{
			*(pusData + lOffset) = (unsigned short)ulValue;

			lOffset += sHorizontalInc;
		}

		//
		// Move the data pointer.
		//
		pusData += sVerticalInc;
	}
}

//*****************************************************************************
//
//! Translates a 24-bit RGB color to a display driver-specific color.
//!
//! \param ulValue is the 24-bit RGB color.  The least-significant byte is the
//! blue channel, the next byte is the green channel, and the third byte is the
//! red channel.
//!
//! This function translates a 24-bit RGB color into a value that can be
//! written into the display's frame buffer in order to reproduce that color,
//! or the closest possible approximation of that color.
//!
//! \return Returns the display-driver specific color.
//
//*****************************************************************************
static unsigned long
LCD_ColorTranslate(unsigned long ulValue)
{
    //
    // Translate from a 24-bit RGB color to a 5-6-5 RGB color.
    //
    return(DPYCOLORTRANSLATE(ulValue));
}

//*****************************************************************************
//
//! Flushes rectangle area to the display.
//!
//! \param pRect is a pointer to the rectangle to be flushed.
//!
//! This functions flushes rectangle area to the display.  This is useful when
//! a local frame buffer is used for drawing operations, and the flush would
//! copy the local frame buffer to the display.
//!
//! \return None.
//
//*****************************************************************************
static void
LCD_Flush(tRectangle *pRect)
{

}

//*****************************************************************************
//
//! The display structure that describes the driver for the Sharp
//! LQ035 TFT panel with an HX8238 controller.
//
//*****************************************************************************
tDisplay g_sDisplay =
{
	LCD_Enable,
	LCD_Disable,
	LCD_BacklightStateSet,
	LCD_BacklightStateGet,
	LCD_OrientationSet,
	LCD_OrientationGet,
	LCD_WidthGet,
	LCD_HeightGet,
    LCD_PixelDraw,
    LCD_PixelDrawMultiple,
    LCD_LineDrawH,
    LCD_LineDrawV,
    LCD_RectFill,
    LCD_ColorTranslate,
    LCD_Flush
};
