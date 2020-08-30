#include <stdio.h>
#include <stdlib.h>
#define ARRAY_LENGHT 100

void reverse(char arreglo[],int lenght){
	char tmp;
    for (int x = 0;  x < lenght/2; x++) {
		tmp = arreglo[x];
		arreglo[x] = arreglo[lenght - x - 1];
		arreglo[lenght - x - 1] = tmp;
    }
	printf("\nReversed word: \n");
	printf("%s", arreglo);
	printf("\n\n");
}

int main(int argc, char **argv){
	int c;
	char arreglo[ARRAY_LENGHT];
	int contador=0;
    while ((c = getchar()) != EOF) {
    	if (c == '\n'){  //VALIDACIÓN
			printf("\nOriginal word: \n");
			printf("%s", arreglo);
       		reverse(arreglo,contador);
			for(int x=0 ; x<contador ; x++){ //VACIO MI ARRAY PARA VOLTEAR LA SIGUIENTE PALABRA 
				arreglo[x] = '\0';
			}
			contador=0; //REINICIO CONTADOR
    	}else{
    		arreglo[contador]=c; //GUARDO LAS LETRAS EN EL ARREGLO
    		contador++;
    	}
    }
	return 0;
}