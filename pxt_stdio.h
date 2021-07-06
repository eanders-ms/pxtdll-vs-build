#ifndef __PXT_STDIO_H__
#define __PXT_STDIO_H__

#include <stdarg.h>

int vasprintf(char** strp, const char* fmt, va_list ap);
int asprintf(char** strp, const char* fmt, ...);

#endif
