#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <errno.h>
#include <time.h>
#include <sys/types.h>
#include <unistd.h>
#include <syslog.h>

#include "debug.h"

#define DEBUG_BUFFER_SIZE 128

#define COLOR_RED     0
#define COLOR_GREEN   1
#define COLOR_YELLOW  2
#define COLOR_WHITE   3
#define COLOR_DEFAULT 4

typedef enum
{
  DEBUG_OUTPUT_NONE = 0x0,
  DEBUG_OUTPUT_FILE = 0x1,
  DEBUG_OUTPUT_SYSLOG = 0x02
} debug_output_t;

typedef struct
{
  unsigned int debug_output;
  FILE *std_file_ptr;
  FILE *output_file_ptr;
  unsigned int output_file_severity;
  const unsigned int std_severity;
} debug_conf_t;

static const char *colors[] = { "\033[31m", "\033[32m", "\033[33m", "\033[37m", "\033[0m" };

static debug_conf_t debug_conf = {
  .debug_output = 0,
  .std_file_ptr = NULL,
  .output_file_ptr = NULL,
  .output_file_severity = (unsigned int) DEBUG_SEVERITY_NONE,
  .std_severity = (unsigned int) (DEBUG_SEVERITY_ERROR | DEBUG_SEVERITY_WARNING | DEBUG_SEVERITY_INFO),
};


static void DBG_WriteFileSyslog(debug_severity_t severity, char *str)
{
  if (((debug_conf.debug_output & DEBUG_OUTPUT_SYSLOG) == DEBUG_OUTPUT_SYSLOG) ||
      (((debug_conf.debug_output & DEBUG_OUTPUT_FILE) == DEBUG_OUTPUT_FILE) &&
       (debug_conf.output_file_ptr != NULL)))
  {
    char buffer[DEBUG_BUFFER_SIZE] = { 0 };

    if ((debug_conf.output_file_severity & DEBUG_SEVERITY_INFO) == DEBUG_SEVERITY_INFO)
    {
      snprintf(buffer, sizeof(buffer), "[INFO] %s", str);
    }

    if ((debug_conf.output_file_severity & DEBUG_SEVERITY_WARNING) == DEBUG_SEVERITY_WARNING)
    {
      snprintf(buffer, sizeof(buffer), "[WARNING] %s", str);
    }

    if ((debug_conf.output_file_severity & DEBUG_SEVERITY_ERROR) == DEBUG_SEVERITY_ERROR)
    {
      snprintf(buffer, sizeof(buffer), "[ERROR] %s", str);
    }

    if (strlen(buffer) > 0)
    {
      if ((debug_conf.debug_output & DEBUG_OUTPUT_SYSLOG) == DEBUG_OUTPUT_SYSLOG)
      {
        int priority = (severity == DEBUG_SEVERITY_INFO ? LOG_NOTICE : severity == DEBUG_SEVERITY_WARNING ? LOG_WARNING : LOG_ERR);

        syslog(priority, "%s", buffer);
      }

      if (((debug_conf.debug_output & DEBUG_OUTPUT_FILE) == DEBUG_OUTPUT_FILE) &&
          (debug_conf.output_file_ptr != NULL))
      {
        time_t raw_time;
        struct tm *t = NULL;

        time(&raw_time);
        t = localtime(&raw_time);

        fprintf(debug_conf.output_file_ptr, "%02d.%02d.%04d %02d:%02d:%02d [%d]: %s", t->tm_mday, t->tm_mon + 1, t->tm_year + 1900, t->tm_hour,
            t->tm_min, t->tm_sec, (int) getpid(), buffer);
        if (buffer[strlen(buffer) - 1] != '\n')
          fprintf(debug_conf.output_file_ptr, "\n");
        fflush(debug_conf.output_file_ptr);
      }
    }
  }

}

/*
 * Writes debug string with given severity to standard output (if enabled)
 */
void DBG_Write(debug_severity_t severity, const char *format, ...)
{
  va_list ap;
  char buffer[DEBUG_BUFFER_SIZE];

  va_start(ap, format);
  vsnprintf(buffer, DEBUG_BUFFER_SIZE, format, ap);
  va_end(ap);

  switch (severity)
  {
    case DEBUG_SEVERITY_ERROR:
      {
        if ((severity & debug_conf.std_severity) && (debug_conf.std_file_ptr != NULL))
        {
          fprintf(debug_conf.std_file_ptr, "%s%s%s", colors[COLOR_RED], buffer, colors[COLOR_DEFAULT]);
          fflush(debug_conf.std_file_ptr);
        }
        break;
      }
    case DEBUG_SEVERITY_WARNING:
      {
        if (severity & debug_conf.std_severity)
        {
          fprintf(stdout, "%s%s%s", colors[COLOR_YELLOW], buffer, colors[COLOR_DEFAULT]);
          fflush(stdout);
        }
        break;
      }
    case DEBUG_SEVERITY_INFO:
      {
        if (severity & debug_conf.std_severity)
        {
          fprintf(stdout, "%s%s%s", colors[COLOR_GREEN], buffer, colors[COLOR_DEFAULT]);
          fflush(stdout);
        }
        break;
      }
    default:
      {
        break;
      }
  }

  DBG_WriteFileSyslog(severity, buffer);
}

/*
 * Converts errno to string
 */
const char *errno_to_string(int error_no)
{
  switch (error_no)
  {
    case EFAULT       : return "EFAULT";
    case EACCES       : return "EACCES";
    case EBADF        : return "EBADF";
    case EMFILE       : return "EMFILE";
    case ENFILE       : return "ENFILE";
    case ENOENT       : return "ENOENT";
    case ENOMEM       : return "ENOMEM";
    case ENOTDIR      : return "ENOTDIR";
    case EINVAL       : return "EINVAL";
    case EIO          : return "EIO";
#if ! defined (_WIN32)
    case ELOOP        : return "ELOOP";
#endif /* _WIN32 */
    case EINTR        : return "EINTR";
    case ENOSYS       : return "ENOSYS";
#if ! defined (_WIN32)
    case EOVERFLOW    : return "EVOERFLOW";
#endif /* _WIN32 */
    case ENAMETOOLONG : return "ENAMETOOLONG";
    case ERANGE       : return "ERANGE";
    default           : return "OTHER";
  }

  return "OTHER";
}

static void debug_constructor(void) __attribute__ ((constructor));
void debug_constructor(void)
{
  /* Set default error output to stderr */
  debug_conf.std_file_ptr = stderr;
}

static void debug_destructor(void) __attribute__ ((destructor));
void debug_destructor(void)
{
  if (((debug_conf.debug_output & DEBUG_OUTPUT_FILE) == DEBUG_OUTPUT_FILE) &&
    (debug_conf.output_file_ptr != NULL))
  {
    fflush(debug_conf.output_file_ptr);
    fclose(debug_conf.output_file_ptr);
  }

  if ((debug_conf.std_file_ptr != NULL) &&
      (debug_conf.std_file_ptr != stderr))
  {
    fclose(debug_conf.std_file_ptr);
  }

  if ((debug_conf.debug_output & DEBUG_OUTPUT_SYSLOG) == DEBUG_OUTPUT_SYSLOG)
  {
    closelog();
  }
}
