#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "ramdisk.h"

struct Ramdisk* RAMDISK;
struct FdtableArray fdtablea;

int main()
{
    char name[14] = "/aaaa";
    RAMDISK = malloc(sizeof(struct Ramdisk));

    _ramdisk_initialize(RAMDISK);

    //create a directory that requires more than one block
    //(use all of the direct pointers)
    int i;
    int numdirectories=128;
    for(i=0; i<numdirectories; i++)
    {
      sprintf(name, "/a%d", i);
      int q = rd_creat(name);
      printf("%d %d %s\n", i, q, name); 
    }
    //test readdir
    ls("/");

    for(i=0; i<numdirectories; i++)
    {
      sprintf(name, "/a%d", i);
      rd_unlink(name);
    }

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
    ls("/");

    for(i=0; i<numdirectories; i++)
    {
      sprintf(name, "/a%d", i);
      rd_unlink(name);
    }

    //@TODO: Make sure behavior is correct when we go over the number
    //of iNodes
    //@TODO: use the superblock for book-keeping

    ls("/");


    //@TODO: Test unlinking subdirectories of the root


    //test making subdirectories and regular files
    rd_mkdir("/home");
    rd_mkdir("/home/christmas");
    rd_creat("/home/christmas/cranberry.txt");
    rd_creat("/home/christmas/candycane.txt");


    //test readdir
    ls("/home/christmas");


    //reading and writing in the direct pointer blocks
    rd_creat("/home/christmas/cookies.txt");
    int repeats=5;
    int fd = rd_open("/home/christmas/cookies.txt");
    char* string1 = "c is for cookie that's good enough for me. "; //43
    char* string2 = "oh! cookie cookie cookie starts with C. "; //40
    char string3[6] ={"-= =-\n"};
    char* null="\0";
    for(i=0; i<repeats; i++)
    {
      rd_write(fd, string1, 43);
      rd_write(fd, string1, 43);
      rd_write(fd, string1, 43);
      rd_write(fd, string2, 40);
      string3[2]='0'+i;
      rd_write(fd, string3, 6);
    }
    rd_write(fd, null, 1);

    //reading out the pieces in different sized chunks than we wrote them
    //175
    char buffer[200];
    rd_seek(fd, 0);
    for(i=0; i<repeats; i++)
    {
      rd_read(fd, buffer, 175);
      buffer[176]='\0';
      //printf("%s \n-.-.-.-\n", buffer);
    }
    fd = rd_close(fd);

    rd_unlink("/home/christmas/cookies.txt");


    //reading and writing in the single indirect blocks
    rd_creat("/home/christmas/friends.txt");
    fd = rd_open("/home/christmas/friends.txt");
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
    int repetitions=10;
    printf("%d\n%s", len, logo);
    //write some longer, multi-block chunks
    for(i=0; i<repetitions; i++)
    {
      rd_write(fd, logo, len);
    }

    //reset to the beginning
    rd_seek(fd, 0);

    //read out as one big chunk
    //char raptor[1953];
    char raptor[488*10+1];
    rd_read(fd, raptor, len*10);
    raptor[len*repetitions]='\0';
    printf("%s", raptor);

    //close the file
    rd_close(fd);

    rd_unlink("/home/christmas/friends.txt");


    // Test the double indirect pointers
    rd_creat("/home/christmas/big.txt");
    fd = rd_open("/home/christmas/big.txt");
    for(i=0; i<40; i++)
    {
      rd_write(fd, logo, len);
    }

    //seek to somewhere inside the double indirect pointers
    rd_seek(fd, 488*38);

    //read the last two iterations
    rd_read(fd, raptor, len*2);
    raptor[len*2]='\0';
    printf("%s", raptor);
    rd_close(fd);

    rd_unlink("/home/christmas/big.txt");


    //test unlinking directories that are subdirectories of the root
    rd_unlink("/home/christmas/cranberry.txt");
    rd_unlink("/home/christmas/candycane.txt");
    rd_unlink("/home/christmas");
    rd_unlink("/home");

    //@TODO: test multiple file handles open
    //@TODO: test interleaving reads/writes of multiple files
    //@TODO: test unlinking open files
    //@TODO: test unlinking non-existent files
    //@TODO: test making entry of non-existent directory
    //@TODO: try using up all the blocks in the filesystem
    //@TODO: write something to all the little files we create in the beginning

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

int more(char* path)
{
  char buffer[198];
  buffer[197]='\0';
  int fd = rd_open(path);
  if(fd < 0)
  {
    return -1;
  }

  while(rd_read(fd, buffer, 197) > 0)
  {
    printf("%s", buffer);
  }
  printf("%s", buffer);
  
  printf("\n");
  return 0;
}
