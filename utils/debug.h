#ifndef DEBUG_H
#define DEBUG_H

#include <stdio.h>

typedef enum debug_severity_e
{
  DEBUG_SEVERITY_NONE = 0x0,
  DEBUG_SEVERITY_ERROR = 0x01,
  DEBUG_SEVERITY_WARNING = 0x02,
  DEBUG_SEVERITY_INFO = 0x04
} debug_severity_t;

#define DBG_Error(x, ...) DBG_Write(DEBUG_SEVERITY_ERROR, x, __VA_ARGS__)
#define DBG_Warning(x, ...) DBG_Write(DEBUG_SEVERITY_WARNING, x, __VA_ARGS__)
#define DBG_Info(x, ...) DBG_Write(DEBUG_SEVERY_INFO, x, __VA_ARGS__)

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

void DBG_Write(debug_severity_t, const char *format, ...);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* DEBUG_H */
