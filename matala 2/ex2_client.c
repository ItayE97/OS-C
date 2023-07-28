#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <sys/fcntl.h>
#include <fcntl.h>
#include <time.h>
#include <signal.h>
#include <sys/random.h>
#include <stdbool.h>

#define SIZE 10
bool reply_recieved = false;

int getRandomSleepTime(int m) { // A function to create a random number 
    unsigned char buf[10];
    ssize_t n = getrandom(buf, sizeof(buf), 0); 
    if (n < 0)
    {
        perror("getRandom failure");
        exit(0);
    }
    int rand = n % m;
    return rand;
}

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

void signal_handler(int sig) // A function to recieve the result from the server by signal
{  
    if (sig == SIGUSR1)
    {
        // Creating the file name to open it
        int selfPid = getpid();
        char pidStr[SIZE];
        sprintf(pidStr, "%d", selfPid); 
        char fileName[5*SIZE] = "to_client_";
        strcat(fileName, pidStr);
        strcat(fileName, ".txt");
        
        int fileFD = open(fileName, O_RDWR); // Open the result file 
        if (fileFD == -1)
        {
            printf("ERROR_FROM_EX2\n");
            exit(0); 
        }

        char buffer[SIZE];
        if (readerFunc(fileFD, buffer, SIZE) == -1) // Reading from result file
        {
            printf("ERROR_FROM_EX2\n");
            exit(0); 
        }
        close(fileFD);

        int pid = fork(); //Used to delete the result file from the server
        if(pid == -1)
        {
            perror("Fork failed!");
            exit(0);
        }
        else
        {
            if(pid == 0)
            {
                close(1);
                dup(pid);
                close(pid);
                char* args[] = {"rm",fileName,NULL};
                execvp(args[0],args);
            }
            //Wait for file to be removed and printf results
            wait(NULL); 
            int res = atoi(buffer);
            printf("End of stage j\n");   
            printf("Result: %d ", res);
            reply_recieved = true; //stops the timer 
        }      
    }
}

int main(int argc, char* argv[]) 
{
    signal(SIGUSR1, signal_handler);
    if (argc != 5) { // In case of an incorrect num of arguments
        printf("ERROR_FROM_EX2\n");
        exit(0);
    }

    if(atoi(argv[3]) < 1 || atoi(argv[3]) > 4)
    { // Value of argument number 3 needs to be between 1 to 4
        printf("ERROR_FROM_EX2\n");
        exit(0);
    }

    if(atoi(argv[3]) == 4 && atoi(argv[4]) == 0)
    { // Value of argument number 3 needs to be between 1 to 4
        printf("ERROR_FROM_EX2\n");
        printf("Cannot divide by zero!\n");
        exit(0);
    }
    
    sleep(getRandomSleepTime(21)); //sleep time to run more than 1 client at the same time for testing

    int fileFD = -1;
    for (int i = 0; i < 10; i++) // A loop to get a random number of seconds for the client to wait and try to open to_srv
    {
        fileFD = open("to_srv.txt", O_RDONLY | O_WRONLY | O_CREAT, 0740);
        if (fileFD < 0) {
            int rand = getRandomSleepTime(6);
            sleep(rand);
        }
        else
        {
            printf("\nEnd of stage e\n");
            break;
        }
    }
    if (fileFD == -1) // Openning file failure
    {
        printf("ERROR_FROM_EX2\n");
        perror("End of stage e with failure.");
        exit(0);
    }
   

    // Writing to to_srv the given arguments
    int selfPid = getpid();
    char selfPidBuf[SIZE] = {0}, num1Buf[SIZE] = {0}, num2Buf[SIZE] = {0}, mathBuf[SIZE] = {0};

    sprintf(selfPidBuf,"%d",selfPid);
    strcat(selfPidBuf,"\n");
    sprintf(num1Buf,"%s",argv[2]);
    strcat(num1Buf,"\n");
    sprintf(mathBuf,"%s",argv[3]);
    strcat(mathBuf,"\n");
    sprintf(num2Buf,"%s",argv[4]);

    //Writing to file
    if(write(fileFD, selfPidBuf, strlen(selfPidBuf)) == -1) 
    {
        printf("ERROR_FROM_EX2\n");
        exit(0);
    }
    if(write(fileFD, num1Buf, strlen(num1Buf)) == -1)
    {
        printf("ERROR_FROM_EX2\n");
        exit(0);
    }
    if(write(fileFD, mathBuf, strlen(mathBuf)) == -1)
    {
        printf("ERROR_FROM_EX2\n");
        exit(0);
    }
    if(write(fileFD, num2Buf, strlen(num2Buf)) == -1)
    {
        printf("ERROR_FROM_EX2\n");
        exit(0);
    }
    close(fileFD);
    
    int serverPid = atoi(argv[1]);
    kill(serverPid,SIGUSR1); // Sending signal to server that the to_srv file is ready
    printf("End of stage d\n");
    
    //Timer to wait for a response from the server
    int wait_time = 30;
    for (size_t i = 0; i < wait_time; i++) //Timer for 30 seconds
    {
        sleep(1);
        if(reply_recieved) //Signal recieved --> close client
        {
            printf("from client %d.\n",selfPid);
            return 1;
        }
    }

    //Timeout
    printf("Client closed because no response was received from the server for 30 seconds\n");
    return 0;
}