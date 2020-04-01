
#ifndef DIALOG_ADJUST_DOUBLE_H_
#define DIALOG_ADJUST_DOUBLE_H_

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

#define VERY_SPECIAL_DEBUG_BUTTON    0

typedef enum {
    Whole,
	Higher,
	Lower,
} val_t;

typedef enum {
    Alphabetical,
    Hexadecimal,
    Decimal,
    Octal,
    Binary,
    BoundaryCount,
} boundary_t;

/*
 *
 * The following enum represents a value, used for bit shifting, depending on
 * a size of a value passed
 *
 * e.g.: if we use u_short decimal value, like (65535) we pass 'Short' as a parameter,
 * leaving us with the following situation: a bit-shift value became 8, so the
 * operation of bit shifing will return us a left value of 255 and right value
 * of 255 too.
 *
 * lValueLeft  = ParameterValue >> bit-shift
 * lValueRight = ParameterValue &  ((1 << bit-shift) - 1)
 *
 */

typedef enum {
    Byte  =  4,
    Short =  8,
} bit_shift_t;


class DialogAdjustDouble : public Widget
{
public:
	DialogAdjustDouble(unsigned char ucID,
	                   Widget *pParent,
	                   long lX, long lY,
	                   unsigned short usValue,
	                   bit_shift_t digit_max_size,
	                   boundary_t bounds);

	short ValueGet(val_t value);

	long  MessageProc(unsigned long ulMsg,
	                  unsigned long ulParam1,
	                  unsigned long ulParam2);

	~DialogAdjustDouble( void );

private:
#if VERY_SPECIAL_DEBUG_BUTTON == 1
    void  DebugInfoRender(tContext *pContext);
#endif
    void                DisplayableValuesUpdate(void);
	void                ValueModify(unsigned long & usValue, char sign);
	unsigned long       usValue;
	unsigned long       usValueInitial;
	unsigned long       ucValueHigher;
	unsigned long       ucValueLower;
	boundary_t          xBounds;
	bit_shift_t         xDigitMaxSize;
	char*               pcText;
	Canvas*             pxCanvasText;
};

//*****************************************************************************
//
// Mark the end of the C bindings section for C++ compilers.
//
//*****************************************************************************
#ifdef __cplusplus
}
#endif

#endif /* DIALOG_ADJUST_DOUBLE_H_ */
