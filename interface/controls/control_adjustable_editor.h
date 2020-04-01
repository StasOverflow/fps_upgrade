/*
 * control_numeric_editor_v_0.9.h
 *
 *  Created on: 7 мая 2019 г.
 *      Author: V.Striukov
 */

#ifndef INTERFACE_CONTROLS_CONTROL_ADJUSTABLE_EDITOR_H_
#define INTERFACE_CONTROLS_CONTROL_ADJUSTABLE_EDITOR_H_

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

class ControlAdjustableEditor : public Widget
{
public:
    ControlAdjustableEditor( unsigned char ucID, Widget *pParent, long lX, long lY, short use_arrows );

    void Init( long lValue, long lStep, char *pcText );

    long ValueGet(void)
    {
        return this->lValue;
    }

    long MessageProc(unsigned long ulMsg, unsigned long ulParam1,
                     unsigned long ulParam2);

    ~ControlAdjustableEditor( void );

private:
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


#endif /* INTERFACE_CONTROLS_CONTROL_ADJUSTABLE_EDITOR_H_ */
