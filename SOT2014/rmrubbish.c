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

int rubbishOK;
int hijosBien;
int hijosMal;


void
BorrarRubbish(char *path2,int pid){
	char *a;
	int valor;
	a= strstr(path2,".rubbish");
	if((a != NULL) && (strlen(a)== strlen(".rubbish"))){
		valor = unlink(path2);
		rubbishOK = 1;
		if(valor <0){
			fprintf(stderr,"%d: error removing %s\n",pid,path2);
			exit(EXIT_FAILURE);
		}
	}
}


void
repetir(char *directorio, int pid){
	char pathCopia[2048];
	struct stat info;
	char *path1;
	char *path2;
	DIR *d;
	struct dirent *de;


	d = opendir(directorio);
	if(d == NULL) {
		fprintf(stderr,"%d: no existe el directorio %s\n",pid,directorio);
	}
	while((de = readdir(d)) != NULL) {
		if((strcmp(de->d_name,".")!= 0) && (strcmp(de->d_name,"..")!= 0)){
			strcpy(pathCopia,directorio);
			path1 = strcat(pathCopia,"/");
			path2 = strcat(path1,de->d_name);
				
			stat(path2,&info);
			if((info.st_mode & S_IFMT) == S_IFDIR){
				repetir(path2,pid);
			}
			if((info.st_mode & S_IFMT) == S_IFREG){
				BorrarRubbish(path2,pid);
			}
		}
	}
		closedir(d);
}


void
RubbishEncontrado(int pid,char *directorio){
		if(rubbishOK ==1){
			fprintf(stderr,"%d: %s ok\n",pid,directorio);
			exit(EXIT_SUCCESS);
		}
		if(rubbishOK == 0){
			fprintf(stderr,"%d: no files to remove in dir %s\n",pid,directorio);
			exit(EXIT_FAILURE);
		}	
}


void 
EsperaPadre(int argc){
	int status;
	int i;
	for(i=1;i<argc;i++){
		wait (&status);
		if(status==0){
			hijosBien = hijosBien + 1;
		}
		else{
			hijosMal = hijosMal + 1;
		}
	}
}


void
EstadoProceso(int argc){
	if(hijosBien == (argc-1)){
		fprintf(stderr,"all processes were successful\n");
	}
	else{
		fprintf(stderr,"%d processes failed\n",hijosMal);
	}
}


int
main(int argc, char* argv[]){
	int i;
	int pid;
	
	if(argc == 1){
		fprintf(stderr,"error en numero argumentos,pasar al menos %d directorio.\n",argc);
		exit(EXIT_FAILURE);
	}
	
	for(i=1;i<argc;i++){
		pid = fork();
		if(pid<0){
			fprintf(stderr,"fork\n");
		}
		if(pid==0){
			pid = getpid();
			repetir(argv[i], pid);
			RubbishEncontrado(pid,argv[i]);
		}
	}
	EsperaPadre(argc);
	EstadoProceso(argc);
	exit(EXIT_SUCCESS);
}

