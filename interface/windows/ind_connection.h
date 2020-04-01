/*
 * window_error_message.h
 *
 *  Created on: 5 нояб. 2018 г.
 *      Author: V.Striukov
 */

#ifndef CONNECTION_LOST_INDICATOR_H_
#define CONNECTION_LOST_INDICATOR_H_

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
class IndicatorConnection : public Widget
{
public:
	IndicatorConnection(unsigned char ucID, Widget *pParent);

	void IndicatorAnimationRender(tContext *sCtx);

	long MessageProc(unsigned long ulMsg, unsigned long ulParam1,
				     unsigned long ulParam2);

private:
	~IndicatorConnection(void);
};

//*****************************************************************************
//
// Mark the end of the C bindings section for C++ compilers.
//
//*****************************************************************************
#ifdef __cplusplus
}
#endif

#endif /* INTERFACE_WINDOWS_CONNECTION_LOST_INDICATOR_H_ */
