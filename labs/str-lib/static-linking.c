#include <stdio.h>
#include <string.h> 

int mystrlen(char*);
char *mystradd(char*,char*);
int mystrfind(char*,char*);

int main(int argc, char **argv) {
    if(argc<4){
        printf("%s\n","FALTAN VALORES");
    }else{
        char *palabra=argv[2];
        char *palabra2=argv[3];
        char *lib=argv[1];
        if(strcmp(lib,"-add")==0){
            mystradd(palabra,palabra2);
        }else{
            mystrfind(palabra,palabra2);
        }
    }
    return 0;
}

