#ifndef ZLOG_H
#define ZLOG_H

typedef enum z_log_level_t {
	Z_LOG_LEVEL_DEBUG,
	Z_LOG_LEVEL_INFO,
	Z_LOG_LEVEL_WARN,
	Z_LOG_LEVEL_ERROR,
	Z_LOG_LEVEL_FATAL
} z_log_level_t;

typedef void (zlog_fatal_callback)(void* data);

void zlog_set_level(z_log_level_t level);
void zlog_disable_color(void);

void zlog_debug(const char *fmt, ...);
void zlog_info(const char *fmt, ...);
void zlog_warn(const char *fmt, ...);
void zlog_error(const char *fmt, ...);
void zlog_fatal(const char *fmt, ...);
void zlog_fatal_with_callback(zlog_fatal_callback *callback, void* data, const char *fmt, ...);

#endif // ZLOG_H
