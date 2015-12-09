#ifndef LOG_H
#define LOG_H

enum LOG_LEVEL {
    OFF_LOG__ = -100,
    //
    PRINT_LOG__ = 0,
    INFO_LOG__ = 10,
    ERROR_LOG__ = 20,
    WARN_LOG__ = 30,
    DEBUG_LOG__ = 40,
    ALL_LOG__ = 100,
    NEVER_LOG__ = 101
};

void log_init(enum LOG_LEVEL level, const char *log_filename);
void log_print(enum LOG_LEVEL level, const char *format, ...);
void log_set_level(enum LOG_LEVEL level);
void log_exit(void);

#endif //LOG_H
