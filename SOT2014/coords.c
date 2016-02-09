#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <err.h>


typedef struct TipoCoordenada TipoCoordenada;
	struct TipoCoordenada{
		int x;
		int y;
	};

typedef struct TipoLista TipoLista;
	struct TipoLista{
		TipoCoordenada coordenada;
		TipoLista * siguiente;
	};
	
TipoLista *lista = NULL;


void
insertarPrincipio(int x,int y){
	TipoLista *nuevoNodo;
	nuevoNodo = malloc(sizeof(TipoLista));
	nuevoNodo-> coordenada.x=x;
	nuevoNodo-> coordenada.y=y;
	nuevoNodo-> siguiente= NULL;
	if(lista == NULL){
		lista = nuevoNodo;
	}
	else{
		nuevoNodo->siguiente = lista;
		lista= nuevoNodo;
	}
}

void
imprimirCoordenadas(void){
	TipoLista *paux;
	paux= lista;
	
	while(paux!=NULL){
		printf("(%d,%d)\n",paux->coordenada.x,paux->coordenada.y);
		paux=paux->siguiente;
	}
}

void
Borrar(void){
	TipoLista *paux;
	paux= lista;
	
	while(lista!=NULL){
		paux=lista;
		lista= paux->siguiente;
		free(paux);
	}
}
 
 int main(int argc, char *argv[]){

	int i;
	int numero;
	int argumento1;
	int x;
	int y;
	int fd;
	int leido;
	int nw;
	TipoCoordenada coord;

	if(argc == 3){
		numero = atoi(argv[2]);
		argumento1 = strcmp(argv[1],"-w"); 	
		if(argumento1 == 0){		
			for(i=0;i<=numero;i++){
				nw = write(1,&i,sizeof(int));
				if(nw != sizeof(int))
					err(1, "write %s", "/dev/tty");
				nw = write(1,&i,sizeof(int)); 
				if(nw != sizeof(int))
					err(1, "write %s", "/dev/tty");
			}
		}
	}
	
	if(argc == 2){
		
		fd=open(argv[1],O_RDONLY);
		if (fd<0)
			err(1,"%s\n",argv[1]);
		for(;;){	
			
			leido= read(fd,&coord,sizeof(TipoCoordenada));

			if(leido > 0){
				x=coord.x;
				y=coord.y;
				insertarPrincipio(x,y);
			}
			else{
				break;
			}
		}
		imprimirCoordenadas();
		Borrar();
		close(fd);
		
	}	
	if(argc == 1){

		for(;;){
			leido= read(0,&coord,sizeof(TipoCoordenada));

			if(leido > 0){
				x=coord.x;
				y=coord.y;
				insertarPrincipio(x,y);
			}
			else{
				break;
			}
		}
		imprimirCoordenadas();
		Borrar();
	}	
		
		exit(EXIT_SUCCESS);
 }


