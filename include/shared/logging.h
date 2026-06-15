#ifndef LOGGING_H
#define LOGGING_H

#include <stdint.h>
#include <stdio.h>
#include <stdarg.h>

static FILE *g_logFile = NULL;
static char g_logPath[512];

static void open_log_file(const char *romFilePath) {
#if defined(DEBUG_LOG)
    snprintf(g_logPath, sizeof(g_logPath), "%s.log", romFilePath);
    g_logFile = fopen(g_logPath, "w");
#endif
}

static void close_log_file(void) {
#if defined(DEBUG_LOG)
    if (g_logFile) {
        fclose(g_logFile);
        g_logFile = NULL;
    }
#endif
}

static void debug_printf(const char *format, ...) {
    va_list args;
    va_start(args, format);
#if defined(DEBUG)
    vfprintf(stderr, format, args);
#endif
#if defined(DEBUG_LOG)
    if (g_logFile) {
        vfprintf(g_logFile, format, args);
    }
#endif
    va_end(args);
}

static void write_error(const char *romFilePath, uint32_t line_num, const char *line, const char *message) {
#if defined(DEBUG_LOG)
    char logPath[512];
    snprintf(logPath, sizeof(logPath), "%s.log", romFilePath);
    FILE *logFile = fopen(logPath, "w");
    if (logFile) {
        fprintf(logFile, "ERROR at line %u:\n", line_num);
        fprintf(logFile, "  %s\n", line);
        fprintf(logFile, "  %s\n", message);
        fclose(logFile);
    }
#endif
#if defined(DEBUG)
    fprintf(stderr, "ERROR at line %u:\n", line_num);
    fprintf(stderr, "  %s\n", line);
    fprintf(stderr, "  %s\n", message);
#endif
}

#endif
