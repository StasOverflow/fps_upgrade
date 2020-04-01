
#ifndef DIALOG_ADJUST_H_
#define DIALOG_ADJUST_H_

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

class DialogAdjust : public Widget
{
public:
	DialogAdjust( unsigned char ucID, Widget *pParent, long lX, long lY );

	void Init( long lValue, char *pcText );

	long ValueGet(void)
	{
		return this->lValue;
	}

	long MessageProc(unsigned long ulMsg, unsigned long ulParam1,
			         unsigned long ulParam2);

	~DialogAdjust( void );

private:
	long lValue;

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

#endif /* DIALOG_ADJUST_H_ */
