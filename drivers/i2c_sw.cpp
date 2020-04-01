/*
 * i2c_sw.c
 *
 *  Created on: 24 февр. 2020 г.
 *      Author: V.Striukov
 */
#include "i2c_sw.h"

#define              is_even(x)         (x & 1 ? 0 : 1)

typedef enum {
    Bus_Idle,
    Msg_Prepare,
    Start_Repeat,
    Start,
    Cmd_Send,           // TODO: wrap into a single Data_Handle
    Data_Send,
    Data_Read,
    Sequence_handle,        // XXX: until here
    Stop,
    Done,
    Bus_Force_Stop,
} i2c_bus_st;


//static enum {
//    Stage_Init,
//    Stage_I2C_Idle,
//    Stage_I2C_Comm,
//    Stage_Done,
//} i2c_proc_stage;


static uint8_t        i2c_ffff[2] = {0xff, 0xff};
static uint8_t        i2c_module_init;

static uint8_t        i2c_data_tx[I2C_BUF_SIZE];
static uint8_t        i2c_data_rx[I2C_BUF_SIZE];
static uint8_t        i2c_data_comm_seq_size;
static uint8_t        i2c_data_rx_size;

static uint32_t       i2c_seq_ending[SEQ_SEPARATORS];
static i2c_bus_st     i2c_bus_state;
static i2c_err_t      i2c_error;

//static uint8_t        i2c_data_lock;



i2c_bus_st
i2c_bus_state_get( void )
{
    i2c_bus_st      state;

    I2C_ENTER_CRITICAL();
    state = i2c_bus_state;
    I2C_EXIT_CRITICAL();

    return state;
}


void
i2c_sw_bus_stop( void )
{
    I2C_ENTER_CRITICAL();
    i2c_bus_state = Bus_Force_Stop;
    I2C_EXIT_CRITICAL();
}


void
i2c_sw_bus_resume( void )
{
    I2C_ENTER_CRITICAL();
    if( i2c_bus_state == Bus_Force_Stop)
    {
        i2c_bus_state = Bus_Idle;
    }
    I2C_EXIT_CRITICAL();
}


#include "app_data.h"

static inline uint8_t
i2c_msg_add( uint16_t addr, uint8_t* data, uint8_t size, uint8_t rw )
{
//    uint8_t       success = 0;      // TODO: Handle failure scenario
//    uint8_t       addr_size = ADRR_7_BIT_SIZE;   // TODO: Add 10-bit
//    uint8_t       seq_end_pos;


//    if( !i2c_sw_busy() )
//    {
//        if( (i2c_data_comm_seq_size + size + addr_size) <= I2C_BUF_SIZE )
//        {
//            i2c_data_tx[i2c_data_comm_seq_size] = (addr << 1) | rw;
//            i2c_data_comm_seq_size++;
//
//            memcpy(&i2c_data_tx[i2c_data_comm_seq_size], data, size);
//            i2c_data_comm_seq_size += size;
//
//            /*
//             * detecting seuquence ending will result in either
//             * repeated start or stop condition.
//             *
//             * Data Size is a POSITION of the last message, to
//             * set a certain bit at X position, we should left
//             * shift 1 for (X-1) digits
//             */
//            seq_end_pos = i2c_data_comm_seq_size - 1;
//            i2c_seq_ending[seq_end_pos >> 5] |= ( 1 << (seq_end_pos) );
//            success = 1;
//        }
//    }

    return 1;
}


/* Blocking functions */
//uint8_t
//i2c_sw_write( uint16_t addr, uint8_t reg, uint8_t* data, uint8_t size )
//{
//    uint8_t       success = 0;      // TODO: Handle failure scenario
//    uint8_t       addr_size = ADRR_7_BIT_SIZE;   // TODO: Add 10-bit
//    uint8_t       seq_end_pos;
//
//    if( (size + addr_size) <= I2C_BUF_SIZE )
//    {
//        i2c_data_tx[0] = (addr << 1) | rw;
//        i2c_data_comm_seq_size++;
//
//        memcpy(&i2c_data_tx[i2c_data_comm_seq_size], data, size);
//        i2c_data_comm_seq_size += size;
//
//        /*
//         * detecting seuquence ending will result in either
//         * repeated start or stop condition.
//         *
//         * Data Size is a POSITION of the last message, to
//         * set a certain bit at X position, we should left
//         * shift 1 for (X-1) digits
//         */
//        seq_end_pos = i2c_data_comm_seq_size - 1;
//        i2c_seq_ending[seq_end_pos >> 5] |= ( 1 << (seq_end_pos) );
//    }
//    else
//    {
//    }
//
//    return i2c_msg_add( addr, data, size, I2C_RW_WRITE);
//}


//uint8_t
//i2c_sw_read_reg( uint16_t addr, uint8_t reg, uint8_t* data, uint8_t size )
//{
//
//}



void
i2c_error_clr( void )
{
    i2c_error = I2C_ERR_NONE;
}


i2c_err_t
i2c_error_get( void )
{
    return i2c_error;
}


void
i2c_sw_flush( void )
{
    memset(i2c_data_tx, 0, I2C_BUF_SIZE);
    memset(i2c_data_rx, 0, I2C_BUF_SIZE);
    memset(i2c_seq_ending, 0, sizeof(i2c_seq_ending));
    i2c_data_comm_seq_size = 0;
    i2c_data_rx_size = 0;

    i2c_error_clr();
}


static inline void
i2c_sw_rst_or_stop_mark_add( uint8_t data_size )
{
    /*
     * detecting seuquence ending will result in either
     * repeated start or stop condition.
     *
     * Data Size is a POSITION of the last message, to
     * set a certain bit at X position, we should left
     * shift 1 for (X-1) digits
     */

    uint8_t       seq_end_pos = data_size - 1;
    i2c_seq_ending[seq_end_pos >> 5] |= ( 1 << (seq_end_pos) );
}


static inline uint8_t
i2c_sw_exec(uint16_t addr, uint8_t reg, uint8_t* data, uint8_t size,
             uint8_t rw)
{
    uint8_t       addr_size = ADRR_7_BIT_SIZE;   // TODO: Add 10-bit
    uint8_t       i2c_bus_curr_state = i2c_bus_state_get();

    if( i2c_bus_curr_state == Bus_Idle )
    {
        if( (size + addr_size) <= I2C_BUF_SIZE )
        {
            i2c_sw_flush();

            i2c_data_tx[0] = (addr << 1) | I2C_RW_WRITE;
            i2c_data_tx[1] = reg;
            i2c_data_comm_seq_size += 2;

            if( rw == I2C_RW_READ )
            {
                i2c_sw_rst_or_stop_mark_add(i2c_data_comm_seq_size);

                i2c_data_tx[2] = (addr << 1) | rw;
                i2c_data_comm_seq_size++;
            }
            if( rw == I2C_RW_WRITE )
            {
                memcpy(&i2c_data_tx[i2c_data_comm_seq_size], data, size);
            }
            i2c_data_comm_seq_size += size;
            i2c_sw_rst_or_stop_mark_add(i2c_data_comm_seq_size);

            i2c_bus_state = Msg_Prepare;
        }
        else
        {
            i2c_error = I2C_ERR_OVERFLOW;
        }
    }
    else
    if( i2c_bus_curr_state == Done )
    {
        if( rw == I2C_RW_READ ) { memcpy(data, i2c_data_rx, size); }

        i2c_bus_state = Bus_Idle;
    }

    return (i2c_error ? 1 : 0) || (i2c_bus_curr_state == Done);
}


uint8_t
i2c_sw_reg_write( uint16_t addr, uint8_t reg, uint8_t* data, uint8_t size )
{
    return i2c_sw_exec(addr, reg, data, size, I2C_RW_WRITE);
}


uint8_t
i2c_sw_reg_read( uint16_t addr, uint8_t reg, uint8_t* data, uint8_t size )
{
    return i2c_sw_exec(addr, reg, data, size, I2C_RW_READ);
}


//uint8_t
//i2c_sw_data_request( uint16_t addr, uint8_t size )
//{
//    return i2c_msg_add( addr, 0, size, I2C_RW_READ);
//}
//
//
//uint8_t
//i2c_sw_data_size_get( void )
//{
//    return i2c_data_comm_seq_size;
//}


static inline i2c_bus_st
i2c_next_act_get( uint8_t cmd )
{
    /*
     *  Based on the previous sent cmd, we decide to either execute
     *  read sequence (save to rx), or write sequence (send tx)
     */
    return cmd & I2C_RW_READ ? Data_Read : Data_Send;
}


static inline uint8_t
i2c_execute_write( uint8_t clk_toggles, uint8_t data )
{
    uint8_t     clk_even = is_even(clk_toggles);
    uint8_t     bit_hi_lo;
    uint8_t     data_bit = clk_toggles >> 1;
    enum { Data, Ack } msg_sequence = (data_bit < 8) ? Data : Ack;

    if( clk_even ) {SCK_CLR;} else {SCK_SET;}

    if( clk_even  )                                // Prep Data on Lo
    {
        if( msg_sequence == Data )
        {
            bit_hi_lo = (data << data_bit) & 0x80;
            if( bit_hi_lo ) {SDA_SET;} else {SDA_CLR;}
        }
        else /* if( msg_sequence == Ack ) */
        {
            SDA_SET;                                   // Release Bus
        }
    }
    else /* if( !clk_even ) */
    if( msg_sequence == Ack )
    {
        i2c_error = (SDA_GET) ? I2C_ERR_ACK : I2C_ERR_NONE;
    }

    return clk_toggles == 17;                     // Data Frame Ended
}


static inline uint8_t
i2c_execute_read( uint8_t clk_toggles, uint8_t *data, uint8_t seq_msg_final )
{
    uint8_t     clk_even = is_even(clk_toggles);
    uint8_t     data_bit = clk_toggles >> 1;
    uint8_t     bit_hi_lo;
    enum { Data, Ack } msg_sequence = (data_bit < 8) ? Data : Ack;

    if( clk_even ) {SCK_CLR;} else {SCK_SET;}
    if( clk_even )
    {
        if( msg_sequence == Data )
        {
            SDA_SET;                                  // Set Floating
        }
        else  /* if( msg_sequence == Ack ) */
        {
            if (seq_msg_final) { SDA_SET; } else { SDA_CLR; }
        }
    }
    else /* if( !clk_even ) */
    {
        if( msg_sequence == Data )                 // Read Data on Hi
        {
            bit_hi_lo = SDA_GET ? 1 : 0;
            *data |= (bit_hi_lo << (7 - data_bit) );
        }
    }
    return clk_toggles == 17;                     // Data Frame Ended
}


static inline uint8_t
i2c_data_writing_done( uint8_t clk_toggles, uint8_t data )
{
    return i2c_execute_write(clk_toggles, data);
}


static inline uint8_t
i2c_data_reading_done( uint8_t clk_toggles, uint8_t *data, uint8_t msg_fiinal )
{
    return i2c_execute_read(clk_toggles, data, msg_fiinal);
}


static void
i2c_sw_bus_timeproc( void )
{
    static uint16_t     clk_toggles;

    static i2c_bus_st   bus_state_prev;
    static i2c_bus_st   bus_data_seq;  // either Data_Read or Data_Send

    static uint8_t      data_tx[I2C_BUF_SIZE];
    static uint8_t      data_rx[I2C_BUF_SIZE];
    static uint8_t      data_ptr_tx;
    static uint8_t      data_ptr_rx;
    static uint8_t      data_size;

    static uint8_t      recv_byte = 0;
    static uint8_t      seq_msg_final;

    uint8_t             arr_pos;


    if( data_ptr_tx < I2C_BUF_SIZE )
    {
        arr_pos = data_ptr_tx >> 5;
        seq_msg_final = i2c_seq_ending[arr_pos] & (1 << data_ptr_tx) ? 1 : 0;
    }
    else
    {
        seq_msg_final = 1;
    }

    switch( i2c_bus_state )
    {
    case Bus_Idle:
        SDA_SET;
        SCK_SET;
        break;

    case Msg_Prepare:
        memset(data_tx, 0, I2C_BUF_SIZE);
        memset(data_rx, 0, I2C_BUF_SIZE);
        memcpy(data_tx, i2c_data_tx, i2c_data_comm_seq_size);

        data_ptr_tx = 0;
        data_ptr_rx = 0;
        data_size = i2c_data_comm_seq_size;

        i2c_bus_state = Start;
        break;

    case Start_Repeat:
        SDA_CLR;
        SCK_CLR;

        i2c_bus_state = Start;
        break;

    case Start:
        if( !clk_toggles )
        {
            SDA_SET;
            SCK_SET;
        }
        else
        {
            SDA_CLR;
            i2c_bus_state = Cmd_Send;
        }
        clk_toggles++;
        break;

    case Cmd_Send:
        bus_data_seq = i2c_next_act_get( data_tx[data_ptr_tx] );
        i2c_bus_state = Data_Send;
    case Data_Send:
        if( i2c_data_writing_done(clk_toggles, data_tx[data_ptr_tx]) )
        {
            i2c_bus_state = Sequence_handle;
        }
        clk_toggles++;
        break;

    case Data_Read:
        if( i2c_data_reading_done(clk_toggles, &recv_byte, seq_msg_final) )
        {
            data_rx[data_ptr_rx] = recv_byte;

            data_ptr_rx++;
            recv_byte = 0;
            i2c_bus_state = Sequence_handle;
        }
        clk_toggles++;
        break;

    case Sequence_handle:
        SCK_CLR;
        SDA_CLR;

        i2c_bus_state = Stop;
        if( !i2c_error )
        {
            data_ptr_tx++;
            if( data_ptr_tx < data_size )
            {
                i2c_bus_state = seq_msg_final ? Start_Repeat : bus_data_seq;
            } /* else i2c_bus_state = Stop */
        }
        break;

    case Stop:
        if( !clk_toggles )
        {
            SDA_CLR;
            SCK_SET;
        }
        else
        {
            SDA_SET;
            SCK_SET;
            i2c_data_rx_size = data_ptr_rx;
            memcpy(i2c_data_rx, data_rx, I2C_BUF_SIZE);
            i2c_bus_state = Done;
        }
        clk_toggles++;
        break;

    case Done:
//        if( !i2c_data_lock )
        {
        }
        break;

    case Bus_Force_Stop:
        break;
    }

    if( bus_state_prev != i2c_bus_state )
    {
        bus_state_prev  = i2c_bus_state;
        clk_toggles = 0;
    }
}


void
i2c_sw_proc( void )
{
//
//    switch( i2c_proc_stage )
//    {
//    case Stage_Init:
//
//        i2c_proc_stage = Stage_I2C_Idle;
//        break;
//
//    case Stage_I2C_Idle:
//        if( i2c_data_comm_seq_size )
//        {
//            i2c_bus_state = Msg_Prepare;
//            i2c_proc_stage = Stage_I2C_Comm;
//        }
//        break;
//
//    case Stage_I2C_Comm:
//        if( i2c_bus_state == Bus_Idle )
//        {
//            i2c_proc_stage = Stage_Done;
//        }
//        break;
//
//    case Stage_Done:
//        break;
//    }
}


void
i2c_sw_init( void )
{
    GPIO_InitTypeDef   GPIO_InitStruct;

    if( !i2c_module_init )
    {
        RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);

        SCK_SET;
        SDA_SET;

        GPIO_StructInit(&GPIO_InitStruct);
        GPIO_InitStruct.GPIO_Pin   = GPIO_Pin_10 |        // TX
                                     GPIO_Pin_11;         // RX
        GPIO_InitStruct.GPIO_Mode  = GPIO_Mode_OUT;
        GPIO_InitStruct.GPIO_OType = GPIO_OType_OD;
        GPIO_InitStruct.GPIO_Speed = GPIO_Speed_2MHz;
//        GPIO_InitStruct.GPIO_PuPd  = GPIO_PuPd_DOWN;
        GPIO_Init(GPIOC, &GPIO_InitStruct);

        sys_timer_timeproc_add(&i2c_sw_bus_timeproc, 1);

        i2c_module_init = true;
    }
    else
    {
        return;
    }
}
