//Alejandro Malagón López-Páez, Grado en Tecnologías.

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
ficheroApply(){
	int valor;
	int borrado;

	valor = access("apply.output",F_OK);
	if (valor == 0){
		borrado = unlink("apply.output");
		if (borrado!=0)
			err(1,"apply.output\n");
	}
}

void
env(char *argumento[]){
	int correcto;
	int i;
	char path[256];
	char *myenv[] = {"/bin/", "/usr/bin/", "/usr/local/bin/"};
	for(i=0;i<=2;i++){
		memset(path,0,256);
		strcat(path,myenv[i]);
		strcat(path,argumento[1]);
		correcto = access(path,X_OK);
		if(correcto ==0){
			argumento++;
			execv(path,argumento);
			exit(EXIT_FAILURE);
		}
	}
	exit(EXIT_FAILURE);
}

void
buscarFichero(char *directorio, char *argumento[]){
	DIR *d;
	struct dirent *de;
	char *a;
	struct stat info;
	int pid;
	int fd;

	d = opendir(directorio);
	if(d == NULL) {
		fprintf(stderr,"no existe el directorio %s\n",directorio);
		exit(EXIT_FAILURE);
	}
	
	while((de = readdir(d)) != NULL) {
		
		stat(de->d_name,&info);	
		if((info.st_mode & S_IFMT) == S_IFREG){
			a= strstr(de->d_name,".txt");
			if((a != NULL) && (strlen(a)== strlen(".txt"))){
			
				pid = fork();
				switch(pid){
					case -1:
						err(1,"fork\n");
					case 0:
						fd = open(de->d_name,O_RDONLY);
						if(fd < 0){
							err(1,"%s\n",de->d_name);
						}
						dup2(fd,0);
						close(fd);
						fd = open("apply.output",O_WRONLY|O_APPEND|O_CREAT,0644);
						if(fd < 0){
							err(1,"apply.output\n");
						}
						dup2(fd,1);
						close(fd);
						env(argumento);
					default:
						wait(NULL);
				}
			}
		
		}		
	}
	closedir(d);
}

int
main(int argc, char *argv[]){
	
	ficheroApply();
	buscarFichero(".",argv);
	exit(EXIT_SUCCESS);

}




