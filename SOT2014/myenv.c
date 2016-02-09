#include <stdio.h>
 #include <stdlib.h>
 #include <unistd.h>
 #include <sys/types.h>

 
 int 
 main(int argc, char *argv[]){
	 int pid;
	 int uid;
	 char * valor;
	 int i;
	 
	 pid = getpid();
	 uid = getuid();
	 
	 printf("UID: %d\n", uid);
	 printf("PID: %d\n", pid);
	
	for(i=1;i< argc; i++){
		valor= getenv(argv[i]);
		if(valor == NULL){
			fprintf(stderr,"error: %s does not exist\n", argv[i]);
		}
		else{
			printf("%s: %s\n",argv[i],valor);
		}
	}

	exit(EXIT_SUCCESS);
 }
