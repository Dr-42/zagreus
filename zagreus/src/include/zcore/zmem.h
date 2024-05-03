#ifndef ZMEM_H
#define ZMEM_H

#include <stdlib.h>
#include <stdint.h>

void zmem_disable_logging(void);

void *zmalloc(size_t size);
void *zrealloc(void *ptr, size_t size);
void *zcalloc(size_t nmemb, size_t size);
void zfree(void *ptr);

void zmemzero(void *ptr, size_t size);
void zmemfill(void *ptr, size_t size, uint8_t byte);
void zmemcpy(void *dst, const void *src, size_t size);

#endif // ZMEM_H
