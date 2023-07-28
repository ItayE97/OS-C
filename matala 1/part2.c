#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/fcntl.h>
#include <fcntl.h>

#define SIZE 100

void fixBuf(char* buf, int size) //Removes all '/r' from the buffer (Caused problems with execvp later on)
{
    for (size_t i = 0; i < size; i++)
        if(buf[i] == '\r')
            buf[i] = '\0';
}

int readerFunc(int fd ,char* buf ,int size) //Reads one line from File(fd) to buf with maximum size of 'size'
{ //returns the index of the last char read

    int tmp = 0, i = 0;
    do 
    {
        tmp = read(fd, &buf[i++], 1);
        if(tmp < 1) 
            return 0;

    } while (buf[i - 1] != '\n');

    buf[i - 1] = '\0';
    fixBuf(buf,size);
    return i;
}

int main(int argc, char* argv[]) 
{
    if (argc != 2)
    {
        perror("Expected 1 parameter");
        exit(-1);
    }

    char dir[SIZE], in[SIZE], out[SIZE];
    //Opening the config file
    int configFD = open("part2_configFile.txt", O_RDONLY);
    if (configFD == -1) 
    {
        perror("Error opening config file");
        exit(-1);
    }
    //Reading from config file
    if(readerFunc(configFD,dir,SIZE) == -1)
    {
        perror("Error reading into dir from config file");
        exit(-1);
    }
    if(readerFunc(configFD,in,SIZE) == -1)
    {
        perror("Error reading into in from config file");
        exit(-1);
    }
    if(readerFunc(configFD,out,SIZE) == -1)
    {
        perror("Error reading into out from config file");
        exit(-1);
    }
    close(configFD);

    //Creating grades for later use
    int gradesFD = open("Grades.csv",O_WRONLY | O_CREAT | O_TRUNC, 0777);
    if(gradesFD == -1)
    {
        perror("Error creating Grades.csv");
        exit(-1);
    }
    
    //First child used to create the student list with the names of all students inside
    int studentListFD = open("StudentList.txt",O_WRONLY | O_CREAT | O_TRUNC, 0777);
    if(studentListFD == -1)
    {
        perror("Error creating studentList");
        exit(-1);
    }

    pid_t pid;
    if((pid = fork()) < 0)
    {
        perror("fork number 1 failed");
        exit(-1);
    }
    else
    {
        if(pid == 0) /*Child*/
        {
            close(1);
            dup(studentListFD);
            close(studentListFD); //fd is now 1 and all output is sent there
            close(gradesFD);
            char* args[] = {"ls",dir,NULL};
            execvp(args[0],args); //runs the command 'ls' from dir folder to print all the student names into student list

        } /*Child ends*/
    }    
    
    if (pid > 0) /* wait for first child */
		wait(NULL);

    char studentName[SIZE];
    int StudentListFD = open("StudentList.txt", O_RDONLY); //Opening the file after child process put all the student names inside
    if(StudentListFD == -1)
    {
        perror("Failed to open StudentList.txt");
        exit(-1);
    }

    while(readerFunc(StudentListFD,studentName,SIZE) > 0) //reads one student from the studentList, stops when no students are left
    {
        int outFD = open("out.txt", O_WRONLY | O_CREAT | O_TRUNC, 0777);
        if(outFD == -1)
        {
            perror("Open outputFile failed");
            exit(-1);
        }
        //creating the path needed to reach the student's main.exe
        char sPath[SIZE];
        strcpy(sPath,dir);
        strcat(sPath,"/");
        strcat(sPath,studentName);
        strcat(sPath,"/main.exe");

        if((pid = fork()) < 0) //Child created to run the student's main.exe with the given input from input file.
        {
            perror("fork number 2 failed");
            exit(-1);
        }
        else
        {
            if(pid == 0) /*Child*/
            {
                int inFD = open(in, O_RDONLY);
                if(inFD == -1)
                {
                    perror("Failed to open input file");
                    exit(-1);
                }
                
                close(1);
                dup(outFD);
                close(outFD);

                char input1[2],input2[2]; //reading inputs from input file
                readerFunc(inFD,input1,2);
                readerFunc(inFD,input2,2);
                close(inFD);

                char* args[] = {sPath,input1,input2,NULL};
                execvp(args[0],args);
            } /*Child ends*/
        }

        if(pid > 0)	/*wait for child*/
            wait(NULL);

        if((pid = fork()) < 0) //Child created to compare the student's output with the expected output using the code from part 1
        {
            perror("fork number 2 failed");
            exit(-1);
        }
        else
        {
            if(pid == 0) /*Child*/
            {
                execvp("./compare.exe", (char*[]){"./compare.out","out.txt",out,NULL});         
            } /*Child ends*/
        }
        wait(&pid); //waits from child to return a value
        if (WEXITSTATUS(pid)) //reads the value from the child to set the score
        {
            write(gradesFD,studentName,strlen(studentName)); //writes the student's name
            if(WEXITSTATUS(pid) != 2) //return value of 2 --> score = 100, anyting else is considered score = 0
                write(gradesFD,",0",strlen(",0"));
            else 
                write(gradesFD,",100",strlen(",100"));
            write(gradesFD,"\n",strlen("\n"));
        }	
        system("rm out.txt");
    }
        
    close(gradesFD);
    system("rm StudentList.txt");
    printf("Done.\n");
    return 1;
}
