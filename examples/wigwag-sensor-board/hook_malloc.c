/*
 * hook_malloc.c
 *
 *  Created on: Sep 9, 2011
 *      Author: ed
 */

#include <malloc.h>
#include <stdio.h>

/**
 * From ld manual:
--wrap symbol
Use a wrapper function for symbol. Any undefined reference to symbol will be resolved to "__wrap_ symbol ". Any undefined reference to "__real_ symbol " will be resolved to symbol.
This can be used to provide a wrapper for a system function. The wrapper function should be called "__wrap_ symbol ". If it wishes to call the system function, it should call "__real_ symbol ".
 *
 */

void *__real_malloc(size_t);

void *__wrap_malloc(size_t c) {
	printf ("** MC: %u\n", c);
	return __real_malloc (c);
}


// Below is a GNU libc built in way
// http://www.gnu.org/s/hello/manual/libc/Hooks-for-Malloc.html



//
///* Prototypes for our hooks.  */
//static void my_init_hook(void);
//static void *my_malloc_hook(size_t, const void *);
//static void my_free_hook(void*, const void *);
//
///* Override initializing hook from the C library. */
//void (*__malloc_initialize_hook)(void) = my_init_hook;
//
//static void my_init_hook(void) {
//	old_malloc_hook = __malloc_hook;
//	old_free_hook = __free_hook;
//	__malloc_hook = my_malloc_hook;
//	__free_hook = my_free_hook;
//}
//
//static void *
//my_malloc_hook(size_t size, const void *caller) {
//	void *result;
//	/* Restore all old hooks */
//	__malloc_hook = old_malloc_hook;
//	__free_hook = old_free_hook;
//	/* Call recursively */
//	result = malloc(size);
//	/* Save underlying hooks */
//	old_malloc_hook = __malloc_hook;
//	old_free_hook = __free_hook;
//	/* printf might call malloc, so protect it too. */
//	printf("malloc (%u) returns %p\n", (unsigned int) size, result);
//	/* Restore our own hooks */
//	__malloc_hook = my_malloc_hook;
//	__free_hook = my_free_hook;
//	return result;
//}
//
//static void my_free_hook(void *ptr, const void *caller) {
//	/* Restore all old hooks */
//	__malloc_hook = old_malloc_hook;
//	__free_hook = old_free_hook;
//	/* Call recursively */
//	free(ptr);
//	/* Save underlying hooks */
//	old_malloc_hook = __malloc_hook;
//	old_free_hook = __free_hook;
//	/* printf might call free, so protect it too. */
//	printf("freed pointer %p\n", ptr);
//	/* Restore our own hooks */
//	__malloc_hook = my_malloc_hook;
//	__free_hook = my_free_hook;
//}
//
