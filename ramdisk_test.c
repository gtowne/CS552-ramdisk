#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>

#ifdef USE_PTHREADS
#include "ramdisk.h"
struct Ramdisk* RAMDISK;
struct FdtableArray fdtablea;
#else
#include "ramdisk_lib.h"
#endif

int writeCookieSong(int fd, int times);
int writeLogo(int fd, int times);
int writeRaptor(int fd, int times);
int more(char* path, int seek);
int testSize(char* path);
int ls(char* path);

int testDirectoryDirect();
int testDirectoryIndirect();

int testFileDirect();
int testFileIndirect();
int testFileDoubleIndirect();
int testFileMax();

int stressFilesystem();
int testErrors();

int fillFilesystem();
int cleanFilesystem();

int main(int argc, char* argv[])
{
    if(argc < 2)
    {
        printf("./ramdisk_test \n");
        printf("\t 0 test creating entries in a directory using only direct pointers \n");
        printf("\t 1 test creating entries in a directory using direct and indirect pointers \n");
        printf("\t 2 test writing data to a file using only direct pointers \n");
        printf("\t 3 test writing data to a file using direct and indirect pointers \n");
        printf("\t 4 test writing data to a file using direct, indirect, and double indirect pointers \n");
        printf("\t 5 test writing maximum amount of data to file and filesystem \n");
        printf("\t 6 test stressing directory and file creation and unlinking \n");
        printf("\t 7 test error cases\n");

        return 0;
    }


#ifdef USE_PTHREADS
    RAMDISK = malloc(sizeof(struct Ramdisk));
    _ramdisk_initialize(RAMDISK);
#else
    rd_init();
#endif

    int testNum = atoi(argv[1]);

    switch(testNum)
    {
        //create a directory that requires more than one block
        //(use all of the direct pointers)
    case 0:
        testDirectoryDirect();
        break;

        //now, use some of the indirect pointers
        //(and max out the inodes)
    case 1:
        testDirectoryIndirect();
        break;

        //reading and writing in the direct pointer blocks
    case 2:
        testFileDirect();
        break;

        //reading and writing in the single indirect blocks
    case 3:
        testFileIndirect();
        break;

        // Test the double indirect pointers
    case 4:
        testFileDoubleIndirect();
        break;

        //try using up all the blocks in the filesystem
    case 5:
        testFileMax();
        break;
        //stress test creating directories, subdirectories, regular files,
        //writing data, and unlinking
    case 6:
        stressFilesystem();
        break;

        //TESTS THAT SHOULD FAIL
    case 7:
        testErrors();
        break;

    default:
        printf("unknown test identifier\n");
    }

    printf("This LS should be empty\n");
    ls("/");

    return 0;
}

int testDirectoryDirect()
{
    int retval, fd;
    int i;
    int numdirectories=128;
    char name[100] = "/aaaa";

    for(i=0; i<numdirectories; i++)
    {
        sprintf(name, "/a%d", i);
        int q = rd_creat(name);
    }
    //test readdir
    printf("Created enough directories to fill all the direct pointers in the root directory. Should 128 entries listed\n");
    ls("/");


    for(i=0; i<numdirectories; i++)
    {
        sprintf(name, "/a%d", i);
        rd_unlink(name);
    }
    printf("unlinked all 128 directories. The next LS should be empty.\n");
    ls("/");
    return 0;
}


int testDirectoryIndirect()
{
    int retval, fd;
    int i;
    int numdirectories=1023;
    char name[100] = "/aaaa";

    numdirectories=1023;
    for(i=0; i<numdirectories; i++)
    {
        sprintf(name, "/a%d", i);
        int q = rd_creat(name);
        if(q < 0)
        {
            printf("TEST ERROR: Failed to create file %s\n", name);
        }
    }
    //test readdir
    printf("Created enough directories to use all of the inodes. Should be 1023 entries listed\n");
    ls("/");

    for(i=0; i<numdirectories; i++)
    {
        sprintf(name, "/a%d", i);
        rd_unlink(name);
    }
    printf("unlinked all 1023 directories. The next LS should be empty\n");
    ls("/");

    return 0;
}

int testFileDirect()
{
    int fd;
    printf("creating file to test correct writing to all of the direct blocks\n");
    rd_creat("/cookies.txt");
    int repeats=4;
    fd = rd_open("/cookies.txt");
    int bytesIn, bytesOut;
    bytesIn = writeCookieSong(fd, repeats);
    rd_close(fd);
    bytesOut = more("/cookies.txt", 0);
    if(bytesIn != bytesOut)
    {
        printf("TEST ERROR: Cookie Song %d %d\n", bytesIn, bytesOut);
    }
    else
    {
        printf("TEST PASSED: Cookie Song\n");
    }
    rd_close(fd);
    rd_unlink("/cookies.txt");
    return 0;
}

int testFileIndirect()
{
    int fd;
    int bytesIn,bytesOut;
    //reading and writing in the single indirect blocks
    printf("creating file to test correct writing to all of the indirect blocks\n");
    rd_creat("/friends.txt");
    fd = rd_open("/friends.txt");
    bytesIn = writeLogo(fd, 10);
    rd_close(fd);
    //seek to somewhere in the indirect pointers
    bytesOut = testSize("/friends.txt");
    more("/friends.txt", 488*8);
    if(bytesIn != bytesOut)
    {
        printf("TEST ERROR: Logo (single indirect pointers)\n");
    }
    else
    {
        printf("TEST PASSED: Logo  (single indirect pointers)\n");
    }
    rd_unlink("/friends.txt");
    return 0;
}

int testFileDoubleIndirect()
{
    int fd;
    int bytesIn,bytesOut;
    // Test the double indirect pointers
    printf("testing correct writing to the double indirect pointers\n");
    rd_creat("/big.txt");
    fd = rd_open("/big.txt");
    bytesIn = writeLogo(fd, 45);
    rd_close(fd);
    bytesOut = testSize("/big.txt");
    //seek to somewhere inside the double indirect pointers
    more("/big.txt", 488*43);
    if(bytesIn != bytesOut)
    {
        printf("TEST ERROR: Logo (double indirect pointers)\n");
    }
    else
    {
        printf("TEST PASSED: Logo  (double indirect pointers)\n");
    }
    rd_unlink("/big.txt");

    return 0;
}

int testFileMax()
{
    int fd;
    int bytesIn,bytesOut, retval;

    printf("testing filling up the file system\n");
    rd_creat("/huge1.txt");
    fd=rd_open("/huge1.txt");
    bytesIn = writeLogo(fd, 2186); //488*2186 = 1066768 -> just under
    //the max number of bytes in a file
    bytesOut = testSize("/huge1.txt");
    if(bytesIn != bytesOut)
    {
        printf("TEST ERROR: Huge File 1 (max file size)\n");
    }
    else
    {
        printf("TEST PASSED: Huge File 1. Wrote %d, Checked file is size %d. File is using %d blocks, plus 66 blocks overhead. \n", bytesIn, bytesOut, (int)(ceil(bytesIn/256.0)));
    }
    rd_close(fd);

    rd_creat("/huge2.txt");
    fd=rd_open("/huge2.txt");
    retval = writeLogo(fd, 2186); //488*2186+1 = 1066769 -> just under
    //the max number of bytes in a file
    //should crap out somewhere in here b/c the file system is full.
    rd_close(fd);

    if(retval != bytesIn)
    {
        printf("TEST PASSED: File system filled. Tried to write %d bytes. Only able to write %d bytes. \n", bytesIn, retval);
    }

    rd_unlink("/huge1.txt");
    rd_unlink("/huge2.txt");

    return 0;
}

int stressFilesystem()
{
    int i,j,k;
    char name[100] = "/aaaa";
    int testfd[30];

    printf("Creating lots of files and subdirectories. Writing a small amount to each file.\n");
    fillFilesystem();
    ls("/");

    printf("\nUnlinking all of the files and subdirectories\n");
    cleanFilesystem();

    printf("unlinked files. ls should be empty\n");
    ls("/");
    return 0;
}

int testErrors()
{
    int i,j,k, fd, retval;
    char name[100] = "/aaaa";

    printf("This function is to test error behavior\n");
    fillFilesystem();
//  ls("/");
    //test creating one more file
    printf("\nTest to create the 1025th file\n");
    retval = rd_creat("/too_many");
    if(retval == -1)
    {
        printf("TEST PASSED: failed to create file. This is the expected behavior.\n");
    }
    else
    {
        printf("TEST ERROR: file creation should have failed\n");
    }

    //test trying to create a file that already exists
    printf("\ntest to create file that already exists\n");
    sprintf(name, "/a%d", 0);
    retval = rd_creat(name);
    if(retval == -1)
    {
        printf("TEST PASSED: failed to create file. This is the expected behavior.\n");
    }
    else
    {
        printf("TEST ERROR: file creation should have failed\n");
    }

    printf("\ntest to create a file in a subdirectory that already exists\n");
    sprintf(name, "/a%d/b%d/c%d", 1,1,1);
    if(retval == -1)
    {
        printf("TEST PASSED: failed to create file. This is the expected behavior.\n");
    }
    else
    {
        printf("TEST ERROR: file creation should have failed\n");
    }

    //test unlinking a non-empty directory
    printf("\ntest to unlink a non-empty directory\n");
    sprintf(name, "/a%d", 1,1);
    retval = rd_unlink(name);
    if(retval == -1)
    {
        printf("TEST PASSED: Failed to unlink directory. This is the expected behavior\n");
    }
    else
    {
        printf("TEST ERROR: Should have failed unlinking.\n");
    }

    //test unlinking an open file
    printf("\ntest to unlink an open file\n");
    sprintf(name, "/a%d/b%d/c%d", 1,1,1);
    fd = rd_open(name);
    retval = rd_unlink(name);
    if(retval == -1)
    {
        printf("TEST PASSED: Failed to unlink directory. This is the expected behavior\n");
    }
    else
    {
        printf("TEST ERROR: Should have failed unlinking. %d %d\n", fd, retval);
    }
    rd_close(fd);

    //test unlinking nonexistant file
    printf("\ntest to unlink nonexistant file\n");
    retval = rd_unlink("/nonexistant");
    if(retval == -1)
    {
        printf("TEST PASSED: Failed to unlink directory. This is the expected behavior\n");
    }
    else
    {
        printf("TEST ERROR: Should have failed unlinking.\n");
    }


    //test opening a non-existant file
    printf("\ntest to open a nonexistant file\n");
    retval = rd_open("/nonexistant");
    if(retval == -1)
    {
        printf("TEST PASSED: Failed to open nonexistant file. This is the expected behavior\n");
    }
    else
    {
        printf("TEST ERROR: Should not be able to open a nonexistant file\n");
    }

    //test reading from a directory
    printf("\ntest rd_read from a directory\n");
    sprintf(name, "/a%d/b%d", 1,1);
    fd = rd_open(name);
    retval = rd_read(fd, name, 16);
    if(retval == -1)
    {
        printf("TEST PASSED: Unable to rd_read from a directory. This is the expected behavior\n");
    }
    else
    {
        printf("TEST FAILED: Should not be able to rd_read from a directory.\n");
    }
    rd_close(fd);

    //test readdir from a file
    printf("\ntest rd_readdir from a file\n");
    sprintf(name, "/a%d/b%d/c%d", 1,1,1);
    fd = rd_open(name);
    retval = rd_readdir(fd, name);
    if(retval == -1)
    {
        printf("TEST PASSED: Unable to rd_readdir from a file. This is the expected behavior\n");
    }
    else
    {
        printf("TEST FAILED: Should not be able to rd_readdir from a file.\n");
    }
    rd_close(fd);


    //test closing a non-open file descriptor
    printf("\ntest to close a non-open file descriptor\n");
    retval = rd_close(42);
    if(retval == -1)
    {
        printf("TEST PASSED: failed to close file. This is the expected behavior\n");
    }
    else
    {
        printf("TEST ERROR: should not have closed file successfully\n");
    }

    //test double-closing a file descriptor
    printf("\ntest to close a double closing file descriptor\n");
    sprintf(name, "/a%d/b%d/c%d", 1,1,1);
    fd = rd_open(name);
    retval = rd_close(fd);
    retval = rd_close(fd);
    if(retval == -1)
    {
        printf("TEST PASSED: failed to close file. This is the expected behavior\n");
    }
    else
    {
        printf("TEST ERROR: should not have closed file successfully\n");
    }

    cleanFilesystem();
    return 0;
}

int fillFilesystem()
{
    int i,j,k, fd;
    char name[100] = "/aaaa";
    int testfd[30];

    for(i=0; i<4; i++)
    {
        sprintf(name, "/a%d", i);
        rd_mkdir(name);
        for(j=0; j<8; j++)
        {
            sprintf(name, "/a%d/b%d", i,j);
            rd_mkdir(name);
            for(k=0; k<30; k++)
            {
                //this should do 996 files (+ 1 for the root)
                //4 + 4*8 + 4*8*30
                sprintf(name, "/a%d/b%d/c%d", i,j,k);
                rd_creat(name);
                testfd[k]=rd_open(name);
                printf("opened file: %s %d\n", name, testfd[k]);
                writeCookieSong(testfd[k], 1);
            }
            for(k=0; k<30; k++)
            {
                rd_close(testfd[k]);
                sprintf(name, "/a%d/b%d/c%d", i,j,k);
                int size = testSize(name);
                if(size != 176)
                {
                    printf("TEST ERROR: FS Stress Test. File has incorrect size\n");
                }
            }
            sprintf(name, "/a%d/b%d", i,j);
            ls(name);
        }
        sprintf(name, "/a%d", i);
        ls(name);
    }

    //create and extra 27 files, to max out the file system

    for(i=0; i<27; i++)
    {
        sprintf(name, "/d%d", i);
        rd_creat(name);
        fd=rd_open(name);
        writeCookieSong(fd, 1);
        rd_close(fd);
    }
    return 0;
}

int cleanFilesystem()
{
    int i,j,k, fd;
    char name[100] = "/aaaa";
    int testfd[30];

    for(i=0; i<27; i++)
    {
        sprintf(name, "/d%d", i);
        rd_unlink(name);
    }

    for(i=0; i<4; i++)
    {
        for(j=0; j<8; j++)
        {
            for(k=0; k<30; k++)
            {
                sprintf(name, "/a%d/b%d/c%d", i,j,k);
                rd_unlink(name);
            }
            sprintf(name, "/a%d/b%d", i,j);
            rd_unlink(name);
        }
        sprintf(name, "/a%d", i);
        rd_unlink(name);
    }
    for(i=0; i<27; i++)
    {
        sprintf(name, "/d%d", i);
        rd_unlink(name);
    }
    return 0;
}






int ls(char* path)
{
    printf("This is LS\n");

    char buffer[16];
    int fd = rd_open(path);
    if(fd < 0)
    {
        return -1;
    }

    while(rd_readdir(fd, buffer) > 0)
    {
        printf("%s/%s\n", path, buffer);
    }
    fd = rd_close(fd);
    return 0;
}




int more(char* path, int seek)
{
    printf("This is MORE\n");
    char buffer[1000];
    buffer[1000]='\0';
    int fd = rd_open(path);

    printf("Seeking to position: %d\n", seek);
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

int testSize(char* path)
{
    char buffer[1000];
    buffer[1000]='\0';
    int fd = rd_open(path);
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
    }
    while(retval > 0);

    rd_close(fd);

    return totalBytes;
}


int writeCookieSong(int fd, int repeats)
{
    char* string1 = "c is for cookie that's good enough for me. "; //43
    char* string2 = "oh! cookie cookie cookie starts with C. "; //40
    char string3[7] = {"-= =-\n"};
    char* null="\0";

    int retval, i;
    int totalBytes=0;

    for(i=0; i<repeats; i++)
    {
        retval = rd_write(fd, string1, 43);
        if(retval != 43)
        {
            printf("TEST ERROR (Cookie Song): Did not write required number of bytes\n");
        }
        totalBytes += retval;

        retval = rd_write(fd, string1, 43);
        if(retval != 43)
        {
            printf("TEST ERROR (Cookie Song): Did not write required number of bytes\n");
        }
        totalBytes += retval;

        retval = rd_write(fd, string1, 43);
        if(retval != 43)
        {
            printf("TEST ERROR (Cookie Song): Did not write required number of bytes\n");
        }
        totalBytes += retval;

        retval = rd_write(fd, string2, 40);
        if(retval != 40)
        {
            printf("TEST ERROR (Cookie Song): Did not write required number of bytes\n");
        }
        totalBytes += retval;

        string3[2]='0'+i;
        retval = rd_write(fd, string3, 6);
        if(retval != 6)
        {
            printf("TEST ERROR (Cookie Song): Did not write required number of bytes\n");
        }
        totalBytes += retval;
    }
    retval = rd_write(fd, null, 1);
    totalBytes += retval;
    if(retval != 1)
    {
        printf("TEST ERROR (Cookie Song): Did not write required number of bytes\n");
    }

    //printf("TEST PASSED (Cookie Song)\n");
    //return repeats*(43*3+40+6)+1;
    return totalBytes;
}

int writeLogo(int fd, int repeats)
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
    bytesWritten = rd_write(fd, null, 1);
    totalBytes += bytesWritten;
    if(bytesWritten != 1)
    {
        //printf("TEST ERROR (Logo): Did not write required number of bytes\n");
    }
    return totalBytes;
}

