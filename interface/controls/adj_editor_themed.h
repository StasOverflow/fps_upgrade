/*
 * adj_editor_themed.h
 *
 *  Created on: 27 янв. 2020 г.
 *      Author: V.Striukov
 */

#ifndef INTERFACE_CONTROLS_ADJ_EDITOR_THEMED_H_
#define INTERFACE_CONTROLS_ADJ_EDITOR_THEMED_H_


typedef enum {
    Adj_Iface_Arrows,
    Adj_Iface_Math_Sign,
} adj_editor_iface_t;


//*****************************************************************************
//
// If building with a C++ compiler, make all of the definitions in this header
// have a C binding.
//
//*****************************************************************************
#ifdef __cplusplus
extern "C"
{
#endif

class AdjEditor : public Widget
{
public:
    AdjEditor( unsigned char ucID, Widget *pParent, long lX, long lY, short use_arrows );

    void Init( long lValue, long lStep, char *pcText );
    void enable_state_set( uint8_t enable );

    long ValueGet(void)
    {
        return lValue;
    }

    long MessageProc(unsigned long ulMsg, unsigned long ulParam1,
                     unsigned long ulParam2);

    ~AdjEditor( void );

private:
    /*
     * For this to actually work (change color on the fly)
     * whe have to do the following:
     * 1) Assign a pointer to all canvases, created by the
     *    widget (because i dont know how to get an access
     *    to a child other way around)
     *
     * 2) Provide an interface to each parameter, that
     *    will not only assign a new value to property,
     *    but will redraw the widget with a new values.
     */


    adj_editor_iface_t      iface;
    uint32_t                btn_color;
    uint32_t                btn_text_color;
    uint32_t                bg_color;
    uint32_t                bg_text_color;

    RectangularButton       *button[2];
    long lValue;

    long lStep;

    char *pcText;
};

//*****************************************************************************
//
// Mark the end of the C bindings section for C++ compilers.
//
//*****************************************************************************
#ifdef __cplusplus
}
#endif

#endif /* INTERFACE_CONTROLS_ADJ_EDITOR_THEMED_H_ */
