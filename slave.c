#include <stdio.h>
#include <time.h>
#include <sys/time.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "config.h"
#include <string.h>

char *ofile= "cstest";
FILE *file, *lfile;
char logfile[10]="logfile.";

enum state{idle,want_in,in_cs};
int turn;
enum state flag[MAX_PROCESS];

void printlogfile(int n) {
	char logNum[3];
	sprintf(logNum,"%d",n);
	strncat(logfile,logNum,2);
	lfile=fopen(logfile,"w");

	struct timeval  now;
        struct tm* local;
        gettimeofday(&now, NULL);
        local = localtime(&now.tv_sec);

	//Add what time enter and exit??
	fprintf(lfile,"%02d:%02d:%05d File modified by process number %d\n",local->tm_hour, local->tm_min, local->tm_sec,n);
	
	
	fclose(lfile);
}

void message(int n) {

       // FILE *file;
       file=fopen(ofile,"a");

        struct timeval  now;
        struct tm* local;
        gettimeofday(&now, NULL);
        local = localtime(&now.tv_sec);

        //Write to output file
        fprintf(file,"%02d:%02d:%05d File modified by process number %d\n",local->tm_hour, local->tm_min, local->tm_sec,n);
        fclose(file);
}

void process (const int i,int num) {
        int j;
        char numP[2]; //Process number
        
	do{ 
                do {
                        flag[i] = want_in;
                        j=turn;
                        while (j!=i) {
                                if (flag[j]!=idle) {
                                        j=turn;
                                } else {
                                        j=(j+1)%num;
                                }   
                        }   
                        //Declare intention to enter critical section
                        flag[i] = in_cs;

                        //Check that no one else is in critical section
                        for (j=0;j<num;j++){ 
                                if ((j!=i) && (flag[j]== in_cs)) {
                                        break;
                                }   
                        }   

                } while(j<num || (turn != i && flag[turn]!= idle));
                    
                //Enter critical section
                turn =i; 
		message(i);
		printlogfile(i);
                //Exit critical section
                j= (turn+1) %num;
                while (flag[j]==idle) {
                        j= (j+1)%num;

                }

                //Assign turn to next waiting process
                turn =j;
                flag[i]=idle;   
                
        }while(1);
}


int main(int argc, char *argv[]) {
	int numP=atoi(argv[0]);
	int nProcess=atoi(argv[1]);
	//message(n,numP);
	process(numP,nProcess);
	printf("Hello World\n");
   return 0;
}
