
#ifndef WINDOW_MANAGER_H_
#define WINDOW_MANAGER_H_

#include "windows/ind_connection.h"
#include "windows/window_main.h"

#include "windows/window_irps_tester.h"

#include "windows/window_setup_modbus.h"
#include "windows/window_setup_mbus_props.h"

#include "windows/window_info_horizontal.h"

#include "windows/window_debug_horizontal.h"

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
// Window ID's
//
//*****************************************************************************
typedef enum
{

    Window_Confirmation,
    Window_Main,
    Window_Irps_Tester,
    Window_F1,
    Window_F2,
    Window_Phase_Inputs,
    Window_Chargers,
    Window_ES_1,
    Window_ES_2,
    Window_Battery,
    Window_Battery_Setup,
    Window_Output,
    Window_Inputs,
    Window_Setup,
    Window_Setup_General,
    Window_Setup_Alarm_Overheat,
    Window_Setup_Alarm_Undervoltage,
    Window_Setup_Alarm_Inputs,
    Window_Info,
    Window_InsulationMonitor,
    Window_InsulationMonitor_Setup,
    Window_VoltageMonitor_Setup,
    Window_TouchCalibration,

    Window_Setup_Mbus_Props,
#if ( INCLUDE_EXTMODBUS == 1 )
    Window_Setup_Modbus,
#endif
#if ( INCLUDE_NET == 1 )
	Window_LanSetup,
#endif
	Window_Debug,

#if (CONNECTION_LOST_AWARENESS == 1)
	Window_Error_Message,
#endif
	WINDOW_NUM,

} tWndID;

//*****************************************************************************
//
// Window manager class definition
//
//*****************************************************************************
class WindowManager
{
public:
    //
    // Constructor
    //
    WindowManager(void) {};

	//
	// Posts message to window manager queue
	//
	static long MessagePost(unsigned long ulMessage, unsigned long ulParam);

	//
	// Process the message from a window manager queue
	//
	static void MessageQueueProcess(void);

    //
    // Destructor
    //
    ~WindowManager(void) {};

private:
	//
	// Handles message from a window manager queue
	//
	static void MessageHandle(unsigned long ulMessage, unsigned long ulParam);

	//
	// Creates a window object
	//
	static Widget* Create(tWndID WndID);

#if CONNECTION_LOST_AWARENESS == 1

	static unsigned char DisplayError(void);

	static Widget* CreateConnectionIndicator(tWndID WnID, Widget *parent);

#endif
};

//*****************************************************************************
//
// Window manager messages
//
//*****************************************************************************
#define WM_MSG_SHOW                 0x00000001
#define WM_MSG_CLOSE                0x00000002
#define WM_MSG_CONNECTION_LOST      0x00000003

//*****************************************************************************
//
// Mark the end of the C bindings section for C++ compilers.
//
//*****************************************************************************
#ifdef __cplusplus
}
#endif

#endif /* WINDOW_MANAGER_H_ */
