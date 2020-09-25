#include <stdio.h>
#include <stdlib.h>
#include "logger.h"

int main(int argc, char **argv)
{
    if(argc!=2){
        printf("Error, missing parameters\n");
        return 0;
    }

    printf("%d\n", atoi(argv[1]));
    switch (atoi(argv[1]))
    {
    case 1:
        // default logging
        infof("INFO Message %d", 1);
        warnf("WARN Message %d", 2);
        errorf("ERROR Message %d", 2);
        break;
    case 2:
        // stdout logging
        initLogger("stdout");
        infof("INFO Message %d", 1);
        warnf("WARN Message %d", 2);
        errorf("ERROR Message %d", 2);
        panicf(" %s\n", "WE ARE DOOMED!");
        break;
    case 3:
        // syslog logging
        initLogger("syslog");
        infof("INFO Message %d", 1);
        warnf("WARN Message %d", 2);
        errorf("ERROR Message %d", 2);
        
        break;
    default:
        errorf("Invalid test case");
        break;
    }

    return 0;
}