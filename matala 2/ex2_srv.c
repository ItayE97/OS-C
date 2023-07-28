#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/fcntl.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/random.h>
#include <stdbool.h>

#define SIZE 10
bool reply_recieved = false;

int readerFunc(int fd ,char* buf ,int size) //Reads one line from File(fd) to buf with maximum size of 'size'
{ //returns the index of the last char read

    int tmp = 0, i = 0;
    do 
    {
        tmp = read(fd, &buf[i++], 1);
        if(tmp == 0)
            return 0;    
        if(tmp < 1) 
            return -1;

    } while (buf[i - 1] != '\n');

    buf[i - 1] = '\0';
    return i;
}

void signal_handler(int sig)
{
    reply_recieved = true; //Set timer back to 60 seconds before timeout
    if(sig == SIGUSR1)
    {
        int childPid = fork(); //Child process to handle client
        if(childPid < 0)
        {
            perror("server fork failed!");
            exit(0);
        }
        else
        {
            if(childPid == 0) //Child
            {
                int toServerFD = open("to_srv.txt", O_RDONLY);
                if(toServerFD == -1)
                {
                    printf("ERROR_FROM_EX2\n");
                    exit(0);
                }

                printf("End of stage f\n");
                char pidBuf[SIZE] = {0}, num1Buf[SIZE] = {0} , num2Buf[SIZE] = {0}, mathBuf[SIZE] = {0};
                int res = 0;
                //Reading from file
                if(readerFunc(toServerFD,pidBuf,SIZE) == -1)
                {
                    printf("ERROR_FROM_EX2\n");
                    exit(0);
                }
                if(readerFunc(toServerFD,num1Buf,SIZE) == -1)
                {
                    printf("ERROR_FROM_EX2\n");
                    exit(0);
                }
                if(readerFunc(toServerFD,mathBuf,SIZE) == -1)
                {
                    printf("ERROR_FROM_EX2\n");
                    exit(0);
                }
                if(readerFunc(toServerFD,num2Buf,SIZE) == -1)
                {
                    printf("ERROR_FROM_EX2\n");
                    exit(0);
                }
                close(toServerFD); //Done reading
                system("rm to_srv.txt"); //Delete file to allow new clients to connect
                printf("End of stage g\n");
                //calcualte result
                int mathAction = atoi(mathBuf);
                switch (mathAction)
                {
                case 1:
                    res = atoi(num1Buf) + atoi(num2Buf);
                    break;
                case 2:
                    res = atoi(num1Buf) - atoi(num2Buf);
                    break;
                case 3:
                    res = atoi(num1Buf) * atoi(num2Buf);
                    break;
                case 4:
                    res = atoi(num1Buf) / atoi(num2Buf);
                    break;
                default:
                    printf("ERROR_FROM_EX2\n");
                    exit(0);
                }

                //create name for to_client_xxx file
                char fileName[SIZE*5] = "to_client_";
                strcat(fileName,pidBuf);
                strcat(fileName,".txt");

                //create the file
                int toClientFD = open(fileName, O_RDWR | O_CREAT, 0777);
                if(toClientFD == -1)
                {
                    printf("ERROR_FROM_EX2\n");
                    perror(fileName);
                    exit(0);
                }

                printf("End of stage h\n");
                char resStr[10];
                sprintf(resStr, "%d", res);
                if(write(toClientFD,resStr,strlen(resStr)) == -1)
                {
                    printf("ERROR_FROM_EX2\n");
                    exit(0);
                }
                close(toClientFD);
                int clientPid = atoi(pidBuf);
                kill(clientPid,SIGUSR1); //Done writing, signals client
                printf("End of stage i\n");
                exit(1);
            } //Child ends, server has made a to_client_xxx file
            else
            {
                int status;
                waitpid(childPid,&status,0); //Catching the child's exit value to prevent a zombie
            }
        } 
    }        
}

int main(int argc, char* argv[])
{
    signal(SIGUSR1,signal_handler);
    system("rm to_srv.txt");
    printf("End of stage c\n");

    int wait_time = 60;
    for (size_t i = 0; i < wait_time; i++) //Timer for 60 seconds
    {
        sleep(1);
        if(reply_recieved) //Signal recieved --> Set timer to 0
        {
            reply_recieved = false;
            i = 0;
        }
    }

    //Timeout
    printf("Server closed because no service request was received for the last 60 seconds.\n");
    return 1;  
}