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

enum{N=1024};
int p[N][2];
int argumentoEntrada;
int argumentoSalida;
char *ficheroLectura;
char *ficheroEscritura;
int HayRedireccionEntrada;
int HayRedireccionSalida;

	
void
RedireccionEntrada(char *fichero){
	int fd;
		fd = open(fichero,O_RDONLY);
		if(fd < 0){
			err(1,"error al abrir %s\n",fichero);
		}
		dup2(fd,0);
		close(fd);
}

void
RedireccionSalida(char *fichero){
	int fd;
		fd = open(fichero,O_WRONLY|O_CREAT|O_TRUNC,0644);
		if(fd < 0){
			err(1,"error al abrir %s\n",fichero);
		}
		dup2(fd,1);
		close(fd);
}

void
crearPipes(int num_pipes){
	int i;
	for(i=0;i<num_pipes;i++){
		pipe(p[i]);
	}
}

void
unirPipes(int i,int num_Comandos){
	if (num_Comandos > 1){
		if(i == 0){
			dup2(p[0][1],1);
		}else if(i == num_Comandos){
			dup2(p[i-1][0],0);
		}else{
			dup2(p[i][1],1);
			dup2(p[i-1][0],0);
		}
	}
}

void
cerrarPipes(int num_pipes){
	int j;
	for(j=0;j<num_pipes;j++){
		close(p[j][0]);
		close(p[j][1]);
	}
}


void
redirigir(int i,int num_Comandos){
	
	
	if (i==0){
		if(HayRedireccionEntrada == 1){
			RedireccionEntrada(ficheroLectura);
		}
	}
	if (i==num_Comandos-1){
		if(HayRedireccionSalida == 1){
			RedireccionSalida(ficheroEscritura);
		}
	}

}

void
controlArgumentos(int argc,char *almacen[],char *argv[]){
	int i;
	int j=0;
	int a;
	int b;
	
	for(i=1;i< argc;i++){
		argumentoEntrada = strcmp(argv[i],"-i"); 
		argumentoSalida = strcmp(argv[i],"-o");
		if(argumentoEntrada == 0){
			a = i+1;
			ficheroLectura = argv[a];
			if(i != argc-2){
				i++;
			}
			HayRedireccionEntrada = 1;
		}else if(argumentoSalida == 0){
			b = i+1;
			ficheroEscritura = argv[b];
			if(i != argc-2){
				i++;
			}
			HayRedireccionSalida = 1;
		}else{
			almacen[j]= argv[i]; 
			j= j+1;
		}
	}
}

void
ejecutarComandos(int argc,char *argv[]){
	int i;
	int pid;
	int num_pipes;
	char *almacen[256];
	int num_Comandos;

	controlArgumentos(argc,almacen,argv);
	
	num_Comandos = argc - 5;
	num_pipes= num_Comandos -1;
	crearPipes(num_pipes);
		
	for(i=0;i<num_Comandos;i++){
		pid = fork();
		switch(pid){
			case -1:
				err(1,"fork\n");
			case 0:
				
				unirPipes(i,num_Comandos);
				redirigir(i,num_Comandos);
				cerrarPipes(num_pipes);
				execlp(almacen[i],almacen[i],NULL);
				fprintf(stderr,"%s: command not found\n",almacen[i]);
				exit(EXIT_FAILURE);
		}
	}
		
	cerrarPipes(num_pipes);

	for(i=0; i< num_Comandos;i++){
		wait(NULL);
	}
}

int
main(int argc, char *argv[]){
	
	ejecutarComandos(argc,argv);
	
	exit(EXIT_SUCCESS);

}
