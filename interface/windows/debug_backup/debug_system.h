
#ifndef DEBUG_SYSTEM_H_
#define DEBUG_SYSTEM_H_

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

#define ALLOW_MAC_SETTING            0
//*****************************************************************************
//
// Class describes this window
//
//*****************************************************************************
class DebugSystem : public Widget
{
public:
	DebugSystem(unsigned char ucID, Widget *pParent);

	void DebugSystemRender(tContext *pContext);
	void DebugDebugRender(tContext *pContext);
    void DebugNetRender(tContext *pContext);

	long MessageProc(unsigned long ulMsg, unsigned long ulParam1,
				     unsigned long ulParam2);

	~DebugSystem(void);

private:
	RectangularButton   **pButtons;
	Canvas              **alertWindow;
    unsigned char       mac_modified[6];
};

//*****************************************************************************
//
// Mark the end of the C bindings section for C++ compilers.
//
//*****************************************************************************
#ifdef __cplusplus
}
#endif

#endif /* DEBUG_SYSTEM_H_ */
