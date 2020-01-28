/* Includes */
#include <unistd.h>     /* Symbolic Constants */
#include <sys/types.h>  /* Primitive System Data Types */ 
#include <errno.h>      /* Errors */
#include <stdio.h>      /* Input/Output */
#include <stdlib.h>     /* General Utilities */
#include <pthread.h>    /* POSIX Threads */
#include <string.h>     /* String handling */
#include <semaphore.h>  /* Semaphore */
#include <stdbool.h>	/*Boolean variables*/

/*	Eray Ayaz 		150116053
	Ertuğrul Sağdıç 	150116061	*/

/*Create linked list for the threads type*/
struct read_thread
{
    int	thread_id;
    int assign;
    int count;
};
struct upper_thread
{
    int	thread_id;
    int assign;
    int count;
};
struct replace_thread
{
    int	thread_id;
    int assign;
    int count;
};
struct write_thread
{
    int	thread_id;
    int assign;
    int count;
};
/*Global Variables*/
FILE* file;
char * line = NULL;
size_t len = 0;
ssize_t read2;
char *readLine[256];
FILE* file1;
char * line1 = NULL;
/* End Global Variables for Read */
int upper=0;
int replace=0;
int writed=0;
int numberOfLine;/*find the how many line in the file*/
int numberOfReadThread;/*number of read thread*/
int numberOfUpperThread;/**number of upper thread*/
int numberOfReplaceThread;/*number of replace thread*/
int numberOfWriteThread;/*number of write thread*/
bool readable=false;
bool upperable=false;
bool replaceable=false;
bool writeable=false;
int lineNumber = 0;
int upperNumber = 0;
int replaceNumber= 0;
int writeNumber = 0;
pthread_mutex_t readMutex= PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t upperMutex= PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t replaceMutex= PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t writeMutex= PTHREAD_MUTEX_INITIALIZER;
sem_t sema;


/*Start Functions*/
/*Read the file*/
void *Read(void *threadid);
/*Change the content to uppercase*/
void *Upper(void *threadid);
/*Substitute all of the spaces with underscore characters*/
void *Replace(void *threadid);
/*Write to file*/
void *Write(void *threadid);
/*This function create a 4 type of thread*/
void create_thread(int argc, char* argv[]);
/*Read the input file*/
void readTxt(char *argv[]);
/*End Functions*/

int main(int argc, char* argv[])
{
	 if(strcmp(argv[1], "-d") == 0 && strcmp(argv[3], "-n") == 0 && argv[2] != NULL && argv[4] != NULL && argv[5] != NULL && argv[6] != NULL && argv[7] != NULL){
		printf("<Thread-type and ID> \t\t\t <Output>\n");
		readTxt(argv);
		create_thread(argc,argv);
        
	}else{/*uncorrect input*/
		fprintf(stderr,"Please enter the correct input");
	}
	
	return 0;
}
void create_thread(int argc, char* argv[]){
    /*Initialize the Numbers of Threads*/	
    numberOfReadThread = atoi(argv[4]);
    numberOfUpperThread = atoi(argv[5]);
    numberOfReplaceThread= atoi(argv[6]);
    numberOfWriteThread= atoi(argv[7]);
	
    char const* const fileName = argv[2]; /*assign to file name*/
    file1 = fopen(fileName, "r+"); /*We opened the file*/

    /*Initialize the Mutex*/
    pthread_mutex_init(&readMutex, NULL);
    pthread_mutex_init(&upperMutex, NULL);
    pthread_mutex_init(&replaceMutex, NULL);
    pthread_mutex_init(&writeMutex, NULL);

    sem_init (&sema,0,1);
    struct read_thread read_thread_array[numberOfReadThread];
    struct upper_thread upper_thread_array[numberOfUpperThread];
    struct replace_thread replace_thread_array[numberOfReplaceThread];
    struct write_thread write_thread_array[numberOfWriteThread];
    
    /*Create the threads array*/
    pthread_t readThreads[numberOfReadThread];
    pthread_t upperThreads[numberOfUpperThread];
    pthread_t replaceThreads[numberOfReplaceThread];
    pthread_t writeThreads[numberOfWriteThread];

	long i;
	int rc;
	void *status;
	/*create the threads*/
	for(i=0;i<numberOfReadThread;i++){
            read_thread_array[i].thread_id=i;
			rc = pthread_create(&readThreads[i],NULL, Read, (void*)&read_thread_array[i]);
			if (rc) {
            			fprintf(stderr, "ERROR; return code from pthread_create() is %d\n", rc);
    	        		exit(-1);
       			 }
	}
	for(i=0;i<numberOfUpperThread;i++){
		upper_thread_array[i].thread_id=i;
		rc = pthread_create(&upperThreads[i],NULL, Upper, (void *)&upper_thread_array[i]);
		if (rc) {//Error checking
		    fprintf(stderr, "ERROR; return code from pthread_create() is %d\n", rc);
		    exit(-1);
		}
        }
	for(i=0;i<numberOfReplaceThread;i++){
		replace_thread_array[i].thread_id=i;
		rc = pthread_create(&replaceThreads[i],NULL, Replace, (void *)&replace_thread_array[i]);
		if (rc) {//Error checking
		    fprintf(stderr, "ERROR; return code from pthread_create() is %d\n", rc);
		    exit(-1);
		}
        }
	/*wait the read thread*/
	for(i=0;i<numberOfReadThread;i++) {
		pthread_join(readThreads[i], &status);
	}

	for(i=0;i<numberOfWriteThread;i++){
       		 write_thread_array[i].thread_id=i;
        	 rc = pthread_create(&writeThreads[i],NULL, Write, (void*)&write_thread_array[i]);
        if (rc) {
                 fprintf(stderr, "ERROR; return code from pthread_create() is %d\n", rc);
                 exit(-1);
        	}
        }
	/*wait the threads*/
	for(i=0;i<numberOfWriteThread;i++) {
        pthread_join(writeThreads[i], &status);
        }
	for(i=0;i<numberOfUpperThread;i++) {
		pthread_join(upperThreads[i], &status);
	}
	for(i=0;i<numberOfReplaceThread;i++) {
		pthread_join(replaceThreads[i], &status);
	}
	
	


	/*Destroy the mutexes*/
	pthread_mutex_destroy(&readMutex);
	pthread_mutex_destroy(&upperMutex);
	pthread_mutex_destroy(&replaceMutex);
	pthread_mutex_destroy(&writeMutex);
        
	/*close the file*/
	fclose(file1);
	if (line1)
        	free(line1);
	/*exit threads*/
	pthread_exit(NULL);
}

void *Read(void *threadid) {
    
    struct read_thread *readData;
    /*pointer takes the id of the threads*/
    readData = (struct read_thread *) threadid;
    for(int z=0;z<100;z++){}
    while(!readable){
        char line1[256];
        pthread_mutex_lock(&readMutex);
        readData->assign = lineNumber;/* threads take line numbers and threads will read the read the this line number*/
        lineNumber++;
        if(lineNumber == numberOfLine){
            readable=true;/*if the line number reach the number of line exit from the loop */
        }
        pthread_mutex_unlock(&readMutex);
        fseek(file1, 0, SEEK_SET);
        for(readData->count = 0; readData->count <= readData->assign+1; readData->count++){
            fgets(line1, 256, file1);
            if(readData->count==readData->assign){
                readLine[readData->count]= strdup(line1);/*copy the line to array*/
		/*print the information about the thread and line*/
                printf("Read_%d\t\t\t Read_%d read the line %d which is %s",readData->thread_id + 1, readData->thread_id +1, readData->count+1, readLine[readData->count]);

            }
            line1[0] = '\0';
        }
    }
    /*threads exit from the function*/
    pthread_exit(NULL);
}
void *Upper(void *threadid) {

    struct upper_thread *upperData;
    /*pointer takes the id of the threads*/
    upperData = (struct upper_thread *) threadid;
    for(int z=0;z<1000;z++){}
    while(!upperable){

        pthread_mutex_lock(&upperMutex);
        upper++;
        if(upper == 1){
            pthread_mutex_lock(&replaceMutex);/*when the upper threads jobs start lock the replace threads*/
        }
        upperData->assign=upperNumber;/* threads take line numbers and threads will read the read the this line number*/
        upperNumber++;
        if(upperNumber == numberOfLine){
            upperable = true;/*if the line number reach the number of line exit from the loop */
        }
        upperData->count = 0;
        pthread_mutex_unlock(&upperMutex);
        int ind=0;
        int j=0;
        char *p;
        while(ind==0){
            if(readLine[upperData->assign] != NULL){
                if(upperData->count == upperData->assign){
                    p=readLine[upperData->assign];
                    while(p[j] != '\0'){/*when the pointer reach the end of line exit from the loop*/
                        if(p[j]>='a' && p[j] <= 'z'){
                            p[j]=p[j]-32;/*if the line has lowercase letter in the word change with the uppercase letter*/
                        }
                        j++;
                        ind = 1;
                        pthread_mutex_lock(&upperMutex);
                        upper--;
                        if(upper == 0){
                            pthread_mutex_unlock(&replaceMutex);/*unclock the replace threads*/
                        }
                        pthread_mutex_unlock(&upperMutex);
                    }
		    /*print the information about the line and threads*/
                    printf("Upper_%d\t\t\t\t Upper_%d read index %d and converted %s",upperData->thread_id+1, upperData->thread_id+1,upperData->assign+1, readLine[upperData->assign]);
                    j=0;
                }else{
                    upperData->count++;
                }
            }
        }
    }
    pthread_exit(NULL);
}
void *Replace(void *threadid){
  
    struct replace_thread *replaceData;
    /*pointer takes the id of the threads*/
    replaceData = (struct replace_thread *) threadid;
    for(int z=0;z<1000;z++){}
    while(!replaceable){

        pthread_mutex_lock(&replaceMutex);
        replace++;
        if(replace == 1){
            pthread_mutex_lock(&upperMutex);/*lock the upper threads*/
        }
        replaceData->assign=replaceNumber;/* threads take line numbers and threads will read the read the this line number*/
        replaceNumber++;
        if(replaceNumber == numberOfLine){
            replaceable = true;/*if the line number reach the number of line exit from the loop */
        }
        replaceData->count = 0;
        pthread_mutex_unlock(&replaceMutex);
        int ind=0;
        int j=0;
        char *p;
        char ch;
        while(ind==0){
            if(readLine[replaceData->assign] != NULL){
                if(replaceData->count == replaceData->assign){
                    p=readLine[replaceData->assign];
                    while(p[j] != '\0'){
                        ch = p[j];
                        if(p[j]== ' '){
                            p[j]='_';/*if the words has a empty spaces  this line will change with the underscore character*/
                        }
                        j++;/*walk in the lines*/
                        ind = 1;/*exit from the loop*/
                        pthread_mutex_lock(&replaceMutex);
                        replace--;
                        if(replace == 0){
                            pthread_mutex_unlock(&upperMutex);/*unlock the upper threads*/
                        }
                        pthread_mutex_unlock(&replaceMutex);
                    }/*print the information about hte lines and threads*/
                    printf("Replace%d\t\t\t Replace_%d read index %d and converted %s",replaceData->thread_id+1, replaceData->thread_id+1,replaceData->assign+1, readLine[replaceData->assign]);
                    j=0;

                }else{
                    replaceData->count++;/*if assign number not macth with count number then walk in the file*/
                }
            }
        }

    }

    pthread_exit(NULL);


}
void *Write(void *threadid){
   
    struct replace_thread *writeData;
    /*pointer takes the id of the threads*/
    writeData = (struct replace_thread *) threadid;
   
    while(!writeable){
        pthread_mutex_lock(&writeMutex);
            if(readLine[writeNumber] != NULL) {
                fseek(file1, writed, SEEK_SET);
		/*print array to file*/
                fprintf(file1, "%s", readLine[writeNumber]);
                fprintf(file1, "\n");
		/*print the information about the threads*/
                printf("Write%d\t\t\t\t Write_%d read index %d and written %s", writeData->thread_id + 1,writeData->thread_id + 1, writeNumber + 1, readLine[writeNumber]);
                writed = writed + strlen(readLine[writeNumber]);
                writeNumber++;

                if (writeNumber == numberOfLine) {
                    writeable = true;/*if the write number reach the number of line exit from the loop */
                }
            }
        pthread_mutex_unlock(&writeMutex);
    }
    pthread_exit(NULL);
}

void readTxt(char *argv[]){
    numberOfLine=0;
    char const* const fileName = argv[2]; /*assign to file name*/
    file = fopen(fileName, "r"); /*We opened the file*/
    if (file == NULL)
        exit(EXIT_FAILURE);
    while((read2 = getline(&line, &len, file)) != -1) {
        numberOfLine++;/*number of line in the file*/
    }
    /*close the file*/
    fclose(file);
    if (line)
        free(line);
}
