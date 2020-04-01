
#ifndef DEBUG_BATT_PID_H_
#define DEBUG_BATT_PID_H_

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
class DebugBattPID : public Widget
{
public:
	DebugBattPID(unsigned char ucID, Widget *pParent);

	void BattPIDDraw(tContext *pContext);
	void BattParamDraw(tContext *pContext);
	void BattStatusDraw(tContext *pContext);

	long MessageProc(unsigned long ulMsg, unsigned long ulParam1,
				     unsigned long ulParam2);

	~DebugBattPID(void);

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

#endif /* DEBUG_BATT_PID_H_ */
