
#ifndef DIALOG_NUMERIC_EDITOR_H_
#define DIALOG_NUMERIC_EDITOR_H_

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

class DialogNumericEditor : public Widget
{
public:
	DialogNumericEditor( unsigned char ucID, Widget *pParent, long lX, long lY );

	long MessageProc(unsigned long ulMsg, unsigned long ulParam1,
			         unsigned long ulParam2);

	void Init(long lValue, unsigned char ucDotPos);

	long ValueGet( void )
	{
		return lValue;
	}

	~DialogNumericEditor( void );

private:
	unsigned char ucAnyNumberPressed;
	unsigned char ucDotPos;
	long          lValue;
	char          *pcText;
};
//*****************************************************************************
//
// Mark the end of the C bindings section for C++ compilers.
//
//*****************************************************************************
#ifdef __cplusplus
}
#endif

#endif /* DIALOG_NUMERIC_EDITOR_H_ */
