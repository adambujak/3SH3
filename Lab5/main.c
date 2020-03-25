#include <stdio.h>
#include <unistd.h>
#include <sys/stat.h>
#include <time.h>
#include <dirent.h>
#include <string.h>
#include <stdbool.h>
#include <pwd.h>
#include <grp.h>

#define MAX_PATH_LENGTH 150 
char path[MAX_PATH_LENGTH];
// char bufferPath[150];
struct stat stats; 

bool isFilePath ( const char * filePath, struct stat * path_stat )
{
    if (stat(path, path_stat) != 0)
    {
        printf("Unable to get file properties.\n");
        printf("Please check whether '%s' file exists.\n", filePath);
        return false;
    }
    return S_ISREG(path_stat->st_mode);
}

char * getFileNameFromPath ( const char * filePath, char * fileName )
{
    int length = strlen(filePath);
    char name[MAX_PATH_LENGTH];
    int cnt = 0;
    while (filePath[length - 1 - cnt] != '/')
    {
        cnt++;
    }
    memcpy(fileName, filePath+length-cnt, cnt);
    *(fileName+cnt+1) = '\0'; 
}

void printFileData ( struct stat * fileStat, char * path ) 
{
    struct tm modifiedDate;
    if (fileStat == NULL)
        return;

    struct group *grp;
    struct passwd *usr;

    grp = getgrgid(fileStat->st_gid);
    usr = getpwuid(fileStat->st_uid);
    modifiedDate = *(gmtime(&stats.st_mtime));
    char fileName[MAX_PATH_LENGTH];
    getFileNameFromPath(path, fileName);


    printf("mode: ");
    printf((fileStat->st_mode & S_IRUSR) ? "r":"-");
    printf((fileStat->st_mode & S_IWUSR) ? "w":"-");
    printf((fileStat->st_mode & S_IXUSR) ? "x":"-");
    printf((fileStat->st_mode & S_IRGRP) ? "r":"-");
    printf((fileStat->st_mode & S_IWGRP) ? "w":"-");
    printf((fileStat->st_mode & S_IXGRP) ? "x":"-");
    printf((fileStat->st_mode & S_IROTH) ? "r":"-");
    printf((fileStat->st_mode & S_IWOTH) ? "w":"-");
    printf((fileStat->st_mode & S_IXOTH) ? "x":"-");
    printf("\n");
    printf("links: %d\n", fileStat->st_nlink);
    printf("size (bytes): %d\n", fileStat->st_size);
    printf("size (blocks): %d\n", fileStat->st_blksize);
    printf("username: %s\n", usr->pw_name);
    printf("group: %s\n", grp->gr_name);
    printf("modified: %d-%d-%d %d:%d:%d\n", modifiedDate.tm_mday, modifiedDate.tm_mon, modifiedDate.tm_year + 1900, 
                                              modifiedDate.tm_hour, modifiedDate.tm_min, modifiedDate.tm_sec);
    printf("name: %s\n", fileName);
}

void printItemData ( char * filePath ) 
{
    struct tm modifiedDate;
    struct stat fileStat; 
    
    if (lstat(filePath, &fileStat) != 0)
    {
        printf("Unable to get file properties.\n");
        printf("Please check whether '%s' file exists.\n", filePath);
        return;
    }

    struct group *grp;
    struct passwd *usr;

    grp = getgrgid(fileStat.st_gid);
    usr = getpwuid(fileStat.st_uid);
    modifiedDate = *(gmtime(&stats.st_mtime));
    


    printf((fileStat.st_mode & S_IRUSR) ? "r":"-");
    printf((fileStat.st_mode & S_IWUSR) ? "w":"-");
    printf((fileStat.st_mode & S_IXUSR) ? "x":"-");
    printf((fileStat.st_mode & S_IRGRP) ? "r":"-");
    printf((fileStat.st_mode & S_IWGRP) ? "w":"-");
    printf((fileStat.st_mode & S_IXGRP) ? "x":"-");
    printf((fileStat.st_mode & S_IROTH) ? "r":"-");
    printf((fileStat.st_mode & S_IWOTH) ? "w":"-");
    printf((fileStat.st_mode & S_IXOTH) ? "x":"-");
    printf(" %d", fileStat.st_nlink);
    printf(" %s", usr->pw_name);
    printf(" %s", grp->gr_name);
    printf(" %d", fileStat.st_size);
    printf(" %d-%d-%d %d:%d:%d ", modifiedDate.tm_mday, modifiedDate.tm_mon, modifiedDate.tm_year + 1900, 
                                              modifiedDate.tm_hour, modifiedDate.tm_min, modifiedDate.tm_sec);
    // printf(" %s", fileName);
}

void printDirectoryContents ( char * pathToPrint )
{
    DIR * dir = opendir(pathToPrint);
    if (dir == NULL)
    {
        printf("Error with directory: %s\n", pathToPrint);
        return;
    }
    struct dirent * contents; 

    while ((contents = readdir(dir)) != NULL)
    {   
        char bufpath[MAX_PATH_LENGTH];

        /* Add slash to end of path if not there */
        int length = strlen(pathToPrint);
        if (pathToPrint[length-1] != '/')
        {
            strcat(pathToPrint, "/");
        }
        snprintf(bufpath, MAX_PATH_LENGTH, "%s%s", pathToPrint, contents->d_name);
        if ( contents->d_name[0] == '.' )
        {
            continue;
        }
        printItemData(bufpath);
        printf("%s\n", contents->d_name);

        /* If directory */
        if (contents->d_type == DT_DIR)
        {
            
            
            
            printDirectoryContents(bufpath);
            printf("\n");
            
        }
        /* If file */
        else 
        {
            // printItemData()
        }
    }

    closedir(dir);
}

int main () 
{

    while (1)
    {        
        printf("Enter file or directory: ");
        scanf("%s", path);
        if ( isFilePath(path, &stats) ) 
        {
            printFileData (&stats, path); // Is file 
        }
        else 
        {
            printDirectoryContents(path); // Is directory
        }
    }

    return 0;
}

  