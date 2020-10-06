#include <stdio.h>
#include <stdlib.h>
#include "logger.h"
#include <ftw.h>
#include <string.h>
#include <stdint.h>
#include <sys/inotify.h>
#include <unistd.h>

#define EVENT_SIZE (sizeof(struct inotify_event))
#define BUFFER_LEN (2048 * (EVENT_SIZE + 32))
#define SIZE_DICT 69697

int fd;
void monitor();
int addFolder(const char *fpath, const struct stat *sb, int typeflag);
unsigned hash(long key);
char *getValue(int key);
void addElement(int key, char *path);
int handleEvent(struct inotify_event *event);
struct entry *getElement(int key);
char *fileRenamed;

struct entry
{
    struct entry *next;
    long key;
    char *path;
};

static struct entry *dictionary[SIZE_DICT];

unsigned hash(long key)
{
    return (key + 101) % SIZE_DICT;
}


int main(int argc, char **argv)
{
    if (argc < 2)
    {
        errorf("Missing parameters \n");
        return 0;
    }
    else if (argc > 2)
    {
        errorf("Too many parameters\n Correct form example: ➜ ./monitor /home/user/Documetns/myDir");
        return 0;
    }
    fd = inotify_init();
    if (fd < 0)
    {
        errorf("Failed initializing inotify\n");
        return 0;
    }

    if (ftw(argv[1], addFolder, 20) == -1)
    {
        errorf("Failed when trying to monitor the directory given.\n");
        return 0;
    }
    else
    {
        infof("Starting File/Directory Monitor on %s\n -----------------\n", argv[1]);
    }
    monitor();
    return 0;
}

void monitor()
{
    char buffer[BUFFER_LEN];
    if (fd < 0)
    {
        errorf("Failed to initialize nofity");
        return;
    }
    int i = 0;
    while (1)
    {
        i = 0;
        int readBy = read(fd, buffer, BUFFER_LEN);
        if (readBy < 0)
        {
            errorf("Failer when start to read\n");
            return;
        }
        while (i < readBy)
        {
            struct inotify_event *event = (struct inotify_event *)&buffer[i];
            if (event->len)
            {
                handleEvent(event);
                i += EVENT_SIZE + (event->len);
            }
        }
    }

    return;
}

int addFolder(const char* fpath, const struct stat *sb, int flag){
	if(flag == FTW_D){
		int watch_desc = inotify_add_watch(fd, fpath, IN_CREATE | IN_MODIFY | IN_DELETE | IN_MOVE);
		addElement(watch_desc, fpath);
		return 0;
	}
}


int handleEvent(struct inotify_event *event)
{
    char *path = getValue(event->wd);
    if (event->mask & IN_CREATE)
    {
        if (event->mask & IN_ISDIR)
        {
            infof("- [Directory - Create] - %s", event->name);
            char nameFile[strlen(path) + strlen(event->name) + 2]; 
            strcpy(nameFile, path);
            strcat(nameFile, "/");
            strcat(nameFile, event->name);
            addFolder(nameFile, NULL, FTW_D);
        }
        else
        {
            infof("- [File - Create] - %s", event->name);
        }
    }
    else if (event->mask & IN_MODIFY)
    {
        if (event->mask & IN_ISDIR)
        {
            infof("- [Directory - Modified] - %s", event->name);
        }
        else
        {
            infof("- [File - Modified] - %s", event->name);
        }
    }
    else if (event->mask & IN_DELETE)
    {
        if (event->mask & IN_ISDIR)
        {
            infof("- [Directory - Removal] - %s", event->name);
            inotify_rm_watch(fd, event->wd);
        }
        else
        {
            infof("- [File - Removal] - %s", event->name);
        }
    }
    else if (event->mask & IN_MOVED_TO)
    {
        if (strcmp(fileRenamed, path) == 0)
        {
            if (event->mask & IN_ISDIR)
            {
                infof("- [Directory - Rename] - %s -> %s", fileRenamed, event->name);
            }
            else
            {
                infof("- [File - Rename] - %s -> %s", fileRenamed, event->name);
            }
        }

        fileRenamed = NULL;
    }
    else if (event->mask & IN_MOVED_FROM)
    {
        fileRenamed = event->name;
    }
}

char* getValue(int key){
	struct entry *element;
	for(element = dictionary[hash(key)]; element != NULL; element = element->next){
		if(key == element->key){
			return element->path;
		}
	}

	return NULL;
}

struct entry* getElement(int key){
	struct entry *element;
	for(element = dictionary[hash(key)]; element != NULL; element = element->next){
		if(key == element->key){
			return element;
		}
	}

	return NULL;
}

void addElement(int key, char *path){
	struct entry *element;
	unsigned hashval;
	if((element = getElement(key)) == NULL){
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
