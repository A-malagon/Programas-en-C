 
  
 #include <stdio.h>
 #include <stdlib.h>
 #include <string.h>
 
 
 int
 igual_posicion(int LongArray,char cadena[1024][1024]){
	char * palabra;
	int i;
	int j;
	int encontrado;
	printf("[");
	palabra = cadena[0];
	
		for(i=0;i< strlen(palabra); i++){
			encontrado =1;
			j=1;
			
			while((j< LongArray+1) && (encontrado==1)){
				
				if(palabra[i] != cadena[j][i]){
					encontrado = 0;
				}
				j++;	
			}
			
			if(encontrado==1){
				printf("%c",palabra[i]);
			}
		}
		
	 printf("]\n");
	 return encontrado;
 }	  
	
 int
 contar_letras(char *s,char c){
	 int i;
	 int veces=0;
	 i=0;
	 while(i<strlen(s)){
		 if (s[i]==c)
			veces++;
		 i++;
	 }
	 return veces;
	 
 }
 int
 es_anagrama(char *s1, char *s2){
	int ana=0;
	int i;
	if (s1!=NULL){
		if (s2!=NULL){
			if (strlen(s1)==strlen(s2)){
				ana=1;
				for(i=0;i<strlen(s1);i++){
					if (contar_letras(s1,s1[i])!=contar_letras(s2,s1[i])){
						ana=0;
						break;
					}
				}			
			}	
		}
		
	}
	return ana;
	
 }
 
 int 
 main(int argc, char *argv[]){
	int i;
	int j;
	char cadena[1024][1024];
	int es_ana;
	int x;
	int y;

	for(i=1;i< argc;i++){
		x=0;
		for((j=i+1);j<argc;j++){
			es_ana = es_anagrama(argv[i],argv[j]);
	
			if(es_ana ==1){
				strcpy(cadena[0],argv[i]);
				x= x+1;
				strcpy(cadena[x],argv[j]);
				argv[j]= NULL;
			}
		}
		
		if (x>=1){
			for(y=0;y<=x;y++){
				printf("%s ",cadena[y]); 
			}
			igual_posicion(x,cadena);
		
		}
	}	

	exit(EXIT_SUCCESS);
 }
