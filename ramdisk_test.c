#include <stdio.h>
#include <string.h>

#include "ramdisk.h"

struct Ramdisk ramdisk;
struct FdtableArray fdtablea;

int main()
{
    char name[16] = "/aaaa";

    _ramdisk_initialize(&ramdisk);

    //create a directory that requires more than one block
    int i;
    for(i=0; i<26; i++)
    {
      int q = rd_creat(name);
      printf("%d %d %s\n", i, q, name); 
      name[1]++;
    }
    //test readdir
    ls("/");

    name[1]='a';
    for(i=0; i<10; i++)
    {
      rd_unlink(name);
      name[1]++;
    }
    printf("unlinked 10 files\n");
    ls("/");

    for(; i<26; i++)
    {
      rd_unlink(name);
      name[1]++;
    }

    ls("/");

    //return 0;

    //test making subdirectories and regular files
    rd_mkdir("/home");
    rd_mkdir("/home/christmas");
    rd_creat("/home/christmas/cranberry.txt");
    rd_creat("/home/christmas/candycane.txt");
    rd_creat("/home/christmas/cookies.txt");
    rd_creat("/home/christmas/friends.txt");

    //test readdir
    ls("/home/christmas");


    //writing lots of little pieces
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
      printf("%s \n-.-.-.-\n", buffer);
    }
    fd = rd_close(fd);

    //writing/reading one big chunk
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
    printf("%d\n%s", len, logo);
    //write some longer, multi-block chunks
    for(i=0; i<4; i++)
    {
      rd_write(fd, logo, len);
    }

    //reset to the beginning
    rd_seek(fd, 0);

    //read out as one big chunk
    char raptor[1953];
    rd_read(fd, raptor, len*4);
    raptor[len*4]='\0';
    printf("%s", raptor);

    //close the file
    rd_close(fd);

    more("/home/christmas/friends.txt");




//@TODO: test interleaving


    //this should fail
    //int p = rd_mkdir("/usr/blah/bin");

    return 0;
}

int ls(char* path)
{
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
