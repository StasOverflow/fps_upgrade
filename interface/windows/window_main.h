#ifndef WINDOW_MAIN_H_
#define WINDOW_MAIN_H_

/****
 *  C Binding header for C++ compiler
 */
#ifdef __cplusplus
extern "C"
{
#endif

#include "stdint.h"
#include "tables/modbus_table.h"


/****
 *  Class, that contains methods and props of a current widget
 */
class WindowMain : public Widget
{
public:
    WindowMain(unsigned char ucID, Widget *pParent);

    void actual_background_render(tContext *pContext);
    void inner_context_render(tContext *pContext);
    void on_button_text_render(tContext *pContext);
    void button_disable_handler( void );

    long MessageProc(unsigned long ulMsg, unsigned long ulParam1,
                     unsigned long ulParam2);

    void modules_additional_info_render( tContext *pContext );

    ~WindowMain(void);

private:
    RectangularButton        **button;
    ColorPalette             *palette;
    uint32_t                 adjust_timer;
    ModbusTable              *table;
};


/****
 *  Mark the end of the C Binding section for C++ compilers.
 */
#ifdef __cplusplus
}
#endif


#endif /* WINDOW_MAIN_H_ */
