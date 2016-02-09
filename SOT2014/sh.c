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
int redireccionEntrada;
int redireccionSalida;
char *ficheroSalida;
char *ficheroEntrada;
char *directorio;


typedef struct TipoComando TipoComando;
	struct TipoComando{
		char *comando;
		char *argumentos[64];
	};

int
tokenizar(char *line,char *separador, char *almacen[]){
	int i = 0;
	char *comando;
	comando = strtok(line,separador);
	
	while(comando!= NULL){
		almacen[i] = comando;
		i= i + 1;
		comando = strtok(NULL,separador);
	}
	return i;
}


void
prompt(void){
	char directorioActual[1024];
	char ruta[1024];
	char *home;
	char *punteroCadena;
	
	getcwd(directorioActual,1024);
	home = getenv("HOME");
	
	punteroCadena = strstr(directorioActual,home);
	if (punteroCadena != NULL){
		punteroCadena = punteroCadena +strlen(home);
		strcpy(ruta,"~");
		strcat(ruta,punteroCadena);
	}else{
		strcpy(ruta,directorioActual);
	}	
	printf("myshell@Alex:%s$ ",ruta);
}

void 
buscar_ruta_comando(char *cmd, char *cmdsalida){
	
	char *path;
	char *rutas[64];
	char comando[128];
	int n;
	int i;
	char *aux;
	
	memset(cmdsalida,0,128);
	if (access(cmd,X_OK)==0){		
		strcpy(cmdsalida,cmd);
	}
	else{
		path=getenv("PATH");
		aux = strdup(path);

		n=tokenizar(aux,":",rutas);
		for(i=0;i<n;i++){
			sprintf(comando,"%s/%s",rutas[i],cmd);
			if (access(comando,X_OK)==0){
					strcpy(cmdsalida,comando);
				
			}	
		}
	}	
}

int  
cambiarDolar(char *arg[], int num_Argumentos){
	int i;
	char *aux;
	for(i=0;i<num_Argumentos;i++){
		if(arg[i][0] == '$'){
			aux = &arg[i][1];
			arg[i] = getenv(aux);
			if (arg[i]==NULL){
				fprintf(stderr,"error:var %s does not exist\n",aux);
				return 0;
			}
		}
	}
	return 1;
}



char
redirecciones(char *line){
	int num_tokens;
	char separadorSalida[] = ">\n";
	char separadorEntrada[] = "<\n";
	char separadorEspacio[] = " \t";
	char *partes[256];

	char *parteEspacioSalida[256];
	char *parteEspacioEntrada[256];
	
	redireccionSalida = 0;
	redireccionEntrada = 0;

	num_tokens = tokenizar(line,separadorSalida,partes);
	if(num_tokens == 2){
		ficheroSalida = partes[1];
		num_tokens = tokenizar(ficheroSalida,separadorEspacio,parteEspacioSalida);
		cambiarDolar(parteEspacioSalida,num_tokens);
			
		if(num_tokens == 1){
			ficheroSalida = parteEspacioSalida[0];
		}
		
		redireccionSalida= 1;
	}
	else{
		redireccionSalida= 0;

	}
	line= partes[0];

	num_tokens = tokenizar(line,separadorEntrada,partes);
	if(num_tokens == 2){
		ficheroEntrada = partes[1];
		num_tokens = tokenizar(ficheroEntrada,separadorEspacio,parteEspacioEntrada);
		cambiarDolar(parteEspacioEntrada,num_tokens);
		
		if(num_tokens == 1){
			ficheroEntrada = parteEspacioEntrada[0];
		}
		redireccionEntrada = 1;
	}
	else{
		redireccionEntrada=0;
	}
	line= partes[0];
	
	return *line;
	
}

void
HayRedireccionEntrada(){
	int fd;

	if(redireccionEntrada ==1){
		fd = open(ficheroEntrada,O_RDONLY);
		if(fd < 0){
			err(1,"error al abrir %s\n",ficheroEntrada);
		}
		dup2(fd,0);
		close(fd);
	}

}

void
HayRedireccionSalida(){
	int fd;
	if(redireccionSalida ==1){
		fd = open(ficheroSalida,O_WRONLY|O_CREAT|O_TRUNC,0644);
		if(fd < 0){
			err(1,"error al abrir %s\n",ficheroSalida);
		}
		dup2(fd,1);
		close(fd);
	}
}

void
redirigirEntrada_Salida(int i,int num_Comandos){
	if(i == 0){
		HayRedireccionEntrada();
	}

	if(i == num_Comandos -1){
		HayRedireccionSalida();
	}

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


int
cambiarDirectorio(char *line,TipoComando *comandos,char *almacen[]){
	int num_Argumentos;
	char *home;
	char copia[1024];
	int cd;
	int hayDirectorio;
		
		strcpy(copia,almacen[0]);
		num_Argumentos = tokenizar(copia," \t",comandos[0].argumentos);
		cambiarDolar(comandos[0].argumentos,num_Argumentos);

		comandos[0].argumentos[num_Argumentos] = NULL;
		comandos[0].comando = comandos[0].argumentos[0];
		if (comandos[0].comando==NULL)
			return 1;
		cd = strcmp(comandos[0].comando, "cd");
		if (cd == 0){
			if(comandos[0].argumentos[1] == NULL){
				home = getenv("HOME");
				chdir(home);
				directorio = home;
			}else{
				hayDirectorio = chdir(comandos[0].argumentos[1]);
				if(hayDirectorio != 0){
					fprintf(stderr,"No existe el directorio.\n");
				}
				directorio = comandos[0].argumentos[1];
			}
		}
		return cd;
}

int
variablesEntorno(char *line,int hayVariableEntorno){
	int num_Argumentos;
	char *parte[256];
	char * variable;
	char *valor;
	int num_tokens;
	char separadorEspacio[] = " \t";
	char *parteIzq[256];
	char *parteDer[256];
	
	num_Argumentos = tokenizar(line,"=\n",parte);
	if(num_Argumentos == 2){
		variable = parte[0];
		num_tokens = tokenizar(variable,separadorEspacio,parteIzq);
		if(num_tokens == 1){
			variable = parteIzq[0];
		}
		valor = parte[1];
		num_tokens = tokenizar(valor,separadorEspacio,parteDer);
		if(num_tokens == 1){
			valor = parteDer[0];
		}		
		setenv(variable,valor,1);
		hayVariableEntorno = 1;		
	}else{
		hayVariableEntorno = 0;
	}
	return hayVariableEntorno;
}


int
Background(char *line,int hayBackground){
	int num_tokens;
	char * parte[256];
	num_tokens = tokenizar(line,"&",parte);
	if(num_tokens == 2){
		hayBackground = 1;
	}else{
		hayBackground = 0;
	}
	return hayBackground;
	
}

void
leerLinea(TipoComando *comandos){
	char *Linea;
	char line[2048];
	char *almacen[256];
	char comando[128];
	int num_Comandos;
	int i;
	int num_Argumentos;
	int pid;
	int num_pipes;
	int cd;
	int e;
	int hayVariableEntorno = 0;
	int hayBackground = 0;
	int pids[128];
	int fd;
	int encontrado;
	
	Linea = fgets(line,2048,stdin);//Lee el tercer parámetro significa leer de la entrada estándar.Lee una línea.
	if (Linea == NULL){
		exit(EXIT_SUCCESS);
	}
	if(Linea != NULL && strlen(Linea)!=1){
		hayBackground = Background(line,hayBackground);
		hayVariableEntorno= variablesEntorno(line,hayVariableEntorno);
		
		*line = redirecciones(line);
		
		num_Comandos = tokenizar(line,"|\n",almacen);
		if(num_Comandos == 1){
			cd=cambiarDirectorio(line,comandos,almacen);
		}else{
			cd=1;
		}
		
		if(hayVariableEntorno != 1){
			if(cd != 0){
				num_pipes= num_Comandos -1;
				crearPipes(num_pipes);
				for(i=0;i<num_Comandos;i++){
					pid = fork();
					pids[i] = pid;
					switch(pid){
						case -1:
							err(1,"fork\n");
						case 0:
						
							if ((i==0) && (hayBackground == 1) && (redireccionEntrada == 0)){
									fd=open("/dev/null",O_RDONLY);
									if(fd<0)
										err(1,"/dev/null");
									dup2(fd,0);
									close(fd);
							}
							num_Argumentos = tokenizar(almacen[i]," \t",comandos[i].argumentos);
							e=cambiarDolar(comandos[i].argumentos,num_Argumentos);
							if (e==0)
								exit(EXIT_FAILURE);
							comandos[i].argumentos[num_Argumentos] = NULL;
							comandos[i].comando = comandos[i].argumentos[0];
							
							unirPipes(i,num_Comandos);

							redirigirEntrada_Salida(i,num_Comandos);

							cerrarPipes(num_pipes);
							
							if (comandos[i].comando==NULL)
								exit(EXIT_FAILURE);
							
							buscar_ruta_comando(comandos[i].comando,comando);
							if (strlen(comando)==0){
								fprintf(stderr,"%s: no se encontró la orden\n",comandos[i].comando);
								exit(EXIT_FAILURE);
							}
							
							
							execv(comando,comandos[i].argumentos);
							exit(EXIT_FAILURE);
					}
				}
				
				cerrarPipes(num_pipes);
				
				if (hayBackground == 0){

					encontrado=0;
					while(encontrado!= num_Comandos){
						pid=wait(NULL);
						if (pid<0){
							break;
						}
						for(i=0;i<num_Comandos;i++){
							if (pids[i]==pid){
								encontrado++;
							}
						}
					}
				}
				
			}
		}
	}
	
}

int
main(int argc, char *argv[]){
	 
	TipoComando comandos[256];
	
	memset(comandos,0,sizeof(TipoComando)*256);

	for(;;){
		prompt();
		leerLinea(comandos);
	}
	exit(EXIT_SUCCESS);
}
