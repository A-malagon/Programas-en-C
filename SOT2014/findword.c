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

void
repetir(char *palabra, char *directorio){
	char cadena[1024];
	char pathCopia[2048];
	struct stat info;
	char *path1;
	char *path2;
	DIR *d;
	struct dirent *de;
	int fd;


	d = opendir(directorio);
	if(d == NULL) {
		err(1,"%s",directorio);
	}
	while((de = readdir(d)) != NULL) {
		if((strcmp(de->d_name,".")!= 0) && (strcmp(de->d_name,"..")!= 0)){
			strcpy(pathCopia,directorio);
			path1 = strcat(pathCopia,"/");
			path2 = strcat(path1,de->d_name);
				
			stat(path2,&info);
			if((info.st_mode & S_IFMT) == S_IFDIR){
				repetir(palabra,path2);
			}
			if((info.st_mode & S_IFMT) == S_IFREG){
				fd=open(path2,O_RDONLY);
				if (fd<0){
					err(1,".");
				}

				read(fd,cadena,strlen(palabra));
				if (strcmp(cadena,palabra)== 0){
					printf("%s\n", path2);
				}
				close(fd);
			}
		}
	}
		closedir(d);

}


int
main(int argc, char* argv[])
{
	char * palabra;
	char *directorio;
	int i;
	char punto[]=".";

	palabra = argv[1];
	
	
	if (argc==2){
		repetir(palabra,punto);
	}
	
	else{
		i=2;
		while(i<argc){
			directorio = argv[i];
			repetir(palabra,directorio);
			i++;
		}
	}
	exit(EXIT_SUCCESS);
}

