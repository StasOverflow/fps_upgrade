
#include "FreeRTOS.h"

unsigned short g_usMallocCnt = 0;

//*****************************************************************************
//
// Operator new redefinition
//
//*****************************************************************************
void* operator new(size_t size) throw()
{
	g_usMallocCnt++;

	return pvPortMalloc(size);
}

//*****************************************************************************
//
// Operator new[] redefinition
//
//*****************************************************************************
void* operator new[](size_t size) throw()
{
	g_usMallocCnt++;

	return pvPortMalloc(size);
}

//*****************************************************************************
//
// Operator delete redefinition
//
//*****************************************************************************
void operator delete(void* ptr) throw ()
{
	g_usMallocCnt--;

	vPortFree(ptr);
}

//*****************************************************************************
//
// Operator delete[] redefinition
//
//*****************************************************************************
void operator delete[](void* ptr) throw ()
{
	g_usMallocCnt--;

	vPortFree(ptr);
}

//*****************************************************************************
//
// Pure virtual function stub
//
//*****************************************************************************
extern "C" void __cxa_pure_virtual(void)
{
	for(;;);
}
