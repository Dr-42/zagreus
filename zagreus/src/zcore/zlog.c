#include "zcore/zlog.h"

#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>

static z_log_level_t zlog_level = Z_LOG_LEVEL_INFO;

static char* ANSI_COLOR_RESET = "\x1b[0m";
static char* ANSI_COLOR_RED = "\x1b[31m";
static char* ANSI_COLOR_YELLOW = "\x1b[33m";
static char* ANSI_COLOR_BLUE = "\x1b[34m";
static char* ANSI_COLOR_MAGENTA = "\x1b[35m";
static char* ANSI_COLOR_FATAL = "\x1b[31;1m";

void zlog_set_level(z_log_level_t level) {
	zlog_level = level;
}

void zlog_disable_color(void) {
	ANSI_COLOR_RESET = "";
	ANSI_COLOR_RED = "";
	ANSI_COLOR_YELLOW = "";
	ANSI_COLOR_BLUE = "";
	ANSI_COLOR_MAGENTA = "";
	ANSI_COLOR_FATAL = "";
}


void zlog_debug(const char *fmt, ...) {
	if (zlog_level > Z_LOG_LEVEL_DEBUG) {
		return;
	}
	va_list args;
	va_start(args, fmt);
	printf("%s[DEBUG] %s",ANSI_COLOR_MAGENTA, ANSI_COLOR_RESET);
	vprintf(fmt, args);
	va_end(args);
}

void zlog_info(const char *fmt, ...) {
	if (zlog_level > Z_LOG_LEVEL_INFO) {
		return;
	}
	va_list args;
	va_start(args, fmt);
	printf("%s[INFO] %s", ANSI_COLOR_BLUE, ANSI_COLOR_RESET);
	vprintf(fmt, args);
	va_end(args);
}

void zlog_warn(const char *fmt, ...) {
	if (zlog_level > Z_LOG_LEVEL_WARN) {
		return;
	}
	va_list args;
	va_start(args, fmt);
	printf("%s[WARN] %s", ANSI_COLOR_YELLOW, ANSI_COLOR_RESET);
	vprintf(fmt, args);
	va_end(args);
}

void zlog_error(const char *fmt, ...) {
	if (zlog_level > Z_LOG_LEVEL_ERROR) {
		return;
	}
	va_list args;
	va_start(args, fmt);
	printf("%s[ERROR] %s", ANSI_COLOR_RED, ANSI_COLOR_RESET);
	vprintf(fmt, args);
	va_end(args);
}

void zlog_fatal(const char *fmt, ...) {
	va_list args;
	va_start(args, fmt);
	printf("%s[FATAL] %s", ANSI_COLOR_FATAL, ANSI_COLOR_RESET);
	vprintf(fmt, args);
	va_end(args);
	exit(EXIT_FAILURE);
}

void zlog_fatal_with_callback(zlog_fatal_callback *callback, void* data, const char *fmt, ...) {
	va_list args;
	va_start(args, fmt);
	printf("%s[FATAL] %s", ANSI_COLOR_FATAL, ANSI_COLOR_RESET);
	vprintf(fmt, args);
	va_end(args);
	callback(data);
	exit(EXIT_FAILURE);
}
