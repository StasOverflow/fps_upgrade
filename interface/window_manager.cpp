
#include "../comm/inter_comm.h"
#include "interface.h"

//*****************************************************************************
//
// The message queue size.
//
//*****************************************************************************
#define MESSAGE_QUEUE_SIZE          4

//*****************************************************************************
//
// This structure describes the message queue item used to hold window manager
// messages.
//
//*****************************************************************************
typedef struct
{
    //
    // The message to be sent.
    //
    unsigned long ulMessage;

    //
    // The parameter of the message.
    //
    unsigned long ulParam;

} tMessageQueueItem;

//*****************************************************************************
//
// The array of the message queue items
//
//*****************************************************************************
static tMessageQueueItem sMessageQueueItems[MESSAGE_QUEUE_SIZE];

//*****************************************************************************
//
// The offset to the next message to be read from the message queue
//
//*****************************************************************************
static unsigned long ulReadIndex;

//*****************************************************************************
//
// The offset to the next message to be written to the message queue
//
//*****************************************************************************
static unsigned long ulWriteIndex;

static Widget	        *pCurrentWidget;
static tWndID           usWndCallStack[10];
static tWndID	        ulCurrentWindowID;
static unsigned short   usWndCallStackIndex;

//*****************************************************************************
//
// Posts message to window manager queue
//
//*****************************************************************************
long
WindowManager::MessagePost(unsigned long ulMessage, unsigned long ulParam)
{
    unsigned long     ulNext;

    //
    // Compute the next value for the write pointer.
    //
    ulNext = (ulWriteIndex + 1) % MESSAGE_QUEUE_SIZE;

    //
    // Return a failure if the message queue is full.
    //
    if(ulNext == ulReadIndex)
    {
        return(0);
    }

    //
    // Write this message into the next location in the message queue.
    //
    sMessageQueueItems[ulWriteIndex].ulMessage = ulMessage;
    sMessageQueueItems[ulWriteIndex].ulParam  = ulParam;

    //
    // Update the message queue write pointer.
    //
    ulWriteIndex = ulNext;

    //
    // Success.
    //
    return(1);
}

//*****************************************************************************
//
// Process the message from a window manager queue
//
//*****************************************************************************
void
WindowManager::MessageQueueProcess(void)
{
	unsigned long ulMessage;
	unsigned long ulParam;

    //
    // Loop while there are more messages in the message queue.
    //
    while(ulReadIndex != ulWriteIndex)
    {
        //
        // Copy the contents of this message.
        //
        ulMessage = sMessageQueueItems[ulReadIndex].ulMessage;
        ulParam   = sMessageQueueItems[ulReadIndex].ulParam;

        //
        // Remove this message from the queue.
        //
        ulReadIndex = (ulReadIndex + 1) % MESSAGE_QUEUE_SIZE;

        //
        // Handle the message.
        //
        WindowManager::MessageHandle(ulMessage, ulParam);
    }
}

//*****************************************************************************
//
// Handles message from a window manager queue
//
//*****************************************************************************
void
WindowManager::MessageHandle(unsigned long ulMessage, unsigned long ulParam)
{
    tWndID WindowID;

	switch(ulMessage)
	{
	case WM_MSG_CLOSE:
		//
		// Pop bottom window ID from the window call stack
		//
		WindowID = usWndCallStack[--usWndCallStackIndex];

	    if( pCurrentWidget )
	    {
	        pCurrentWidget->Destroy();
	    }

	    pCurrentWidget = WindowManager::Create(WindowID);

	    ulCurrentWindowID = WindowID;

		break;

	case WM_MSG_SHOW:
		//
		// Push current window ID to the window call stack
		//
		usWndCallStack[usWndCallStackIndex++] = ulCurrentWindowID;

        WindowID = (tWndID)ulParam;

        if( pCurrentWidget )
        {
            pCurrentWidget->Destroy();
        }

        pCurrentWidget = WindowManager::Create(WindowID);

        ulCurrentWindowID = WindowID;


		break;
	}

}

//*****************************************************************************
//
// Creates a window object
//
//*****************************************************************************
Widget*
WindowManager::Create(tWndID WndID)
{
	Widget *pWindow = 0;

	switch( WndID )
	{
	case Window_Main:
		pWindow = new WindowMain(WndID, WIDGET_ROOT);
		break;

    case Window_Irps_Tester:
        pWindow = new WindowIrpsTester(WndID, WIDGET_ROOT);
        break;

#if ( INCLUDE_EXTMODBUS == 1 )
    case Window_Setup_Modbus:
//        pWindow = new WindowSetupModbus(WndID, WIDGET_ROOT);
        break;
#endif

    case Window_Info:
//        pWindow = new WindowInfo(WndID, WIDGET_ROOT);
        break;

#if ( INCLUDE_NET == 1 )
	case Window_LanSetup:
		pWindow = new WindowLanSetup(WndID, WIDGET_ROOT);
		break;
#endif

    case Window_Setup_Mbus_Props:
        pWindow = new WindowSetupMbusProps(WndID, WIDGET_ROOT);
        break;

	case Window_Debug:
//		pWindow = new WindowDebugHorizontal(WndID, WIDGET_ROOT);
		break;

	default:
		break;
	}

	return pWindow;
}

