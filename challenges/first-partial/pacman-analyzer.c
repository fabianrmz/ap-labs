#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

struct Package
{
    char packageName[60];
    char installDate[25];
    char updateDate[25];
    int updateTimes;
    char removalDate[25];
    struct Package *next;
};

void analizeLog(char *logFile, char *report);

int main(int argc, char **argv)
{
    if (argc != 5)
    {
        printf("Usage:./pacman-analizer.o \n");
        printf("Missing parameters \n");
        return 0;
    }

    analizeLog(argv[2], argv[4]);

    return 0;
}

char *toLower(char *s)
{
    for (char *p = s; *p; p++)
        *p = tolower(*p);
    return s;
}
void copyTo(char newValue[], char variable[]){
    int c = 0;
    while (newValue[c]!='\0')
    {
        char a = newValue[c];
        variable[c]=a;
        c++;
    }
    variable[c]='\0';
    return ;
}

void writeReport(char *report, struct Package *node, int installedCounter, int removeCounter, int upgradeCounter, int currentInstalled){
    int fw = open(report, O_CREAT | O_WRONLY, 0600);
    if(fw == -1){
        printf("Failed to create the report \n");
        return ;
    }
    write(fw, "Pacman Packages Report\n", 23);
    write(fw, "---------------------------\n", 28);
    write(fw, "- Installed packages : ", 23);
    char *buf = malloc(sizeof(char) * 5);
    sprintf(buf, "%d", installedCounter);
    write(fw, buf, strlen(buf));
    write(fw, "\n- Removed packages   : ", 24);
    sprintf(buf, "%d", removeCounter);
    write(fw, buf, strlen(buf));
    write(fw, "\n- Upgraded packages  : ", 24);
    sprintf(buf, "%d", upgradeCounter);
    write(fw, buf, strlen(buf));
    write(fw, "\n- Current installed  : ", 24);
    sprintf(buf, "%d", currentInstalled);
    write(fw, buf, strlen(buf));
    write(fw, "\n\nList of packages\n------------------------------\n", 50);
    while (node != NULL)
    {
        sprintf(buf, "- Package Name        : %s\n", node->packageName);
        write(fw, buf, strlen(buf));
        sprintf(buf, "  - Install date      : %s\n", node->installDate);
        write(fw, buf, strlen(buf));
        sprintf(buf, "  - Last update date  : %s\n", node->updateDate);
        write(fw, buf, strlen(buf));
        sprintf(buf, "  - How many updates  : %d\n", node->updateTimes);
        write(fw, buf, strlen(buf));
        sprintf(buf, "  - Removal date      : %s\n", node->removalDate);
        write(fw, buf, strlen(buf));
        node = node->next;
    }
    close(fw);
    return ; 
}

void addInstalled(char date[], char packageName[], struct Package **head)
{
    struct Package *node = (struct Package *)malloc(sizeof(struct Package));
    node->next = NULL;
    copyTo(date, (node->installDate));
    copyTo(packageName, (node->packageName));
    copyTo("-\0", (node->removalDate));
    node->updateTimes = 0;
    if (*head == NULL)
    {
        *head = node;
    }
    else
    {
        struct Package *current = *head;
        while (current->next != NULL)
        {
            current = current->next;
        }
        current->next = node;
    }
    return;
}
int areEqualStrings(char  a[], char b[]){
    int c = 0;
    while (a[c]!='\0' )
    {
        if(a[c]!=b[c]){
            return 0;
        }
        c++;
    }
    return 1;
}
void addRemoved(char date[], char packageName[], struct Package **head)
{
    if (*head == NULL)
    {
        return ;
    }
    else
    {
        struct Package *current = *head;
        while (current->next != NULL)
        {
            if(areEqualStrings(current->packageName, packageName)){
                printf("Removed existing package: %s\n", current->packageName);
                copyTo(date, current->removalDate);
                return ;
            }
            current = current->next;
        }
    }
    return;
}
void addUpgrade(char date[], char packageName[], struct Package **head)
{
    if (*head == NULL)
    {
        return ;
    }
    else
    {
        struct Package *current = *head;
        while (current->next != NULL)
        {
            if(areEqualStrings(current->packageName, packageName)){
                int a = current->updateTimes;
                current->updateTimes= a+1;
                copyTo(date, current->updateDate);
                return ;
            }
            current = current->next;
        }
    }
    return;
}

void analizeLog(char *logFile, char *report)
{
    struct Package *head = NULL;
    printf("Generating Report from: [%s] log file\n", logFile);
    //Solution
    int fd;
    int rd;
    char buffer[2];
    buffer[1] = '\0';
    int counter = -1;
    char word[150];
    char date[25];
    int flag = 0;
    int spaceCount = 0;
    int saveDate;
    char keyWord[10];
    int countWord = 0;
    int startSavingPackage = 0;
    char packageName[60];
    int counterPackage = 0;
    int installedCounter = 0;
    int upgradeCounter = 0;
    int removeCounter = 0;
    fd = open(logFile, O_RDONLY);
    if (fd == -1)
    {
        printf("Error, Failed to open the file, are you sure %s exists?\n", logFile);
        return;
    }
    printf("Open file succesful\n");
    //read function
    while (rd = read(fd, buffer, 1) > 0)
    {
        if (buffer[0] == '\n')
        {
            /*The key words are: installed, upgraded, removed*/
            toLower(keyWord);
            int equal ;
            equal = areEqualStrings("installed\0", keyWord);
            if (equal)
            {
                addInstalled(date, packageName, &head);
                installedCounter++;
            }
            else if (equal = areEqualStrings("upgraded\0", keyWord))
            {
                addUpgrade(date, packageName,&head);
                upgradeCounter++;
            }
            else if (areEqualStrings("removed\0", keyWord))
            {
                addRemoved(date, packageName, &head);
                removeCounter++;
                //printf("- %d ", removeCounter);
            }

            date[0] = '\0';
            keyWord[0] = '\0';
            packageName[0] = '\0';
            flag = 0;
            counter = -1;
            countWord = 0;
            spaceCount = 0;
            counterPackage = 0;
            startSavingPackage = 0;
        }
        if (buffer[0] == '[' || buffer[0] == ']')
        {
            flag++;
        }
        saveDate = flag == 1 ? 1 : 0;
        if (saveDate)
        {
            date[counter] = buffer[0];
            date[counter + 1] = '\0';
            counter++;
        }

        if (startSavingPackage)
        {
            packageName[counterPackage] = buffer[0];
            packageName[counterPackage + 1] = '\0';
            counterPackage++;
        }
        if (flag == 4 && buffer[0] == ' ')
        {
            spaceCount++;
        }
        else if (flag == 4 && spaceCount < 2 && buffer[0] != ']')
        {
            keyWord[countWord] = buffer[0];

            countWord++;
        }
        startSavingPackage = spaceCount == 2 ? 1 : 0;
    }
    close(rd);
    close(fd);
    printf("\nPacman Packages Report\n");
    printf("----------------------\n");
    printf("- Installed packages : %d\n", installedCounter);
    printf("- Removed packages   : %d\n", removeCounter);
    printf("- Upgraded packages  : %d\n", upgradeCounter);
    printf("- Current installed  : %d\n", installedCounter - removeCounter);
    printf("\nReport is generated at: [%s]\n", report);
    writeReport(report, head, installedCounter, removeCounter, upgradeCounter, installedCounter-removeCounter);
}