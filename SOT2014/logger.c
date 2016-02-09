//Alejandro Malagón López-Páez

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <err.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <time.h>
#include <signal.h>

void
tokenizar(char *cadena){
	char *parte[256];
	strtok_r(cadena,"\n", parte);
	cadena = parte[0];
}

void
manejador(int s){
	char *fecha; 
	time_t t;
	time(&t);
	fecha = (ctime(&t));
	tokenizar(fecha);
	printf("%s : Time out,no events\n", fecha);
	alarm(5);
}


int
main(int argc, char* argv[])
{
	char *fecha; 
	time_t t;
	char buf[1024];
	int fd, nr;
	
	unlink("/tmp/fifo");

	if(mkfifo("/tmp/fifo", 0664) < 0)
		err(1, "mkfifo");
	signal(SIGALRM,manejador);
	for(;;){
		time(&t);
		fecha = (ctime(&t));
		tokenizar(fecha);
		fprintf(stderr,"%s : waiting for clients\n", fecha);
		fd = open("/tmp/fifo", O_RDONLY);
		if(fd < 0) {
			err(1, "open");
		}
		time(&t);
		fecha = (ctime(&t));
		tokenizar(fecha);
		fprintf(stderr,"%s : ready to read events\n", fecha);
		for(;;){
			alarm(5);
			nr = read(fd, buf, sizeof buf - 1);
			if(nr < 0){
				err(1, "read");
			}
			if(nr == 0){
				break;
			}
			buf[nr] = 0;
			time(&t);
			fecha = (ctime(&t));
			tokenizar(fecha);
			fprintf(stderr, "%s: %s\n",fecha,buf);
			if(strcmp(buf, "bye\n") == 0){
				close(fd);
				exit(0);
			}
		}
		close(fd);
		alarm(0);
	}
	exit(0);
}
