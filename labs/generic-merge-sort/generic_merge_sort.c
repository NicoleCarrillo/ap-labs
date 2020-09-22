
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

char *elements[10000];
int size = 0;
int numeric = 0;

void getElements(char *text);
int ncmp(char *s1,char *s2);
void mergeSort(void *elements[],int left,int right, int (*pFunction)(void* , void*));
void merge(void *elements[],int left,int middle,int right, int (*pFunction)(void *, void *));
void print();

int main(int argc,char **argv)
{
    if(argc<2){
        printf("ERROR: HANDLE MISSING PARAMETERS.\n");
        printf("Usage:./${APP_NAME}.o <inputFile>\n");
        return 0;
    }
    if(argc == 2 && strcmp(argv[1],"-n") == 0){
        printf("ERROR: HANDLE INCOMPLETE PARAMETERS.\n");
        printf("Usage:../${APP_NAME}.o -n <inputFile>\n");
        return 0;
    }
    if(argc == 3 && strcmp(argv[1],"-n") == 0){
        numeric = 1; //SE TRATA DE NUMEROS 
    }
    if(numeric == 0){ //SE TRATA DE LETRAS 
        getElements(argv[1]); //LETRAS
    }else{
        getElements(argv[2]); //NUMEROS
    }
    mergeSort((void *)elements,0,size-1,(int (*)(void*, void*))(numeric ? ncmp : strcmp)); //LE PASO UNA FUNCION COMO PARAMETRO PARA DEPENDIENDO EL TIPO SEPA QUE HACER
    print();
    return 0;
}

void mergeSort(void *elements[],int left,int right, int (*pFunction)(void *, void *)){ //APUNTADORES VOID LO HACEN GENERICO 
    int middle;
    if(left<right){
        middle = left + (right - left)/2;
        mergeSort((void *)elements, left, middle,(int (*)(void*, void*))(numeric ? ncmp : strcmp)); 
        mergeSort((void *)elements, middle + 1, right,(int (*)(void*, void*))(numeric ? ncmp : strcmp)); 
        merge((void *)elements, left, middle, right,(int (*)(void*, void*))(numeric ? ncmp : strcmp));
    }
}

void merge(void *elements[],int left,int middle,int right, int (*pFunction)(void *, void *)){
    int n1=middle-left+1;
    int n2=right-middle;
    void *L[n1];
    void *R[n2];
    for(int x=0;x<n1;x++){
        L[x]=elements[left+x];
    }
    for(int x=0;x<n2;x++){
        R[x]=elements[middle+1+x];
    }
    int x=0;
    int y=0;
    int z=left;
    while(x<n1&&y<n2){
        if((*pFunction)(L[x],R[y])<0){
            elements[z]=L[x];
            x++;
        }else{
            elements[z]=R[y];
            y++;
        }
        z++;
    }
    while(x<n1){
        elements[z]=L[x];
        x++;
        z++;
    }
    while(y<n2){
        elements[z]=R[y];
        y++;
        z++;
    }
}

void getElements(char *text){
    FILE *file;
    file = fopen(text,"r");
    int pos = 0;
    char characters[200] = {0};
    if (file == NULL) {
        perror("ERROR WHILE OPENING OR CREATING FILE");
        return;
    }
    while(fgets(characters,100,file)){
        elements[pos] = (char*)malloc(strlen(characters) + sizeof(char*));
        strcpy(elements[pos],characters);
        pos++;
        size++;
    }
    if(fclose(file)){
        printf("ERROR COULDN'T CLOSE THE FILE.\n");
    }
}

int ncmp(char *s1,char *s2){ //MISMA FUNCION QUE EL STRCMP PERO CON NUMEROS 
    double v1,v2;
    v1=atof(s1);
    v2=atof(s2);
    if(v1<v2){
        return -1;
    }else if(v1>v2){
        return 1;
    }else{
        return 0;
    }
}

void print(){
    for (int x = 0; x<size;x++){
        printf("%s",elements[x]);
    }
}
