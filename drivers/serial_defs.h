/*
 * serial_defs.h
 *
 *  Created on: 17 февр. 2020 г.
 *      Author: V.Striukov
 */

#ifndef DRIVERS_SERIAL_DEFS_H_
#define DRIVERS_SERIAL_DEFS_H_

/****
 *  C Binding header for C++ compiler
 */
#ifdef __cplusplus
extern "C"
{
#endif


#define SERIAL_BAUDRATE_1200                       ( 1200   )
#define SERIAL_BAUDRATE_2400                       ( 2400   )
#define SERIAL_BAUDRATE_4800                       ( 4800   )
#define SERIAL_BAUDRATE_9600                       ( 9600   )
#define SERIAL_BAUDRATE_19200                      ( 19200  )
#define SERIAL_BAUDRATE_38400                      ( 38400  )
#define SERIAL_BAUDRATE_57600                      ( 57600  )
#define SERIAL_BAUDRATE_115200                     ( 115200 )

typedef enum {
    Baud_1200, Baud_2400, Baud_4800, Baud_9600, Baud_19200,
    Baud_38400, Baud_57600, Baud_115200,

    Baud_Last = Baud_115200,
} baudrate_t;

// In accordance with baudrate_t
static const uint32_t baud_lkp[] =
{
    [Baud_1200  ] = SERIAL_BAUDRATE_1200,
    [Baud_2400  ] = SERIAL_BAUDRATE_2400,
    [Baud_4800  ] = SERIAL_BAUDRATE_4800,
    [Baud_9600  ] = SERIAL_BAUDRATE_9600,
    [Baud_19200 ] = SERIAL_BAUDRATE_19200,
    [Baud_38400 ] = SERIAL_BAUDRATE_38400,
    [Baud_57600 ] = SERIAL_BAUDRATE_57600,
    [Baud_115200] = SERIAL_BAUDRATE_115200,
};

typedef enum {
    Parity_None, Parity_Even, Parity_Odd,

    Parity_Last = Parity_Odd,
} parity_t;

// In accordance with parity_t
static const uint32_t parity_lkp[] =
{
    [Parity_None] = USART_Parity_No,
    [Parity_Even] = USART_Parity_Even,
    [Parity_Odd ] = USART_Parity_Odd,
};

typedef enum {
    StopBits_0_5, StopBits_1, StopBits_1_5, StopBits_2,

    StopBits_Last = StopBits_2,
} stopbits_t;

// In accordance with stopbit_t
static const uint32_t stopbit_lkp[] =
{
    [StopBits_0_5] = USART_StopBits_0_5,
    [StopBits_1  ] = USART_StopBits_1,
    [StopBits_1_5] = USART_StopBits_1_5,
    [StopBits_2  ] = USART_StopBits_2,
};


static const char* const baud_caption[] =
{
    "1200", "2400", "4800", "9600", "19200",
    "38400", "57600", "115200",
};
#define BAUDRATE_MESSAGES_COUNT                 ( sizeof(baud_caption) / sizeof(baud_caption[0]) )

static const char* const parity_caption[] =
{
    "NONE", "EVEN", "ODD",
};
#define PARITY_MESSAGES_COUNT                   ( sizeof(parity_caption) / sizeof(parity_caption[0]) )

static const char* const stopbit_caption[] =
{
    "0.5", "1", "1.5", "2",
};
#define STOPBIT_MESSAGES_COUNT                  ( sizeof(stopbit_caption) / sizeof(stopbit_caption[0]) )

static const char* const databit_caption[] =
{
    "8", "9",
};
#define DATABIT_MESSAGES_COUNT                  ( sizeof(databit_caption) / sizeof(databit_caption[0]) )

/****
 *  Mark the end of the C Binding section for C++ compilers.
 */
#ifdef __cplusplus
}
#endif

#endif /* DRIVERS_SERIAL_DEFS_H_ */
