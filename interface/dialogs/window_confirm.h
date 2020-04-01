/*
 * confirm_window.h
 *
 *  Created on: 1 нояб. 2018 г.
 *      Author: V.Striukov
 */

#ifndef _WINDOW_CONFIRM_H_
#define _WINDOW_CONFIRM_H_

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

class WindowConfirm : public Widget
{
public:
	WindowConfirm( unsigned char ucID, Widget *pParent, long lX, long lY );

	void ConfirmTextRender(tContext *sCtx);

	long MessageProc(unsigned long ulMsg, unsigned long ulParam1,
			         unsigned long ulParam2);

	void Init( bool critical,  char * pcTextAction, char * pcTextWarning );
	~WindowConfirm( void );
private:
	RectangularButton   **pButtons;
	short		  alarmType;
	char          *pcOutputWarning;
	char          *pcOutputAction;
};

//*****************************************************************************
//
// Mark the end of the C bindings section for C++ compilers.
//
//*****************************************************************************
#ifdef __cplusplus
}
#endif

#endif /* INTERFACE_DIALOGS_WINDOW_CONFIRM_H_ */
