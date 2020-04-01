/*
 * window_cell_setup_popup.h
 *
 *  Created on: 18 февр. 2020 г.
 *      Author: V.Striukov
 */

#ifndef INTERFACE_WINDOWS_WINDOW_CELL_SETUP_POPUP_H_
#define INTERFACE_WINDOWS_WINDOW_CELL_SETUP_POPUP_H_

/****
 *  C Binding header for C++ compiler
 */
#ifdef __cplusplus
extern "C"
{
#endif

#include "inter_comm.h"

class WindowCellSetupPopup : public Widget
{
public:
    WindowCellSetupPopup( unsigned char ucID, Widget *pParent,
                          uint16_t icomm_id, int32_t pos_x, int32_t pos_y );

    long MessageProc(unsigned long ulMsg, unsigned long ulParam1,
                     unsigned long ulParam2);
    void interface_update( void );
    void display_values_update( tContext & sCtx, tRectangle & sPosition );
    void setup_value_modify( uint8_t new_data );

    void new_icomm_addr_set(  uint16_t icomm_id );

    ~WindowCellSetupPopup(void);

private:
    RectangularButton         **buttons;
    uint8_t                     input_in_process;
    TickType_t                  input_action_timer;
    uint16_t                    comm_id;
    format_t                    format;

    uint8_t                     minus_sign;
    uint16_t                    display_value;
    uint16_t                    setup_value;
    uint16_t                    modify_value;

    TickType_t                  blink_timer;
    uint8_t                     blink;

    char                        string[16];
};



/****
 *  Mark the end of the C Binding section for C++ compilers.
 */
#ifdef __cplusplus
}
#endif

#endif /* INTERFACE_WINDOWS_WINDOW_CELL_SETUP_POPUP_H_ */
