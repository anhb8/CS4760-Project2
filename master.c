#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h> // #define
#include <ctype.h> //isdigit
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/ipc.h> //shared memory
#include <sys/shm.h> //shared memory
#include <time.h> //local time
#include <sys/time.h>
#include "config.h"
pid_t all_cProcess[MAX_PROCESS];
int seg_id, *seg_p;
/*enum state{idle,want_in,in_cs};
int turn;
enum state flag[MAX_PROCESS];

void process (int i, char * n) {
	int j;
	i=i+1;
	printf("%s",n);
	char numP[2]; //Process number
	int num=atoi(n); //Number of process
	sprintf(numP,"%d",i);
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
		printf("%s",numP);
		execl("slave", numP, NULL);

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
*/
//Allocate shared memory
int createSharedMemory() {
	//Create shared memory segment
	key_t SHM_KEY;
	int shmid;
	shmid=shmget(SHM_KEY, sizeof(int), 0644|IPC_CREAT);
	if (shmid == -1) {
      		perror("Error:shmid");
      		exit(1);
   	}
	
	return shmid;
}

//Attach the process to shared memory segment just created - pointer
int *attachSharedMemory(int shmid) {
	int *shmp;
	shmp=(int *) shmat(shmid, NULL, 0);
	if (shmp == (int *) -1) {
      		perror("Error:shmp");
      		exit(1);
   	}	
	return shmp;

}

//Deallocate shared memory
void removeSharedMemory(int *shmp, int shmid) {
	//Detach the process
	 if (shmdt(shmp) == -1) {
      		perror("Error: shmdt");
      		exit(1);
   	}

	//Remove shared memory segment
	if (shmctl(shmid, IPC_RMID, 0) == -1) {
      		perror("Error: shmctl");
     	 	exit(1);
   }
}

//Interrupt signal (^C) 
void siginit_handler () {
	printf("-Ctrl C triggered\n");
	removeSharedMemory(seg_p,seg_id);
	exit(1);

}

//Signal when the program runs more than time limit
void alarm_handler () {
	perror("Error: Exceed time limit");
	//fprintf(stderr,"Error: Exceed time limit\n");
	//Kill all processes if exceeds time limit
	for (int i=0; i<MAX_PROCESS; i++) {
        	kill(all_cProcess[i],SIGTERM);
        } 
       removeSharedMemory(seg_p,seg_id);
	exit(1);	
}
void forkProcess(int n_process,int sec) {
	char num[2];
	char nProcess[2];
	int timeout = 0;
	int child_done = 0;
	
	//pid_t * children = (int*) malloc(n * sizeof(pid_t));
	/*if(children == NULL){
		fprintf(stderr,"%s: ",prog);
		perror("Error:");
		exit(EXIT_FAILURE);
	}*/
		//printf("%d",n);
	sprintf(nProcess,"%d",n_process);	
	for (int i=0;i<n_process;i++) {
		sprintf(num,"%d",i);
		pid_t pid = fork();
		if (pid<0) {
			fprintf(stderr, "Error: Fork failed");
			//exit(EXIT_FAILURE);
			exit(1);
		} else if (pid == 0) {  	//Child process
			execl("slave",num,nProcess,NULL);
		} else {			//Parent process
			all_cProcess[i]=pid;

		}
	}
	while(wait(NULL) > 0);	
}

int validNum(char* sec){
	int size = strlen(sec);
	int i = 0;
	while(i < size){
		if(!isdigit(sec[i]))
			return 0;
		i++;
	}
	return 1;
}

int main(int argc, char *argv[])
{	
	int sec=100;
	int n_process=-1;
	int option;
	FILE *file;	
	signal(SIGINT, siginit_handler);
	
	while(optind < argc){
		if ((option = getopt(argc,argv, "ht:")) !=-1) {
			switch(option){
				case 'h':
					printf("%s -h: HELP MENU\n",argv[0]);
                                        printf("Command:\n");
					printf("./master -t ss n	Specify maximum time\n");
					printf("./master n              Default maximum time:100\n");
                                        printf("\nss: Maximum time in seconds after which the process should terminate itself if not completed (Default: 100)\n");
                                        printf("n: The maximum processes that the program runs at a time (From 1-20)\n");
					exit(1);

				case 't':
					if(validNum(optarg)){
						sec = atoi(optarg);
					}else{
						fprintf(stderr,"%s: ERROR: %s is not a valid number\n",argv[0],optarg);
						return EXIT_FAILURE;
					}
					break;
				case '?':
					if(optopt == 't')
						fprintf(stderr,"%s: ERROR: -%c without argument\n",argv[0],optopt);
					else
						fprintf(stderr, "%s: ERROR: Unrecognized option: -%c\n",argv[0],optopt);
					return EXIT_FAILURE;
			}
		} else {
			if(n_process == -1) {
				if(validNum(argv[optind])) {
					n_process = atoi(argv[optind]);	
					//printf("%d\n",n_process);
					//Validate if number of processes doesn't exceed 20
					if(n_process >MAX_PROCESS) {
						fprintf(stderr,"%s: Error: Number of processes exceeds 20\n",argv[0]);
                				exit(1);
					}

				} else {
                                       fprintf(stderr,"%s: ERROR: %s is not a valid number\n",argv[0],argv[optind]);
                                       return EXIT_FAILURE;
                                }

			}
			else{
				fprintf(stderr,"Error: There are too many file name\n");
				return EXIT_FAILURE;
			}
			optind++;		
		}
		
	}
	//printf("%d",n_process);
	signal(SIGALRM, alarm_handler);
        alarm(sec);

	if (n_process ==-1) {
		fprintf(stderr,"%s: Error: Missing number of processes\n",argv[0]);
		exit(1);
	} 
	//Validate if number of processes doesn't exceed 20
        else if (n_process >MAX_PROCESS || n_process <1) {
        	fprintf(stderr,"%s: Error: Number of processes needs to be from 1-20\n",argv[0]);
                exit(1);
        } 
	//MAIN CODE
	seg_id=createSharedMemory();
        seg_p=attachSharedMemory(seg_id);
 
	
	forkProcess(n_process,sec);
       	removeSharedMemory(seg_p,seg_id);
        
	return 0;
}
