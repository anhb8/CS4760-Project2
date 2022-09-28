#include <stdio.h>
#include <time.h>
#include <sys/time.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>

char *ofile= "cstest";
void forkProcess(int n) {
    
        FILE *file;
        file=fopen(ofile,"w");

        struct timeval  now;
        struct tm* local;
        gettimeofday(&now, NULL);
        local = localtime(&now.tv_sec);

	

	//Write to output file
        fprintf(file,"%02d:%02d:%02d File modified by process number xx\n",local->tm_hour, local->tm_min, local->tm_sec);      

        fclose(file);
}

int main(int argc, char *argv[]) {
   	//printf("%s\n",argv[0]);
	int n=atoi(argv[0]);
	forkProcess(n);

	printf("Hello World\n");
   return 0;
}
