#include "zcore/zmem.h"

// zi stands for zagreus internal

#include <math.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "zcore/zlog.h"
#include "zcore/ztrace.h"

typedef struct trace_data_t {
	void** trace;
	size_t size;
} trace_data_t;

typedef struct {
	size_t size;
	void* data;
	trace_data_t allocation_trace;
	size_t num_reallocs;
	trace_data_t* realloc_traces;
} zmem_chunk_t;

static int zmem_logging_enabled = 1;
static zmem_chunk_t* zmem_chunks = NULL;
static size_t zmem_chunks_num = 0;
static size_t zmem_chunk_capacity = 0;
static double zmem_realloc_factor = 1.6;

// void *malloc(size_t size);
// void free(void *_Nullable ptr);
// void *calloc(size_t nmemb, size_t size);
// void *realloc(void *_Nullable ptr, size_t size);

// Declare the stdlib functions as function pointers so it's easier to use other functions with the same signature
static void *(*zi_malloc)(size_t size) = malloc;
static void (*zi_free)(void *ptr) = free;
// static void *(*zi_calloc)(size_t nmemb, size_t size) = calloc;
static void *(*zi_realloc)(void *ptr, size_t size) = realloc;

size_t zi_find_entry(void* ptr) {
	for (size_t i = 0; i < zmem_chunks_num; i++) {
		if (zmem_chunks[i].data == ptr) {
			return i;
		}
	}
	return UINT64_MAX;
}

void zmem_disable_logging(void) {
	zmem_logging_enabled = 0;
}

void zmem_set_realloc_factor(double factor) {
	zmem_realloc_factor = factor;
}
// From ztrace.h
//int get_intermediate_trace(void** stack, size_t size)
//int sprint_intermediate_trace(void** stack, char* buffer, size_t offset, size_t size)

void *zmalloc(size_t size) {
	if (zmem_chunks_num == zmem_chunk_capacity) {
		if (zmem_chunk_capacity == 0) {
			zmem_chunk_capacity = 16;
			zmem_chunks = zi_malloc(zmem_chunk_capacity * sizeof(zmem_chunk_t));
		} else {
			zmem_chunk_capacity = (size_t)ceil(zmem_chunk_capacity * zmem_realloc_factor);
			zmem_chunks = zi_realloc(zmem_chunks, zmem_chunk_capacity * sizeof(zmem_chunk_t));
		}
		zmem_chunks = zi_realloc(zmem_chunks, zmem_chunk_capacity * sizeof(zmem_chunk_t));
	}
	zmem_chunk_t* chunk = &zmem_chunks[zmem_chunks_num];
	chunk->size = size;
	chunk->data = zi_malloc(size);
	chunk->allocation_trace.trace = zi_malloc(64 * sizeof(void*));
	chunk->allocation_trace.size = get_intermediate_trace(chunk->allocation_trace.trace, 64);
	chunk->num_reallocs = 0;
	chunk->realloc_traces = NULL;
	zmem_chunks_num++;
	return chunk->data;
}

void *zrealloc(void *ptr, size_t size) {
	if (ptr == NULL) {
		return zmalloc(size);
	}
	size_t entry = zi_find_entry(ptr);
	if (entry == UINT64_MAX) {
		zlog_warn("Stack trace for realloc call:");
		void* stack[64];
		get_intermediate_trace(stack, 64);
		char buffer[1024];
		sprint_intermediate_trace(stack, buffer, 0, 64);
		zlog_warn(buffer);
		zlog_fatal("zrealloc: Attempted to realloc a pointer that was not allocated by zmalloc");
	}
	zmem_chunk_t* chunk = &zmem_chunks[entry];
	chunk->num_reallocs++;
	chunk->realloc_traces = zi_realloc(chunk->realloc_traces, chunk->num_reallocs * sizeof(trace_data_t));
	chunk->realloc_traces[chunk->num_reallocs - 1].trace = zi_malloc(64 * sizeof(void*));
	chunk->realloc_traces[chunk->num_reallocs - 1].size = get_intermediate_trace(chunk->realloc_traces[chunk->num_reallocs - 1].trace, 64);
	chunk->data = zi_realloc(chunk->data, size);
	chunk->size = size;
	return chunk->data;
}

void *zcalloc(size_t nmemb, size_t size) {
	void* ptr = zmalloc(nmemb * size);
	zmemzero(ptr, nmemb * size);
	return ptr;
}

void zfree(void *ptr) {
	size_t entry = zi_find_entry(ptr);
	if (entry == UINT64_MAX) {
		zlog_warn("Stack trace for free call:");
		void* stack[64];
		get_intermediate_trace(stack, 64);
		char buffer[1024];
		sprint_intermediate_trace(stack, buffer, 0, 64);
		zlog_warn(buffer);
		zlog_fatal("zfree: Attempted to free a pointer that was not allocated by zmalloc");
	}
	zmem_chunk_t* chunk = &zmem_chunks[entry];
	zi_free(chunk->data);
	zi_free(chunk->allocation_trace.trace);
	for (size_t i = 0; i < chunk->num_reallocs; i++) {
		zi_free(chunk->realloc_traces[i].trace);
	}
	zi_free(chunk->realloc_traces);
	for (size_t i = entry; i < zmem_chunks_num - 1; i++) {
		zmem_chunks[i] = zmem_chunks[i + 1];
	}
	zmem_chunks_num--;
}


void zmemzero(void *ptr, size_t size) {
	zmemfill(ptr, size, 0);
}

void zmemfill(void *ptr, size_t size, uint8_t byte) {
	memset(ptr, byte, size);
}

void zmemcpy(void *dst, const void *src, size_t size) {
	memcpy(dst, src, size);
}

void zmem_print_stats(void) {
	zlog_info("zmem statistics:");
	zlog_info("Number of allocated chunks: %zu", zmem_chunks_num);
	for (size_t i = 0; i < zmem_chunks_num; i++) {
		zlog_info("Chunk %zu:", i);
		zlog_info("Size: %zu", zmem_chunks[i].size);
		zlog_info("Data: %p", zmem_chunks[i].data);
		zlog_info("Allocation trace:");
		char buffer[1024];
		sprint_intermediate_trace(zmem_chunks[i].allocation_trace.trace, buffer, 0, zmem_chunks[i].allocation_trace.size);
		zlog_info(buffer);
		if (zmem_chunks[i].num_reallocs == 0) {
			continue;
		}
		zlog_info("Number of reallocs: %zu", zmem_chunks[i].num_reallocs);
		for (size_t j = 0; j < zmem_chunks[i].num_reallocs; j++) {
			zlog_info("Realloc %zu trace:", j);
			sprint_intermediate_trace(zmem_chunks[i].realloc_traces[j].trace, buffer, 0, zmem_chunks[i].realloc_traces[j].size);
			zlog_info(buffer);
		}
	}
}
