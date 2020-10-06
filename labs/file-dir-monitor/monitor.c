#include <stdio.h>
#include <stdlib.h>
#include "logger.h"
#include <ftw.h>
#include <string.h>
#include <stdint.h>
#include <sys/inotify.h>
#include <unistd.h>

#define EVENT_SIZE (sizeof(struct inotify_event))
#define BUFFER_LEN (1024*(EVENT_SIZE+16))
#define SIZE_DICT 69697

int fd;
void monitor(char* directory);
int addFile(const char *fpath, const struct stat *sb, int typeflag);
unsigned hash(long key);
char* getValue(int key);
void addElement(int key, char *path);
struct entry* getElement(int key);
char *fileRenamed;


struct entry{
    struct entry *next;
    long key;
    char* path;
};

static struct entry *dictionary[SIZE_DICT];

unsigned hash(long key)
{
    return (key + 101) % SIZE_DICT;
}

char* getValue(int key)
{
    struct entry *element;
    for (element = dictionary[hash(key)]; element != NULL; element = element->next) {
        if (key == element->key) {
            return element->path; 
        }
    }
    return NULL; 
}

struct entry* getElement(int key)
{
    struct entry *element;
    for (element = dictionary[hash(key)]; element != NULL; element = element->next) {
        if (key == element->key) {
            return element; 
        }
    }
    return NULL; 
}

void addElement(int key, char *path)
{ 
    struct entry *element;
    unsigned hashval;
    if ((element = getElement(key)) == NULL) { 
        element = malloc(sizeof(*element));
        hashval = hash(key);
        element->next = dictionary[hashval];
        element->key = key;
        element->path = malloc(strlen(path)+1);
        strcpy(element->path, path);
        dictionary[hashval] = element;
    }
    else {
        free(element->path);
        element->path = malloc(strlen(path)+1);
        strcpy(element->path, path);
    }
}

int main(int argc, char **argv)
{
    if(argc < 2){
        errorf("Missing parameters \n");
        return 0;
    }else if(argc>2){
        errorf("Too many parameters\n Correct form example: ➜ ./monitor /home/user/Documetns/myDir");
        return 0;
    }
    fd = inotify_init();
    if(ftw(argv[1], addFile, 20 )== -1){
        errorf("Failed when trying to monitor the directory given.\n");
        return 0;
    }
    monitor(argv[1]);
    return 0;
}


void monitor(char* directory){
    char buffer[BUFFER_LEN];
    if(fd<0){
        errorf("Failed to initialize nofity");
        return ;
    }
    int watch_desc = inotify_add_watch(fd, directory, IN_CREATE | IN_DELETE | IN_MOVE);
    if(watch_desc == -1){
        errorf("Failed when add watch to %s", directory);
        return;
    }else{
        infof("Starting File/Directory Monitor on %s\n", directory);
    }
    int i = 0;
    for (;;)
    {
        i = 0;
        int readBy = read(fd,buffer, BUFFER_LEN);
        while (i<readBy)
        {
            struct inotify_event *event = (struct inotify_event*)&buffer[i];
            char *path = getValue(event->wd); 
            if(event->len){
                if(event->mask & IN_CREATE){
                    if(event->mask & IN_ISDIR){
                        char nameFile[strlen(path) + strlen(event->name) + 1];
                        infof("- [Directory - Create] - %s", event->name);
                        strcpy(nameFile, path);
                        strcat(nameFile, "/");
                        strcat(nameFile, event->name);
                        addFile(nameFile, NULL, FTW_D);
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
                        inotify_rm_watch(fd, event->wd);
                    }else{
                        infof("- [File - Removal] - %s", event->name);
                    }
                }
                if(event->mask & IN_MOVED_TO){
                    if(event->mask & IN_ISDIR){
                        infof("- [Directory - Rename] - %s -> %s", fileRenamed, event->name);
                        fileRenamed = NULL;                        
                    }else{
                        infof("- [File - Rename] - %s -> %s", fileRenamed, event->name);
                        fileRenamed = NULL;
                    }
                }
                if(event->mask & IN_MOVED_FROM){
                    if(event->mask & IN_ISDIR){
                        fileRenamed = event->name;
                    }else{
                        
                        fileRenamed = event->name;
                    }
                }
                i+=EVENT_SIZE+ (event->len);
            }
        }
        
    }

    return;
    
}


int addFile(const char *fpath, const struct stat *sb, int typeflag){
    if(typeflag == FTW_D){
        int wd = inotify_add_watch( fd, fpath, IN_CREATE | IN_DELETE | IN_MOVE);
        addElement(wd, fpath);
    }
    return 0; 
}
