//El array de punteros apuntará a la zona de memoria apuntada por i, cada vez que vaya avanzando, esta permanecerá en la primera posición
//a la que apuntaba i,hasta se encuentre un carácter separador en el cual meterá un /0 en ese caso apuntará a la nueva zona
//de memoria apuntada por i,el programa salta caracteres separadores y terminará al encontrar un /0.
#include <stdio.h>
#include <stdlib.h>

int mytokenize(char *str, char **args, int maxargs){//char *s es un puntero de caracteres, es decir un puntero que apunta a un caracter
												  //char **args es un array de punteros que apunta a un puntero
												  //int maxargs es el numero máximo de palabras que podemos tener
	int i=0;
	int palabras=0;
	
	while((str[i]==' ') || (str[i]=='\n') || (str[i]=='\r') || (str[i]=='\t')){
		i++;
	}
	
	if(palabras<maxargs){
		args[palabras] = &str[i];//El array de punteros a punta a la zona de memoria donde está apuntando la variable i,
								 //no apunta al caracter ó letra sino a la zona zona de memoria
		palabras++;
	}
	else{
		str[i]= '\0';
	}	
	
	while (str[i]!='\0'){
		if((str[i]!=' ') && (str[i]!='\n') && (str[i]!='\r') && (str[i]!='\t')){
			i++;
		}
		else{
			str[i]='\0';
			i++;
			if(str[i]!='\0'){
				while((str[i]==' ') || (str[i]=='\n') || (str[i]=='\r') || (str[i]=='\t')){
					i++;
				}
				if(str[i]!='\0'){
					if(palabras<maxargs){
						args[palabras]=&str[i];
						palabras++;	
					}
					else{
						str[i]= '\0';
					}	
				}
			}
		}
	}
	printf("Número de palabras: %d \n", palabras);
	return palabras;
}
 
 int main(void){
	 char str[]= "   hola	 	alex	  que tal 		estas";//Cadena de caracteres
	 char *args[32]; //array de punteros de 32 zonas de memoria.
	 int n;
	 int i;
	 
	 n= mytokenize(str,args,5);//Llamada a mi función mytokenize
	 for(i=0;i<n;i++){
		printf("Palabra %d:%s\n",i,args[i]); //Imprime por pantalla cada palabra
	 }	
		exit(EXIT_SUCCESS);
 }
