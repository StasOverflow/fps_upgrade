/*
 * window_error_message.h
 *
 *  Created on: 7 нояб. 2018 г.
 *      Author: V.Striukov
 */

#ifndef INTERFACE_WINDOWS_WINDOW_ERROR_MESSAGE_H_
#define INTERFACE_WINDOWS_WINDOW_ERROR_MESSAGE_H_


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

//*****************************************************************************
//
// Class describes this window
//
//*****************************************************************************
class WindowErrorMessage : public Widget
{
public:
	WindowErrorMessage(unsigned char ucID, Widget *pParent);

	void MessageBoxRender(tContext *sCtx);

	long MessageProc(unsigned long ulMsg, unsigned long ulParam1,
				     unsigned long ulParam2);

private:
	~WindowErrorMessage(void);
};

//*****************************************************************************
//
// Mark the end of the C bindings section for C++ compilers.
//
//*****************************************************************************
#ifdef __cplusplus
}
#endif

#endif /* INTERFACE_WINDOWS_WINDOW_ERROR_MESSAGE_H_ */
