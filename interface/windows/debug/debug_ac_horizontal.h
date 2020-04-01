
#ifndef DEBUG_AC_HORIZONTAL_H_
#define DEBUG_AC_HORIZONTAL_H_

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
class DebugACHorizontal : public Widget
{
public:
    DebugACHorizontal(unsigned char ucID, Widget *pParent);

	long MessageProc(unsigned long ulMsg, unsigned long ulParam1,
				     unsigned long ulParam2);

	~DebugACHorizontal(void);

private:
	RectangularButton **pButtons;
};

//*****************************************************************************
//
// Mark the end of the C bindings section for C++ compilers.
//
//*****************************************************************************
#ifdef __cplusplus
}
#endif

#endif /* DEBUG_AC_HORIZONTAL_H_ */
