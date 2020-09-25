#include <stdio.h>
#include <stdarg.h>
#include <syslog.h>
#include <string.h>
#include <signal.h>
#include "logger.h"

#define BACK_COLOR 48
#define RESET_COLOR 37

int sysLog = 0;

void setSpecificColor(int fColor, int bColor)
{
    printf("%c[0;%d;%dm", 0x1B, fColor, bColor);
}

void setSpecificColorDefault()
{
    setSpecificColor(RESET_COLOR, BACK_COLOR);
    fflush(stdout);
}

int initLogger(char *logType)
{
    if (strcmp(logType, "syslog") == 0)
    {
        sysLog = 1;
    }
    printf("Initializing Logger on: %s\n", logType);
    return 0;
}

int infof(const char *format, ...)
{
    //set color back method [blue color]
    setSpecificColor(30, 44);
    va_list args;
    va_start(args, format);
    if (sysLog)
    {
        openlog("logger", LOG_PID | LOG_CONS, LOG_SYSLOG);
        vsyslog(LOG_INFO, "INFO:", args);
        closelog();
    }
    else
    {
        printf("%s", "INFO:");
    }
    //set color font method [blue color]
    setSpecificColor(36, 48);
    if (sysLog)
    {
        openlog("logger", LOG_PID | LOG_CONS, LOG_SYSLOG);
        vsyslog(LOG_INFO, format, args);
        closelog();
    }
    else
    {
        vprintf(format, args);
    }
    va_end(args);
    //set default color and background
    setSpecificColorDefault();
    printf("\n");
    return 0;
}

int warnf(const char *format, ...)
{
    //set color back and font method [yellow warning color]
    setSpecificColor(30, 43);
    va_list args;
    if (sysLog)
    {
        openlog("logger", LOG_PID | LOG_CONS, LOG_SYSLOG);
        vsyslog(LOG_WARNING, "WARNING : ", args);
        closelog();
    }
    else
    {
        printf("%s", "WARNING:");
    }
    //set color font method [yellow warning color]
    setSpecificColor(33, 48);
    va_start(args, format);
    if (sysLog)
    {
        openlog("logger", LOG_PID | LOG_CONS, LOG_SYSLOG);
        vsyslog(LOG_WARNING, format, args);
        closelog();
    }
    else
    {
        vprintf(format, args);
    }

    va_end(args);
    //set default color and background
    setSpecificColorDefault();
    printf("\n");
    return 1;
}

int errorf(const char *format, ...)
{
    //set color font and back method [red error color]
    setSpecificColor(30, 41);
    va_list args;
    if (sysLog)
    {
        openlog("logger", LOG_PID | LOG_CONS, LOG_SYSLOG);
        vsyslog(LOG_ERR, "ERROR :", args);
        closelog();
    }
    else
    {
        printf("%s", "ERROR:");
    }
    //set color font method [red error color]
    setSpecificColor(35, 48);
    va_start(args, format);
    if (sysLog)
    {
        openlog("logger", LOG_PID | LOG_CONS, LOG_SYSLOG);
        vsyslog(LOG_ERR, format, args);
        closelog();
    }
    else
    {
        vprintf(format, args);
    }
    va_end(args);
    //set default color and background
    setSpecificColorDefault();
    printf("\n");
    return 2;
}

int panicf(const char *format, ...)
{
    
    //set color font method [red error color]
    setSpecificColor(30, 41);
    va_list args;
    va_start(args, format);
    
    if (sysLog)
    {
        openlog("logger", LOG_PID | LOG_CONS, LOG_SYSLOG);
        vsyslog(LOG_EMERG, "PANIC :", args);
        closelog();
        
    }
    else
    {
        printf("%s", "PANIC:");
    }
    //set color font method
    setSpecificColor(30, 48);
    if (sysLog)
    {
        openlog("logger", LOG_PID | LOG_CONS, LOG_SYSLOG);
        vsyslog(LOG_EMERG, format, args);
        closelog();
        
    }
    else
    {
        vprintf(format, args);
    }
    va_end(args);
    
    //set default color and background
    setSpecificColorDefault();
    
    
    return 3;
}
