
#ifndef DEBUG_BATT_SWIRE_H_
#define DEBUG_BATT_SWIRE_H_

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
class DebugBattSwire : public Widget
{
public:
	DebugBattSwire(unsigned char ucID, Widget *pParent);

	void ACQMBDebugDraw(tContext *pContext);
	void APSDebugDraw(tContext *pContext);
	void BattShuntDebugDraw(tContext *pContext);

	long MessageProc(unsigned long ulMsg, unsigned long ulParam1,
				     unsigned long ulParam2);

	~DebugBattSwire(void);

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

#endif /* DEBUG_BATT_SWIRE_H_ */
