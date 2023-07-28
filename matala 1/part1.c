#include <stdio.h>
#include <stdlib.h>
#include <sys/fcntl.h>
#include <errno.h>
#include <unistd.h>

void closeFiles(int fd1, int fd2, int status)
{
    close(fd1);
    close(fd2);
    exit(status);
}

void main(int argc, char* argv[])
{
    if(argc != 3) //expected 3 arguments (2 files + code)
        exit(-1);

    char ch1,ch2;
    int index1,index2;

    int fd1 = open(argv[1],O_RDONLY);
    if(fd1 < 0) //failed to open 
        exit(-1);
    int fd2 = open(argv[2],O_RDONLY);
    if(fd2 < 0) //failed to open 
    {
        close(fd1);
        exit(-1);
    }

    while (index1 != EOF && index2 != EOF)
    {
        index1 = read(fd1,&ch1,1);
        if(index1 < 0) //failed to open , error
            closeFiles(fd1,fd2, -1);
            
        index2 = read(fd2,&ch2,1);
        if(index2 < 0) //failed to open , error
            closeFiles(fd1,fd2, -1);

        if(index1 == 0 && index2 == 0) //done reading files , success
            closeFiles(fd1,fd2, 2);

        if((index1 == 0 || index2 == 0) || (ch1 != ch2)) //done reading files , failed
            closeFiles(fd1,fd2, 1);
    }
}
