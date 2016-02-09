//Alejandro Malagón López-Páez

#include <stdio.h>
#include <stdlib.h>
#include <sys/dir.h>
#include <dirent.h>
#include <err.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>


void
escribir(char *argumento,int p[]){
	int fd;
	int leidos;
	char buffer[2048];
	
	fd=open(argumento,O_WRONLY|O_CREAT|O_TRUNC,0664);
	if(fd < 0){
		err(1,"%s\n",argumento);
	}
	close(p[1]);
	
	for(;;){
		leidos = read (p[0],buffer,sizeof(buffer));
		if (leidos == 0)
			break;
		write(1,buffer,leidos);
		write(fd,buffer,leidos);
	}
	close(fd);
	close(p[0]);
	wait(NULL);
}

void
crearHijos(int p[]){
	int pid;
	pipe(p);
	pid = fork();
	switch(pid){
		case -1:
			err(1,"fork\n");
		case 0:
			dup2(p[1],1);
			close(p[1]);
			close(p[0]);
			execlp("gunzip","gunzip",NULL);
			exit(EXIT_FAILURE);

	}
}


int
main(int argc, char *argv[]){
	int p[2];
	
	char *argumento;
	argumento = argv[1];
	
	crearHijos(p);
	escribir(argumento,p);

	exit(EXIT_SUCCESS);

}
