#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "ramdisk.h"

struct Ramdisk* RAMDISK;
struct FdtableArray fdtablea;

int writeCookieSong(int fd, int times);
int writeLogo(int fd, int times);
int writeRaptor(int fd, int times);

int main()
{
  int retval, fd;
    RAMDISK = malloc(sizeof(struct Ramdisk));

    _ramdisk_initialize(RAMDISK);

    //create a directory that requires more than one block
    //(use all of the direct pointers)
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
    printf("unlinked all 128 directories\n");


    //now, use some of the indirect pointers
    //(and max out the inodes)
    numdirectories=1023;
    for(i=0; i<numdirectories; i++)
    {
      sprintf(name, "/a%d", i);
      int q = rd_creat(name);
      printf("%d %d %s\n", i, q, name); 
    }
    //test readdir
    printf("Created enough directories to use all of the inodes. Should be 1023 entries listed\n");
    ls("/");


    for(i=0; i<numdirectories; i++)
    {
      sprintf(name, "/a%d", i);
      rd_unlink(name);
    }
    printf("unlinked all 128 directories. LS should be empty\n");
    ls("/");

    
    //stress test creating directories, subdirectories, regular files,
    //writing data, and unlinking

    printf("Creating lots of files and subdirectories. Writing a small amount to each file.\n");
    int j,k;
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

    //create and extra 31 files, to max out the file system
    for(i=0; i<27; i++)
    {
      sprintf(name, "/d%d", i);
      rd_creat(name);
      fd=rd_open(name);
      writeCookieSong(fd, 1);
      rd_close(fd);
    }


    //
    //TESTS THAT SHOULD FAIL
    //

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
    retval = unlink(name);
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
    retval = unlink(name);
    if(retval == -1)
    {
      printf("TEST PASSED: Failed to unlink directory. This is the expected behavior\n");
    }
    else
    {
      printf("TEST ERROR: Should have failed unlinking.\n");
    }
    rd_close(fd);


    //test unlinking nonexistant file
    printf("\ntest to unlink nonexistant file\n");
    retval = unlink("/nonexistant");
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


    printf("\nUnlinking all of the files and subdirectories\n");
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

    printf("unlinked files. ls should be empty\n");
    ls("/");
    

    //reading and writing in the direct pointer blocks
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



    //reading and writing in the single indirect blocks
    printf("creating file to test correct writing to all of the indirect blocks\n");
    rd_creat("/friends.txt");
    fd = rd_open("/friends.txt");
    bytesIn = writeLogo(fd, 10);
    rd_close(fd);
    //seek to somewhere in the indirect pointers
    bytesOut = testSize("/friends.txt");
    if(bytesIn != bytesOut)
    {
      printf("TEST ERROR: Logo\n");
    }
    else
    {
      printf("TEST PASSED: Logo\n");
    }
    more("/friends.txt", 488*8);
    rd_unlink("/friends.txt");

    // Test the double indirect pointers
    printf("testing correct writing to the double indirect pointers\n");
    rd_creat("/big.txt");
    fd = rd_open("/big.txt");
    bytesIn = writeLogo(fd, 45);
    rd_close(fd);
    bytesOut = testSize("/big.txt");
    //seek to somewhere inside the double indirect pointers
    more("/big.txt", 488*43);

    rd_unlink("/big.txt");


    //@TODO: test interleaving reads/writes of multiple files
    //@TODO: try using up all the blocks in the filesystem

    printf("testing filling up the file system\n");
    rd_creat("/huge1.txt");
    fd=rd_open("/huge1.txt");
    bytesIn = writeLogo(fd, 2186); //488*2186 = 1066768 -> just under
				   //the max number of bytes in a file
    rd_close(fd);
    rd_creat("/huge2.txt");
    fd=rd_open("/huge2.txt");
    bytesIn = writeLogo(fd, 2186); //488*2186 = 1066768 -> just under
				   //the max number of bytes in a file
    //should crap out somewhere in here b/c the file system is full.
    rd_close(fd);
    
    rd_unlink("/huge1.txt");
    rd_unlink("/huge2.txt");


    //@TODO: make "cat" and "i_cat" functions

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
  char string3[6] ={"-= =-\n"};
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
    totalBytes += bytesWritten;
    if(bytesWritten != len)
    {
      printf("TEST ERROR (Logo): Did not write required number of bytes\n");
      break;
    }
  }
  bytesWritten = rd_write(fd, null, 1);
  totalBytes += bytesWritten;
  if(bytesWritten != 1)
  {
      printf("TEST ERROR (Logo): Did not write required number of bytes\n");
  }
  return totalBytes;  
}

