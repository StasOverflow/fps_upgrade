
#include "lwip/opt.h"
#include "lwip/sys.h"

/* Scheduler includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"

#include <string.h>



volatile unsigned int mem_alloc_count;
volatile unsigned int mem_free_count;
volatile unsigned int mem_error_count;


#define POINTERS_COUNT              256
static volatile unsigned long pointers[POINTERS_COUNT];


void
mem_init(void)
{

}

/**
 * Put a struct mem back on the heap
 *
 * @param rmem is the data portion of a struct mem as returned by a previous
 *             call to mem_malloc()
 */
void
mem_free(void *p)
{
	SYS_ARCH_DECL_PROTECT(lev);

	SYS_ARCH_PROTECT(lev);

	mem_free_count += 1;

	vPortFree(p);

	for( int i = 0; i < POINTERS_COUNT; i++ )
	{
		if( pointers[i] == (unsigned long)p )
		{
			pointers[i] = 0;
			break;
		}
	}

	SYS_ARCH_UNPROTECT(lev);
}

/**
 * In contrast to its name, mem_realloc can only shrink memory, not expand it.
 * Since the only use (for now) is in pbuf_realloc (which also can only shrink),
 * this shouldn't be a problem!
 *
 * @param rmem pointer to memory allocated by mem_malloc the is to be shrinked
 * @param newsize required size after shrinking (needs to be smaller than or
 *                equal to the previous size)
 * @return for compatibility reasons: is always == rmem, at the moment
 *         or NULL if newsize is > old size, in which case rmem is NOT touched
 *         or freed!
 */
void *
mem_realloc(void *rmem, mem_size_t newsize)
{
	return rmem;
}

/**
 * Adam's mem_malloc() plus solution for bug #17922
 * Allocate a block of memory with a minimum of 'size' bytes.
 *
 * @param size is the minimum size of the requested block in bytes.
 * @return pointer to allocated memory or NULL if no free memory was found.
 *
 * Note that the returned value will always be aligned (as defined by MEM_ALIGNMENT).
 */
void *
mem_malloc( mem_size_t size )
{
	void *p;

	SYS_ARCH_DECL_PROTECT(lev);

	SYS_ARCH_PROTECT(lev);

	mem_alloc_count += 1;

	p = pvPortMalloc(size);

	if( p == NULL ) {
		mem_error_count += 1;
	}

	for( int i = 0; i < POINTERS_COUNT; i++ )
	{
		if( pointers[i] == 0 )
		{
			pointers[i] = (unsigned long)p;
			break;
		}
	}

	SYS_ARCH_UNPROTECT(lev);

	return p;
}

/**
 * Contiguously allocates enough space for count objects that are size bytes
 * of memory each and returns a pointer to the allocated memory.
 *
 * The allocated memory is filled with bytes of value zero.
 *
 * @param count number of objects to allocate
 * @param size size of the objects to allocate
 * @return pointer to allocated memory / NULL pointer if there is an error
 */
void *mem_calloc(mem_size_t count, mem_size_t size)
{
	void *p;

	/* allocate 'count' objects of size 'size' */
	p = mem_malloc(count * size);

	if( p )
	{
		memset(p, 0, count * size); /* zero the memory */
	}

	return p;
}

/**
 * Shrink memory returned by mem_malloc().
 *
 * @param rmem pointer to memory allocated by mem_malloc the is to be shrinked
 * @param newsize required size after shrinking (needs to be smaller than or
 *                equal to the previous size)
 * @return for compatibility reasons: is always == rmem, at the moment
 *         or NULL if newsize is > old size, in which case rmem is NOT touched
 *         or freed!
 */
void *
mem_trim(void *rmem, mem_size_t newsize)
{
	return rmem;
}
