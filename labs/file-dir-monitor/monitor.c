#include <stdio.h>
#include <stdlib.h>
#include "logger.h"
#include <ftw.h>
#include <string.h>
#include <stdint.h>
#include <sys/inotify.h>
#include <unistd.h>

#define MONITOR_LEN 2048
#define NAME_LEN 32
#define EVENT_SIZE (sizeof(struct inotify_event))
#define BUFFER_LEN (MONITOR_LEN*(EVENT_SIZE+NAME_LEN))
void monitor(char* directory);
int main(int argc, char **argv)
{

    if(argc < 2){
        errorf("Missing parameters \n");
        return 0;
    }else if(argc>2){
        errorf("Too many parameters\n Correct form example: ➜ ./monitor /home/user/Documetns/myDir");
        return 0;
    }
    monitor(argv[1]);
    return 0;
}

void monitor(char* directory){
    int watch_desc;
    char buffer[BUFFER_LEN];
    int fd = inotify_init();
    if(fd<0){
        errorf("Failed to initialize nofity");
        return ;
    }
    watch_desc = inotify_add_watch(fd, directory, IN_CREATE|IN_MODIFY|IN_DELETE|IN_MOVED_TO|IN_MOVED_FROM);
    if(watch_desc == -1){
        errorf("Failed when add watch to %s", directory);
        return;
    }else{
        infof("Starting File/Directory Monitor on %s", directory);
    }
    int i = 0;
    for (;;)
    {
        i = 0;
        char *fileRenamed;
        int readBy = read(fd,buffer, BUFFER_LEN);
        if(readBy<0){
            errorf("Failed to read() in %s\n---------------------------------------------\n",directory);
        }
        while (i<readBy)
        {
            struct inotify_event *event = ( struct inotify_event * ) &buffer[ i ];
            if(event->len){
                if(event->mask & IN_CREATE){
                    if(event->mask & IN_ISDIR){
                        infof("- [Directory - Create] - %s", event->name);
                    }else{
                        infof("- [File - Create] - %s", event->name);
                    }
                }
                if(event->mask & IN_MODIFY){
                    if(event->mask & IN_ISDIR){
                        infof("- [Directory - Modified] - %s", event->name);
                    }else{
                        infof("- [File - Modified] - %s", event->name);
                    }
                }
                if(event->mask & IN_DELETE){
                    if(event->mask & IN_ISDIR){
                        infof("- [Directory - Removal] - %s", event->name);
                    }else{
                        infof("- [File - Removal] - %s", event->name);
                    }
                }
                if(event->mask & IN_MOVED_TO){
                    if(event->mask & IN_ISDIR){
                        infof("- [Directory - Rename] - %s -> %s", fileRenamed, event->name);
                        
                    }else{
                        infof("- [File - Rename] - %s -> %s", fileRenamed, event->name);
                    }
                }
                if(event->mask & IN_MOVED_FROM){
                    if(event->mask & IN_ISDIR){
                        fileRenamed=malloc(sizeof(&(event->name)));
                        fileRenamed = event->name;
                    }else{
                        
                        fileRenamed=malloc(sizeof(&(event->name)));
                        fileRenamed = event->name;
                    }
                }
                i+=EVENT_SIZE+event->len;
            }
        }
        
    }

    inotify_rm_watch(fd,watch_desc);
    close(fd);
    return;
    
}
