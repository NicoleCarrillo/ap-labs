#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>

int installed,removed,upgraded,cont; //VARIABLES FOR GENERAL COUNTS 
char* date="-";
char* name="-";
char* status="-";

struct Node{
    char *pName;
    char *pInstall;
    char *pUpdate;
    int contador;
    char *pRemove;
};
struct Node list[2000]; //"LIST" CON "NODES" FOR SAVING INDIVIDUAL PROGRAMS 

void analizeLog(char *logFile, char *report);
void writeGeneralReport(char *file);
void writeResumeReport(int id);
int mystrlen(char *str);
void readLine(char *line);
char *substring(char *line,int start,int end);
char* getLine(int id);

int main(int argc, char **argv) {
    cont=0;
    if (argc < 5 || strcmp(argv[1],"-input") != 0 || strcmp(argv[3],"-report") != 0) {
        printf("ERROR Usage:./pacman-analizer.o -input <inputFile> -report <destinationFile>\n");
        return 0;
    }
    analizeLog(argv[2], argv[4]);
    return 0;
}

int mystrlen(char *str){  //METHOD FOR GETTING THE LENGTH OF SOME WORD
    int contador=0;
    while (*str != '\0') {
        contador++,str++;
    }
    return contador;
}

char *substring(char *line,int start,int end){  //METHOD FOR ADJUSTING A STRING 
    char *palabra=malloc(sizeof(char)*(end-start+1));
    for (int x = 0; x<end-start; x++){
        palabra[x] = line[x+start];
    }
    palabra[end-start] = '\0';
    return palabra;
}

void readLine(char *line){   //READ LINE BY LINE 
    int startn=0;
    int endn=0;
    int flag=0;
    if(line[0] == '['){ 
        startn=1;
        endn=1;
        while (line[endn]!=']'){
            endn++;
        }
        date=substring(line,startn,endn); //FIRST THE DATE
        endn++;
        endn++;
    }
   while(line[endn]!= ' '){
        endn++;
    }
    endn++;
    startn=endn;
    if(startn+7<mystrlen(line)){ //SECOND THE STATUS 
        if (line[startn] == 'i' && line[startn + 1] == 'n' && line[startn + 2] == 's' && line[startn + 3] == 't'){
            endn=10+startn;
            status="installed";
            flag=1;
        }
        else if (line[startn] == 'u' && line[startn + 1] == 'p' && line[startn + 2] == 'g' && line[startn + 3] == 'r'&& line[startn + 4] == 'a'&& line[startn + 5] == 'd'&& line[startn + 6] == 'e'&& line[startn + 7] == 'd'){
            endn=9+startn;
            status="upgraded";
            flag=1;
        }
        else if (line[startn] == 'r' && line[startn+ 1] == 'e' && line[startn + 2] == 'm' && line[startn + 3] == 'o'&& line[startn + 4] == 'v'&& line[startn + 5] == 'e'&& line[startn + 6] == 'd'){
            endn=8+startn;
            status="removed";
            flag=1;
        }else if (line[startn] == 'r' && line[startn+ 1] == 'e' && line[startn + 2] == 'i' && line[startn + 3] == 'n'&& line[startn + 4] == 's'&& line[startn + 5] == 't'&& line[startn + 6] == 'a'){
            endn=12+startn;
            status="reinstalled";
            flag=1;
        }
    }
    startn=endn;
    while(line[endn]!= ' '){
        endn++;
    }
    if(flag==1){
        name=substring(line,startn,endn); //THIRD THE NAME 
    }else{
        name="-";
    }
}

void writeResumeReport(int id){
    char* sentence = (char*)malloc(sizeof(char)*35);
    write(id, "Pacman Packages Report\n", 24);
    write(id, "----------------------\n", 24);
    sprintf(sentence, "- Installed packages : \t%d\n",installed);
    write(id, sentence, mystrlen(sentence));
    sprintf(sentence, "- Removed packages : \t%d\n", removed);
    write(id, sentence, mystrlen(sentence));
    sprintf(sentence, "- Upgraded packages : \t%d\n", upgraded);
    write(id,sentence,mystrlen(sentence));
    sprintf(sentence, "- Current installed : \t%d\n", installed-removed);
    write(id, sentence, mystrlen(sentence));
    write(id, "\n", 2);
    write(id, "\nList of packages\n", 16);
    write(id, "\n", 2);
    write(id, "----------------\n", 16);
    write(id, "\n", 2);
    for(int x=0;x<installed;x++){
        if(list[x].pName!=NULL){
            sprintf(sentence, "- Package Name : \t%s\n",list[x].pName);
            write(id, sentence, mystrlen(sentence)+1);
            sprintf(sentence, "    - Install date : \t%s\n",list[x].pInstall);
            write(id, sentence, mystrlen(sentence)+1);
            sprintf(sentence, "    - Last update date :%s\n",list[x].pUpdate);
            write(id, sentence, mystrlen(sentence)+1);
            sprintf(sentence, "    - How many updates :%d\n",list[x].contador);
            write(id, sentence, mystrlen(sentence)+1);
            sprintf(sentence, "    - Removal date : \t%s\n",list[x].pRemove);
            write(id, sentence, mystrlen(sentence)+1);
        }
    }
    free(sentence);
}

void writeGeneralReport(char *file){
    int fileID = open(file, O_RDWR | O_CREAT | O_TRUNC, 0644);
    if (fileID == -1){
        printf("%s%s\n","AN ERROR OCURRED WHILE TRYING TO CREATE FILE: ",file);
        exit(EXIT_FAILURE);
    }
    writeResumeReport(fileID); //RESUME REPORT OF ALL PROGRAMS 
    close(fileID);
}

char* getLine(int id){
    int acumulator = 0;
    int numL;
    int size = 100;
    char *line;
    line = calloc(size, sizeof(char));
    while((numL = read(id, line+acumulator, size)) > 0){
        for(int x=acumulator;x<acumulator+numL;x++){
            if(line[x] == '\n'){
                lseek(id, x-(acumulator+numL)+1, SEEK_CUR);
                line[x] = '\0';
                return line;
            }
        }
        acumulator+=numL;
        line = realloc(line, acumulator+size);
    }
    if(acumulator>0){
        line[acumulator] = '\0';
        return line;
    }
    return NULL;
}

void analizeLog(char *logFile, char *report) {
    printf("Generating Report from: [%s] log file\n", logFile);
    // Implement your solution here.
    int id = open(logFile, O_RDONLY);
    if (id < 0) {
        perror("ERROR WHILE OPENING OR CREATING FILE");
        return;
    }
    char *line;
    line=getLine(id);
    while ((line) != NULL){
        readLine(line);
        if(strcmp(status,"installed")==0&&strcmp(name,"-")>0){
            list[cont].pName=name;
            list[cont].pInstall=date;
            list[cont].pUpdate="-";
            list[cont].pRemove="-";
            cont++;
            installed++;
        }else if(strcmp(status,"upgraded")==0&&(strcmp(name,"-")>0||strcmp(name,"-")<0)){
            for(int x=0;x<cont;x++){
                if(strcmp(list[x].pName,name)==0){
                    list[x].pUpdate=date;
                    list[x].contador++;
                    upgraded++;
                    break;
                }
            }
        }else if(strcmp(status,"removed")==0&&(strcmp(name,"-")>0||strcmp(name,"-")<0)){
            for(int x=0;x<cont;x++){
                if(strcmp(list[x].pName,name)==0){
                    list[x].pRemove=date;
                    removed++;
                    break;
                }
            }
        }
        else if(strcmp(status,"reinstalled")==0&&(strcmp(name,"-")>0||strcmp(name,"-")<0)){
            list[cont].pName=name;
            list[cont].pInstall="-";
            list[cont].pUpdate="-";
            list[cont].pRemove="-";
            cont++;
        }
        line=getLine(id);
    }
    close(id);
    writeGeneralReport(report);
    printf("Report is generated at: [%s]\n", report);
}