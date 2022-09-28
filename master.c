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

#define MAX 21

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
	int option,seg_id, *seg_p;	
/*	if (argc<2) {
		fprintf(stderr,"%s: Error: Invalid command. Please type: ./master -h for HELP\n",argv[0]);
		exit(1);
	} else {
	       	if(argv[1][0]!='-'){
			fprintf(stderr,"%s: Error: Invalid command. Please type: ./master -h for HELP\n",argv[0]);
			exit(1);
		}	
		while ((opt = getopt(argc,argv, "ht:")) != -1) {
			switch (opt) {
				case 'h':
					printf("%s -h: Help menu\n",argv[0]);
					printf("Command: ./master -t ss n\n");
					printf("ss: Maximum time in seconds after which the process should terminate itself if not completed (Default: 100)\n");
					printf("n: The maximum processes that the program runs at a time\n");
					exit(1);
				case 't':
					n_process= atoi(argv[3]);
					sec=atoi(argv[2]);
					printf("Second:%d\n",sec);
					printf("Second:%d\n",n_process);
					break;
				case '?': 
					fprintf(stderr,"%s: Error: Unknown option %c\n",argv[0],optopt);
					exit(1);
			}
		}
	}

*/
	
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
					if(n_process >MAX) {
						fprintf(stderr,"%s: Error: Number of processes exceeds 20\n",argv[0]);
                				exit(1);
					} else {
					//MAIN CODE
						seg_id=createSharedMemory();
						printf("%d",seg_id);	
						seg_p=attachSharedMemory(seg_id);
						removeSharedMemory(seg_p,seg_id);

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

	if (n_process ==-1) {
		fprintf(stderr,"%s: Error: Missing number of processes\n",argv[0]);
		exit(1);
	}
	return 0;
}
