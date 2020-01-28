#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <fcntl.h>


//File flags for > type operations
#define CREATE_FLAGS (O_WRONLY | O_TRUNC | O_CREAT )
//File flags for >> type files
#define CREATE_APPENDFLAGS (O_WRONLY | O_APPEND | O_CREAT )
//File flags for input files
#define CREATE_INPUTFLAGS (O_RDWR)
//File flags for type mode
#define CREATE_MODE (S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH)


// ERAY AYAZ 150116053
// ERTUGRUL SAGDIC 150116061


#define MAX_LINE 80 /* 80 chars per line, per command, should be enough. */
/*Our Global Variables*/
int background; /* equals 1 if a command is followed by '&' */
char history[10][80];
int currentForegroundId;
int foregroundProcess = 0;
int backgroundProcess = 0;
int countArgs = 0;
char *pathname; /*In the part our executable path in this variable*/

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wimplicit-function-declaration"

/*Linked List for background process*/
struct Node {
    int id;
    struct Node *next;
    char command[128];
};

struct Node *head = NULL;

/* The setup function below will not return any value, but it will just: read
in the next command line; separate it into distinct arguments (using blanks as
delimiters), and set the args array entries to point to the beginning of what
will become null-terminated, C-style strings. */

/*Methods*/

/*Execute the command*/
int execute(char **args, int *background);

void setup(char inputBuffer[], char *args[], int *background);

/*Adding the last commands into a history array*/
void History(char **args);

/* ^z Method */
void end(int signalNumber);

/*Delete the background process from linked list*/
void deleteBackground(struct Node *iter);

int main(void) {
    char inputBuffer[MAX_LINE]; /*buffer to hold command entered */
    char *args[MAX_LINE / 2 + 1]; /*command line arguments */
    pathname = getenv("PATH");
    char *tempArgs[MAX_LINE / 2 + 1];
    char *tempArgs0[MAX_LINE / 2 + 1];
    int pipeline;
    while (1) {

        /*Start ^z command*/
        struct sigaction act;

        act.sa_handler = end;/* set up signal handler */
        act.sa_flags = 0;
        sigemptyset(&act.sa_mask);
        if ((sigaction(SIGTSTP, &act, NULL) == -1)) {
            perror("Failed to set SIGSTP handler");
            return 1;
        }
        /*End ^z command*/

        background = 0;
        printf("myshell: ");
        fflush(NULL);
        setup(inputBuffer, args, &background);/*setup() calls exit() when Control-D is entered */
        int i = 0;
        //backgroundProcess = isExecutable(args, &background);

        while (args[i] != NULL) {
            if (strcmp(args[i], ";") == 0)
                pipeline = 1; /*If the user enter ; this character then pipeline become 1*/
            countArgs++;
            i++;
        }


        if (args[0] == NULL) {
            continue;
        } else if (pipeline) {
            i = 0;
            int j = 0;
            while ((strcmp(args[i], ";") != 0)) {/*We put into a command 1 to tempArgs array.*/
                tempArgs[i] = strdup(args[i]);
                i++;
            }
            tempArgs[i] = '\0';
            i = i + 1;
            while (args[i] != NULL) {/*We put into a command 2 to tempArgs array.*/
                tempArgs0[j] = strdup(args[i]);
                i++;
                j++;
            }
            tempArgs0[j] = NULL;
            execute(tempArgs, &background);
            execute(tempArgs0, &background);
            pipeline = 0;
        } else {
            execute(args, &background);
        }

        /** the steps are:
        (1) fork a child process using fork()
        (2) the child process will invoke execv()
        (3) if background == 0, the parent will wait,
        otherwise it will invoke the setup() function again. */
    }
}

int execute(char **args, int *background) {
    if (args[1] == NULL && strcmp(args[0], "path") == 0) {/*Print the current path */
        printf("Currently path is : %s\n", pathname);

    } else if (args[1] != NULL && strcmp(args[0], "path") == 0 &&
               strcmp(args[1], "+") == 0) {/*Add something to pathnames*/
        strcat(pathname, ":");
        strcat(pathname, args[2]);
        printf("Currently path is : %s\n", pathname);
    } else if (args[1] != NULL && strcmp(args[0], "path") == 0 &&
               strcmp(args[1], "-") == 0) {/*Delete something from the pathnames*/
        char *dividedPath2[80];
        int i = 0;
        char tempPath[200] = "";
        strcat(tempPath, pathname);

        dividedPath2[0] = strtok(tempPath, ":");
        while (dividedPath2[i] !=
               NULL) {  /*This loop divide the path and it will search the inputs inside the pathname*/
            if (strcmp(dividedPath2[i], args[2]) == 0) {
                dividedPath2[i] = NULL;
            }
            i++;
            dividedPath2[i] = strtok(NULL, ":");/*If the nth path dont match with input It will look the (n+1)th path */
        }

        char temp[400] = "";
        int j = 0;

        for (j = 0; j < i; j++) { /*It will append the path */
            if (dividedPath2[j] == NULL) {
                continue;
            } else {
                strcat(temp, dividedPath2[j]);
                if (j == i - 1)
                    continue;
                strcat(temp, ":");
            }

        }
        strcpy(pathname, temp); /*This copying procedure does the update the global variable of pathname*/
        printf("Currently path is : %s\n", pathname);


    } else if (args[1] != NULL && strcmp(args[0], "history") == 0 && strcmp(args[1], "-i") == 0) {
/*It will execute the command with index of the command */
        //temp variables
        char *dividedHistory[80];
        char tempHistory[80];
        int num, i;
        //converts string to integer
        num = atoi(args[2]);
        num++;
        //checks if history[index] empty or not
        if (history[num] != NULL) {
            int pipeline;
            char *tempArgs[MAX_LINE / 2 + 1];
            char *tempArgs0[MAX_LINE / 2 + 1];
            //copies history to temp
            strcpy(tempHistory, history[num]);
	    printf("%s",tempHistory);
            //divides history[index]
            dividedHistory[0] = strtok(tempHistory, " ");
            i = 0;
            while (dividedHistory[i] != NULL) {
                i++;
                dividedHistory[i] = strtok(NULL, " ");
            }
            while (args[i] != NULL) {
                if (strcmp(args[i], ";") == 0)
                    pipeline = 1;
            }
            //execute with the argument
            if (pipeline) {
                i = 0;
                int j = 0;
                while ((strcmp(dividedHistory[i], ";") != 0)) {/*We put into a command 1 to tempArgs array.*/
                    tempArgs[i] = strdup(dividedHistory[i]);
                    i++;
                }
                tempArgs[i] = '\0';
                i = i + 1;
                while (dividedHistory[i] != NULL) {/*We put into a command 2 to tempArgs array.*/
                    tempArgs0[j] = strdup(dividedHistory[i]);
                    i++;
                    j++;
                }
                tempArgs0[j] = NULL;
                execute(tempArgs, background);
                execute(tempArgs0, background);
                pipeline = 0;
            } else {
                execute(dividedHistory, background);
            }
            //adds used value to history
            History(dividedHistory);
        } else {
            fprintf(stderr, "Please give valid index\n");
        }
    } else if (strcmp(args[0], "history") == 0 && args[1] == NULL) { /*This will print the last 10 executed commands*/
        for (int i = 0; i < 10; i++) {
            printf("%d %s\n", i, history[i]);
        }
    } else if (strcmp(args[0], "exit") == 0 && args[1] ==
                                               NULL) { /*If there is no background process in the system exit command will be exit from the system*/
        int isBackground = 0;
        struct Node *temp;
        temp = head;
        if (temp == NULL) {
            exit(0);
        }
        if (waitpid(-1, NULL, WNOHANG) ==
            0) { /*If there is a running background process in the system this if statement will be wait the end of the background process*/
            fprintf(stderr, "There is a running background process.\n");
            return 0;
        } else {
            exit(0); /* When the background process finish then we can exit from the system with exit command */
        }

    } else if (strcmp(args[0], "fg") == 0 &&
               args[1] != NULL) {/* move the background process with process id to the foreground*/
        int status;
        struct Node *temp;
        temp = head;

        if (temp != NULL) {/*It will check the background process list is it empty or not*/
            while (temp != NULL) {
                if (atoi(args[1]) == temp->id) {
                    currentForegroundId = temp->id;
                    deleteBackground(temp);//delete from the background linked list
                    foregroundProcess = 1; //Now we have a foreground process
                    kill(currentForegroundId, SIGCONT);//kill the background process and wait
                    waitpid(currentForegroundId, &status, WUNTRACED);/*Wait the child*/
                }
                temp = temp->next;
            }
        } else {
            fprintf(stderr, "There is no background process..\n");
            return 0;
        }
    }    /* myprog [args] < file.in > file.out */
    else if (args[4] != NULL && (args[5] == NULL) && strcmp(args[1], "<") == 0 && strcmp(args[3], ">") == 0) {

        pid_t pid;
        int status, i, j;
        //Path which will divided
        char *dividedPath[80];

        char *path = getenv("PATH");
        //to prevent from change of dividedPath path
        char tempPath[200] = "";
        strcat(tempPath, path);

        if ((pid = fork()) < 0) {     /* fork a child process           */
            printf("*** ERROR: forking child process failed\n");
            exit(0);
        } else if (pid == 0) {          /* for the child process:         */

            int fd;
            int fd1;

            char finput[80];
            char foutput[80];

            strcpy(finput, args[2]);
            strcpy(foutput, args[4]);

            /*open the file*/
            fd = open(finput, CREATE_INPUTFLAGS, CREATE_MODE);
            fd1 = open(foutput, CREATE_FLAGS, CREATE_MODE);

            /*Check the errors*/
            if (fd == -1) {
                perror("Failed to open file");
                return 1;
            }
            if (dup2(fd, STDIN_FILENO) == -1) {
                perror("Failed to redirect standart output");
                return 1;
            }
            if (close(fd) == -1) {
                perror("Failed to close the file");
                return 1;
            }

            if (fd1 == -1) {
                perror("Failed to open file");
                return 1;
            }
            if (dup2(fd1, STDOUT_FILENO) == -1) {
                perror("Failed to redirect standart output");
                return 1;
            }

            if (close(fd1) == -1) {
                perror("Failed to close the file");
                return 1;
            }

            /* It will do the every inputs except executable command will become a NULL */
            if (args[1] != NULL || args[2] != NULL || args[3] != NULL || args[4] != NULL) {
                int i;
                for (i = 5; i > 1; i--) {
                    args[i - 1] = args[i];
                }
            }

            //We divided the whole path when the function see ":"  key.
            dividedPath[0] = strtok(tempPath, ":");
            i = 0;
            //do until the path ends
            while (dividedPath[i] != NULL) {
                //to prevent from change of dividedPath
                char tempDivided[80] = " ";
                strcpy(tempDivided, dividedPath[i]);
                //modify the path with given args
                strcat(tempDivided, "/");
                strcat(tempDivided, args[0]);
                // execute the command
                if (execv(tempDivided, args) < 0) {
                    //fprintf(stderr, "*** ERROR: exec failed\n");
                }
                //skip the next path
                dividedPath[i + 1] = strtok(NULL, ":");
                i++;
            }
            fprintf(stderr, "Command not found\n");

        } else {    /* for the parent:      */
            waitpid(pid, NULL, 0);
            return 0;
        }
    } /* myprog [args] 2> file.out */
    else if (args[1] != NULL && strcmp(args[1], "2>") == 0) {

        pid_t pid;
        int i;
        //Path which will divided
        char *dividedPath[80];

        char *path = getenv("PATH");
        //to prevent from change of dividedPath path
        char tempPath[200] = "";
        strcat(tempPath, path);

        if ((pid = fork()) < 0) {     /* fork a child process           */
            printf("*** ERROR: forking child process failed\n");
            exit(0);
        } else if (pid == 0) {          /* for the child process:         */

            int fd;
            char foutput[80];
            strcpy(foutput, args[2]);
            fd = open(foutput, CREATE_FLAGS, CREATE_MODE);
            if (fd == -1) {
                perror("Failed to open file");
                return 1;
            }
            if (dup2(fd, STDERR_FILENO) == -1) {
                perror("Failed to redirect standart output");
                return 1;
            }
            if (close(fd) == -1) {
                perror("Failed to close the file");
                return 1;
            }

            /* It will do the every inputs except executable command will become a NULL */
            if (args[1] != NULL || args[2] != NULL) {
                int i;
                for (i = 3; i > 1; i--) {
                    args[i - 1] = args[i];
                }
            }

            //We divided the whole path when the function see ":"  key.
            dividedPath[0] = strtok(tempPath, ":");
            i = 0;
            //do until the path ends
            while (dividedPath[i] != NULL) {
                //to prevent from change of dividedPath
                char tempDivided[80] = " ";
                strcpy(tempDivided, dividedPath[i]);
                //modify the path with given args
                strcat(tempDivided, "/");
                strcat(tempDivided, args[0]);
                // execute the command
                if (execv(tempDivided, args) < 0) {
                    //fprintf(stderr, "*** ERROR: exec failed\n");
                }
                //skip the next path
                dividedPath[i + 1] = strtok(NULL, ":");
                i++;
            }
            fprintf(stderr, "Command not found\n");

        } else {    /*   for the parent:      */

            waitpid(pid, NULL, 0);
            return 0;
        }

    }       /*myprog [args] < file.in*/
    else if (args[1] != NULL && strcmp(args[1], "<") == 0) {

        pid_t pid;
        int i;
        //Path which will divided
        char *dividedPath[80];

        char *path = getenv("PATH");
        //to prevent from change of dividedPath path
        char tempPath[200] = "";
        strcat(tempPath, path);

        if ((pid = fork()) < 0) {     /* fork a child process           */
            printf("*** ERROR: forking child process failed\n");
            exit(0);
        } else if (pid == 0) {          /* for the child process:         */

            int fd;
            char foutput[80];
            strcpy(foutput, args[2]);
            /*Open the file*/
            fd = open(foutput, CREATE_INPUTFLAGS, CREATE_MODE);
            
            /*Check the errors*/
            if (fd == -1) {
                perror("Failed to open file");
                return 1;
            }
            if (dup2(fd, STDIN_FILENO) == -1) {
                perror("Failed to redirect standart output");
                return 1;
            }
            if (close(fd) == -1) {
                perror("Failed to close the file");
                return 1;
            }
            /* It will do the every inputs except executable command will become a NULL */
            if (args[1] != NULL || args[2] != NULL) {
                int i;
                for (i = 3; i > 1; i--) {
                    args[i - 1] = args[i];
                }
            }
            //We divided the whole path when the function see ":"  key.
            dividedPath[0] = strtok(tempPath, ":");
            i = 0;
            //do until the path ends
            while (dividedPath[i] != NULL) {
                //to prevent from change of dividedPath
                char tempDivided[80] = " ";
                strcpy(tempDivided, dividedPath[i]);
                //modify the path with given args
                strcat(tempDivided, "/");
                strcat(tempDivided, args[0]);
                // execute the command
                if (execv(tempDivided, args) < 0) {
                    //fprintf(stderr, "*** ERROR: exec failed\n");
                }
                //skip the next path
                dividedPath[i + 1] = strtok(NULL, ":");
                i++;
            }
            fprintf(stderr, "Command not found\n");
        } else {    /* for the parent:      */
            waitpid(pid, NULL, 0);
            return 0;
        }
    }
        //myprog [args] >> file.out
    else if (args[1] != NULL && ((strcmp(args[1], ">>") == 0))) {

        pid_t pid;
        int status, i, j;
        //Path which will divided
        char *dividedPath[80];

        char *path = getenv("PATH");
        //to prevent from change of dividedPath path
        char tempPath[200] = "";
        strcat(tempPath, path);

        if ((pid = fork()) < 0) {     /* fork a child process           */
            printf("*** ERROR: forking child process failed\n");
            exit(0);
        } else if (pid == 0) {          /* for the child process:         */

            int fd;
            char foutput[80];
            strcpy(foutput, args[2]);
            printf("Eray");
            fd = open(foutput, CREATE_APPENDFLAGS, CREATE_MODE);
            if (fd == -1) {
                perror("Failed to open file");
                return 1;
            }
            if (dup2(fd, STDOUT_FILENO) == -1) {
                perror("Failed to redirect standart output");
                return 1;
            }
            if (close(fd) == -1) {
                perror("Failed to close the file");
                return 1;
            }

            /* It will do the every inputs except executable command will become a NULL */
            if (args[1] != NULL || args[2] != NULL) {
                int i;
                for (i = 3; i > 1; i--) {
                    args[i - 1] = args[i];
                }
            }

            //We divided the whole path when the function see ":"  key.
            dividedPath[0] = strtok(tempPath, ":");
            i = 0;
            //do until the path ends
            while (dividedPath[i] != NULL) {
                //to prevent from change of dividedPath
                char tempDivided[80] = " ";
                strcpy(tempDivided, dividedPath[i]);
                //modify the path with given args
                strcat(tempDivided, "/");
                strcat(tempDivided, args[0]);
                // execute the command
                if (execv(tempDivided, args) < 0) {
                    //fprintf(stderr, "*** ERROR: exec failed\n");
                }
                //skip the next path
                dividedPath[i + 1] = strtok(NULL, ":");
                i++;
            }
            fprintf(stderr, "Command not found\n");

        } else {    /* for the parent:      */

            waitpid(pid, NULL, 0);
            return 0;
        }

    }/*myprog[args] > file.out*/
    else if (args[1] != NULL && strcmp(args[1], ">") == 0) {

        pid_t pid;
        int status, i, j;
        //Path which will divided
        char *dividedPath[80];
        char *path = getenv("PATH");
        //to prevent from change of dividedPath path
        char tempPath[200] = "";
        strcat(tempPath, path);

        if ((pid = fork()) < 0) {     /* fork a child process           */
            printf("*** ERROR: forking child process failed\n");
            exit(0);
        } else if (pid == 0) {          /* for the child process:         */

            int fd;
            char foutput[80];
            strcpy(foutput, args[2]);
            fd = open(foutput, CREATE_FLAGS, CREATE_MODE);
            if (fd == -1) {
                perror("Failed to open file");
                return 1;
            }
            if (dup2(fd, STDOUT_FILENO) == -1) {
                perror("Failed to redirect standart output");
                return 1;
            }
            if (close(fd) == -1) {
                perror("Failed to close the file");
                return 1;
            }

            if (args[1] != NULL || args[2] != NULL) {
                int i;
                for (i = 3; i > 1; i--) {
                    args[i - 1] = args[i];
                }
            }

            //We divided the whole path when the function see ":"  key.
            dividedPath[0] = strtok(tempPath, ":");
            i = 0;
            //do until the path ends
            while (dividedPath[i] != NULL) {
                //to prevent from change of dividedPath
                char tempDivided[80] = " ";
                strcpy(tempDivided, dividedPath[i]);
                //modify the path with given args
                strcat(tempDivided, "/");
                strcat(tempDivided, args[0]);
                // execute the command
                if (execv(tempDivided, args) < 0) {
                    //fprintf(stderr, "*** ERROR: exec failed\n");
                }
                //skip the next path
                dividedPath[i + 1] = strtok(NULL, ":");
                i++;
            }
            fprintf(stderr, "Command not found\n");

        } else {    /* for the parent:      */
            waitpid(pid, NULL, 0);
            return 0;
        }
    } else {
        if (*background == 1 && args[1] != NULL) {

            struct Node *temp = NULL;

            pid_t pid;
            //Path which will divided
            char *dividedPath[80];

            char *path = getenv("PATH");
            //to prevent from change of dividedPath path
            char tempPath[200] = "";
            strcat(tempPath, path);

            if ((pid = fork()) < 0) {     /* fork a child process           */
                printf("*** ERROR: forking child process failed\n");
                exit(0);
            } else if (pid == 0) {          /* for the child process:         */

                //We divided the whole path when the function see ":"  key.
                dividedPath[0] = strtok(tempPath, ":");
                int i = 0;
                //do until the path ends
                while (dividedPath[i] != NULL) {
                    //to prevent from change of dividedPath
                    char tempDivided[80] = " ";
                    strcpy(tempDivided, dividedPath[i]);
                    //modify the path with given args
                    strcat(tempDivided, "/");
                    strcat(tempDivided, args[0]);

                    // execute the command
                    if (execv(tempDivided, args) < 0) {
                        //fprintf(stderr, "ERROR: exec failed\n");
                    }
                    //skip the next path
                    dividedPath[i + 1] = strtok(NULL, ":");
                    i++;
                }
                fprintf(stderr, "Command not found\n");

            } else {    /* for the parent:      */ /*Create a linked list with background process id and name*/
                if (head == NULL) {
                    head = (struct Node *) malloc(sizeof(struct Node));
                    head->id = pid;
                    strcpy(head->command, args[0]);
                    printf("pid : %d command : %s\n", head->id, head->command);
                    head->next = NULL;
                } else if (head->next == NULL) {
                    temp = (struct Node *) malloc(sizeof(struct Node));
                    temp->id = pid;
                    strcpy(temp->command, args[0]);
                    head->next = temp;
                    printf("pid : %d command : %s\n", temp->id, temp->command);
                    temp->next = NULL;
                }
                while (waitpid(pid, NULL, WNOHANG));
                return 0;
            }

        } else if (*background == 0) {
            int pid;
            pid = fork();
            int i;
            //Path which will divided
            char *dividedPath[80];

            char *path = getenv("PATH");
            //to prevent from change of dividedPath path
            char tempPath[400] = "";
            strcat(tempPath, path);

            if (pid == -1) {     /* fork a child process           */
                printf("*** ERROR: forking child process failed\n");
                exit(0);
            } else if (pid == 0) {          /* for the child process:         */

                //We divided the whole path when the function see ":"  key.
                dividedPath[0] = strtok(tempPath, ":");
                i = 0;
                //do until the path ends
                while (dividedPath[i] != NULL) {
                    //to prevent from change of dividedPath
                    char tempDivided[80] = " ";
                    strcpy(tempDivided, dividedPath[i]);
                    //modify the path with given args
                    strcat(tempDivided, "/");
                    strcat(tempDivided, args[0]);
                    // execute the command
                    if (execv(tempDivided, args) < 0) {
                        //fprintf(stderr, "*** ERROR: exec failed\n");
                    }
                    //skip the next path
                    dividedPath[i + 1] = strtok(NULL, ":");
                    i++;
                }
                fprintf(stderr, "Command not found\n");

            } else if (pid > 0) {    /* for the parent:      */
                currentForegroundId = pid;
                foregroundProcess = 1;
                waitpid(pid, NULL, 0);
                foregroundProcess = 0;
                return 0;
            }
        } else {
            return 0;
        }
    }
}
void deleteBackground(struct Node *iter) {
    struct Node *temp = iter;
    iter = temp->next;
    temp->next = NULL;
    free(temp);
}

void end(int signalNumber) {
    if (foregroundProcess > 0) {
        printf("\n");
        kill(currentForegroundId, SIGKILL);
        foregroundProcess = 0;
    } else {
        printf("\n");
    }
}
void History(char **args) {

    int i, j;
    char tempArgs[MAX_LINE];
    //concats strings
    j = 0;
    while (args[j] != NULL) {
        strcat(tempArgs, args[j]);
        strcat(tempArgs, " ");
        j++;
    }
    //copies the element into history
    for (i = 9; i >= 0; i--) {
        strcpy(history[i], history[i - 1]);
        if (i == 0)
            strcpy(history[0], tempArgs);
    }
}
void setup(char inputBuffer[], char *args[], int *background) {
    int length, /* # of characters in the command line */
            i,      /* loop index for accessing inputBuffer array */
            start,  /* index where beginning of next command parameter is */
            ct;     /* index of where to place the next parameter into args[] */

    ct = 0;

    /* read what the user enters on the command line */
    length = read(STDIN_FILENO, inputBuffer, MAX_LINE);


    /* 0 is the system predefined file descriptor for stdin (standard input),
       which is the user's screen in this case. inputBuffer by itself is the
       same as &inputBuffer[0], i.e. the starting address of where to store
       the command that is read, and length holds the number of characters
       read in. inputBuffer is not a null terminated C-string. */

    start = -1;
    if (length == 0)
        exit(0);            /* ^d was entered, end of user command stream */

/* the signal interrupted the read system call */
/* if the process is in the read() system call, read returns -1
  However, if this occurs, errno is set to EINTR. We can check this  value
  and disregard the -1 value */
    if ((length < 0) && (errno != EINTR)) {
        perror("error reading the command");
        exit(-1);           /* terminate with error code of -1 */
    }

    //printf(">>%s<<",inputBuffer);
    for (i = 0; i < length; i++) { /* examine every character in the inputBuffer */

        switch (inputBuffer[i]) {
            case ' ':
            case '\t' :               /* argument separators */
                if (start != -1) {
                    args[ct] = &inputBuffer[start];    /* set up pointer */
                    ct++;
                }
                inputBuffer[i] = '\0'; /* add a null char; make a C string */
                start = -1;
                break;

            case '\n':                 /* should be the final char examined */
                if (start != -1) {
                    args[ct] = &inputBuffer[start];
                    ct++;
                }
                inputBuffer[i] = '\0';
                args[ct] = NULL; /* no more arguments to this command */
                break;

            default :             /* some other character */
                if (start == -1)
                    start = i;
                if (inputBuffer[i] == '&') {
                    *background = 1;
                    inputBuffer[i - 1] = '\0';
                }
        } /* end of switch */
    }    /* end of for */
    args[ct] = NULL; /* just in case the input line was > 80 */


    for (i = 0; i <= ct; i++) {
        //printf("args %d = %s\n", i, args[i]);
    }
    /*Every command will be recorded in the our history list*/
    History(args);


}
/* end of setup routine */
//#pragma clang diagnostic pop
