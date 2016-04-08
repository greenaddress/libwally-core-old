#ifndef LIBWALLY_SMALLBUF_H
#define LIBWALLY_SMALLBUF_H 1

/**
 * Macros for typically small buffers that switch to heap memory after a
 * certain size threshold.
 *
 * Declare a small buffer of 1024 bytes.
 * sb_declare(foo, unsigned char, 1024);
 *
 * Allocate the memory for the buffer. Anything larger than 1024 bytes will
 * be allocated from the heap. Don't forget to check the result.
 * sb_alloc(foo, unsigned char, runtime_size);
 * if (!foo)
 *     error_handling_code();
 *
 * Free the allocation:
 * sb_free(foo);
 */
#include <config.h>

#ifdef HAVE_ALLOCA
#include <alloca.h>
#include <stdlib.h>

#define sb_declare(name, type_, threshold) \
    const size_t name ## _threshold = threshold; \
    type_ *name ## _local = NULL; \
    type_ *name = NULL

/* The condition checking form below is used in case our platforms
 * alloca implementation frees the allocated block on scope exit.
 */
#define sb_cond_alloc(name, type_, count, cond) \
    (void)((cond) && (count) <= name ## _threshold && \
           (name ## _local = (type_ *)alloca((count) * sizeof(*name)))); \
    if ((cond) && (count) > name ## _threshold) \
        name = (type_ *)malloc((count) * sizeof(*name)); \
    else if (cond) \
        name = name ## _local

#define sb_free(name) \
    if (name && name != name ## _local) \
        free(name)

#else /* HAVE_ALLOCA */

#define sb_declare(name, type_, threshold) \
    type_ name ## _local[threshold]; \
    type_ *name = NULL

#define sb_cond_alloc(name, type_, count, cond) \
    if ((cond) && (count) > sizeof(name ## _local) / sizeof(name ## _local[0])) \
        name = (type_ *)malloc((count) * sizeof(name ## _local[0])); \
    else if (cond) \
        name = name ## _local

#define sb_free(name) \
    if (name && name != name ## _local) \
        free(name)

#endif /* HAVE_ALLOCA */

#define sb_alloc(name, type_, count) \
    sb_cond_alloc(name, type_, (count), true)


#endif /* LIBWALLY_SMALLBUF_H */
