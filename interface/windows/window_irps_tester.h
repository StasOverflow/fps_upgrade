/*
 * window_irps_tester.h
 *
 *  Created on: 25 мар. 2020 г.
 *      Author: V.Striukov
 */

#ifndef INTERFACE_WINDOWS_WINDOW_IRPS_TESTER_H_
#define INTERFACE_WINDOWS_WINDOW_IRPS_TESTER_H_

/****
 *  C Binding header for C++ compiler
 */
#ifdef __cplusplus
extern "C"
{
#endif


/****
 *  Class, that contains methods and props of a current widget
 */
class WindowIrpsTester : public Widget
{
public:
    WindowIrpsTester(unsigned char ucID, Widget *pParent);

    void handle_interface_cmd(tContext *pContext);
    void inner_context_render(tContext *pContext);

    long MessageProc(unsigned long ulMsg, unsigned long ulParam1,
                     unsigned long ulParam2);

    ~WindowIrpsTester(void);

private:
    RectangularButton        **button;
    uint32_t                 adjust_timer;
    uint8_t                  curr_btn_pressed;
    uint8_t                  unprsd;
};





/****
 *  Mark the end of the C Binding section for C++ compilers.
 */
#ifdef __cplusplus
}
#endif

#endif /* INTERFACE_WINDOWS_WINDOW_IRPS_TESTER_H_ */
