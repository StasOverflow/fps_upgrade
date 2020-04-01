#ifndef WINDOW_DEBUG_HORIZONTAL_H_
#define WINDOW_DEBUG_HORIZONTAL_H_

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
class WindowDebugHorizontal : public Widget
{
public:
    WindowDebugHorizontal(unsigned char ucID, Widget *pParent);

	Widget* CreateTab(unsigned short usTab);

	long MessageProc(unsigned long ulMsg, unsigned long ulParam1,
				     unsigned long ulParam2);

	~WindowDebugHorizontal(void);
};

//*****************************************************************************
//
// Mark the end of the C bindings section for C++ compilers.
//
//*****************************************************************************
#ifdef __cplusplus
}
#endif

#endif /* WINDOW_DEBUG_HORIZONTAL_H_ */
