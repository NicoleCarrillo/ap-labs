#include <stdio.h>   
#include <stdlib.h>

int mystrlen(char *str){
    int contador=0;
    while (*str != '\0') {
        contador++,str++;
    }
    return contador;
}

char *mystradd(char *origin, char *addition){
    printf("%s\t%d\n","Initial Length:",mystrlen(origin));
    char new[mystrlen(origin)+mystrlen(addition)];
    int i=0;
    while (*origin!= '\0') {
        new[i]=*origin;
        origin++,i++;
    }
    while(*addition != '\0'){
        new[i]=*addition;
        addition++,i++;
    }
    new[i]='\0';
    printf("%s\t%s\n","New String:",new); 
    printf("%s\t%d\n","New length:",mystrlen(new));
    return 0;
}

int mystrfind(char *origin, char *substr){
    printf("%s%s%s","['",substr,"']"); 
    int pos=0;
    while (*origin!= '\0') {
        while(*origin==*substr){
            origin++,substr++;
            if(*substr=='\0'){
               printf("%s%s%d%s%s\n"," string was found at ","[",pos,"]"," position");  
               return 0;
            }
        }
        substr=&substr[0],pos++,origin++;
    }
    printf("%s\n","string was not found "); 
    return 0;
}