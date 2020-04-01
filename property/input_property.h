
#ifndef INPUT_PROPERTY_H_
#define INPUT_PROPERTY_H_

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

#include "property.h"

enum eInputStatus
{
	INPUT_STATUS_UNKNOWN,
	INPUT_STATUS_OFF,
	INPUT_STATUS_FAULT,
	INPUT_STATUS_PHASE_SEQUENCE_WRONG,
	INPUT_STATUS_SYNCING,
	INPUT_STATUS_READY,
	INPUT_STATUS_OK
};


enum {
    Input_Status_Fault,
    Input_Status_Ok,
};


unsigned short InputPhasePropertyGet( unsigned short usFeeder, unsigned short usProperty );
void           InputPhasePropertyStringGet( unsigned short usFeeder, unsigned short usProperty, char *pcString );

unsigned short InputFeederPropertyGet( unsigned short usGroup, unsigned short usProperty );
void           InputFeederPropertyStringGet( unsigned short usGroup, unsigned short usProperty, char *pcString );

void           InputPhasePropertyValidate( unsigned short usPhase, unsigned short usProperty, unsigned short *pusValue );
void           InputPhasePropertySet( unsigned short usPhase, unsigned short usProperty, unsigned short usValue );
unsigned short InputPhasePropertyStepGet( unsigned short usPhase, unsigned short usProperty );

void           InputPropertyInit( void );

//*****************************************************************************
//
// Mark the end of the C bindings section for C++ compilers.
//
//*****************************************************************************
#ifdef __cplusplus
}
#endif

#endif /* INPUT_PROPERTY_H_ */
