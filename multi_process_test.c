#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <unistd.h>

#include "ramdisk_lib.h"


int ramdisk_muscle_flexer(int num_execs);

int ls(char* path)
{
    printf("This is LS in %u\n", getpid());

    char buffer[16];
    int fd = rd_open(path);
    if(fd < 0)
    {
        return -1;
    }

    while(rd_readdir(fd, buffer) > 0)
    {
        printf("%s/%s in %u\n", path, buffer, getpid());
    }
    fd = rd_close(fd);
    return 0;
}

int more(char* path, int seek)
{
    printf("This is MORE %s\n", path);
    char buffer[1000];
    buffer[1000]='\0';
    int fd = rd_open(path);
    rd_seek(fd, seek);
    if(fd < 0)
    {
        return -1;
    }

    int totalBytes=0;
    int retval;
    do
    {
        retval = rd_read(fd, buffer, 999);
        totalBytes += retval;
        buffer[retval]='\0';
        printf("%s", buffer);
    }
    while(retval > 0);

    rd_close(fd);
    return totalBytes;
}

int writeLogo(int fd, int repeats, int writeNull)
{
    char* null="\0";
    char * logo=
        " _____                     ______            _              \n\
|_   _|                    | ___ \\          | |             \n\
  | | ___  __ _ _ __ ___   | |_/ /__ _ _ __ | |_  ___  _ __ \n\
  | |/ _ \\/ _` | '_ ` _ \\  |    // _` | '_ \\| __|/ _ \\| '__|\n\
  | |  __/ (_| | | | | | | | |\\ \\ (_| | |_) | |_| (_) | |   \n\
  \\_/\\___|\\__,_|_| |_| |_| \\_| \\_\\__,_| .__/ \\__|\\___/|_|   \n\
                                      | |                   \n\
                                      |_|                   \n";


    int len = strlen(logo);
    int i, bytesWritten, retval;
    int totalBytes = 0;
    for(i=0; i<repeats; i++)
    {
        bytesWritten = rd_write(fd, logo, len);
        if(bytesWritten != len)
        {
            //printf("TEST ERROR (Logo): Did not write required number of bytes\n");
            break;
        }
        totalBytes += bytesWritten;
    }

    if(writeNull == 1)
    {
        bytesWritten = rd_write(fd, null, 1);
        totalBytes += bytesWritten;
        if(bytesWritten != 1)
        {
            //printf("TEST ERROR (Logo): Did not write required number of bytes\n");
        }
    }
    return totalBytes;
}


int main(int argc, char** argv)
{
    if (argc != 3)
    {
        printf("usage: ./multi_process_test [num_processes] [iterations]\n");

        return -1;
    }

    int numProcesses = atoi(argv[1]);

    int num_execs = atoi(argv[2]);

    rd_init();

    rd_creat("/thesharedfile");

    pid_t parentPid = getpid();
    int i;
    for (i = 0; i < numProcesses; i++)
    {
        pid_t thisPid = fork();
        if (thisPid == 0)
        {
            ramdisk_muscle_flexer(num_execs);
            break;
        }
    }

    int j;
    if (getpid() == parentPid)
    {
        for (j = 0; j < numProcesses; j++)
        {
            wait(&i);
        }
        int read = more("/thesharedfile", 0);
        printf("Unlinking the shared file\n");
        rd_unlink("/thesharedfile");
    }
}

int ramdisk_muscle_flexer(int num_execs)
{
    printf("Entering process : %u\n", getpid());

    printf("Process %u: wrote to shared file\n", getpid());
    int fdShared = rd_open("/thesharedfile");
    rd_seek(fdShared, -1);
    int written = writeLogo(fdShared, 1,0);
    rd_close(fdShared);

    char startDir[14];

    sprintf(startDir, "/p%u", getpid()%1000);
    rd_mkdir(startDir);

    ls("/");

    char fileA[28];
    char fileB[28];
    char fileC[28];

    sprintf(fileA, "%s/fileA", startDir);
    sprintf(fileB, "%s/fileB", startDir);
    sprintf(fileC, "%s/fileC", startDir);

    int j;
    for (j = 0; j < num_execs; j++)
    {
        rd_creat(fileA);
        rd_creat(fileB);
        rd_creat(fileC);

        ls(startDir);

        int fdFileA = rd_open(fileA);

        int bytesWritten = writeLogo(fdFileA, 2,1);

        int bytesRead = more(fileA, 0);

        if (bytesWritten == bytesRead)
        {
            printf("Process %u wrote and read %i bytes from fileA\n", getpid(), bytesRead);
        }
        else
        {
            printf("ERROR: Process %u bytes written: %i not equal read %i\n", getpid(),bytesWritten,  bytesRead);
        }

        rd_close(fdFileA);
        rd_unlink(fileA);
        rd_unlink(fileB);
        rd_unlink(fileC);
    }


    rd_unlink(startDir);

    ls("/");

    return 1;
}
