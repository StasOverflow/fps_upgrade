
#ifndef CONTROL_NUMERIC_EDITOR_H_
#define CONTROL_NUMERIC_EDITOR_H_

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

class ControlNumericEditor : public Widget
{
public:
	ControlNumericEditor( unsigned char ucID, Widget *pParent, long lX, long lY );

	void Init( long lValue, long lStep, char *pcText );

	long ValueGet(void)
	{
		return this->lValue;
	}

	long MessageProc(unsigned long ulMsg, unsigned long ulParam1,
			         unsigned long ulParam2);

	~ControlNumericEditor( void );

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

#endif /* CONTROL_NUMERIC_EDITOR_H_ */
