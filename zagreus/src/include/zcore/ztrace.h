#ifndef ZTRACE_H
#define ZTRACE_H

#include <stdio.h>

#define print_trace() _print_trace(3);
#define fprint_trace(fp) _fprint_trace(fp, 2);
#define sprint_trace(buffer) _sprint_trace(buffer, 1);

int get_intermediate_trace(void** stack, size_t stack_size);
int sprint_intermediate_trace(void** stack, char* buffer, size_t offset, size_t stack_size);

// Private functions
void _print_trace(size_t offset);
int _fprint_trace(FILE* fp, size_t offset);
int _sprint_trace(char* buffer, size_t offset);

#endif // ZTRACE_H
