
#ifndef DIALOG_CONFIRMATION_H_
#define DIALOG_CONFIRMATION_H_

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

class DialogConfirmation : public Widget
{
public:
	DialogConfirmation( unsigned char ucID, Widget *pParent, long lX, long lY );

	long MessageProc(unsigned long ulMsg, unsigned long ulParam1,
			         unsigned long ulParam2);

	~DialogConfirmation( void );
};

//*****************************************************************************
//
// Mark the end of the C bindings section for C++ compilers.
//
//*****************************************************************************
#ifdef __cplusplus
}
#endif

#endif /* DIALOG_CONFIRMATION_H_ */
