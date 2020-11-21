#include <stdio.h>
#include "logger.h"
#include <pthread.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>

#define NUM_THREADS 2000
#define N 2000

int NUM_BUFFERS;
long * result;
long **buffers;
char *nameFile;
pthread_mutex_t *mutexes;
pthread_t numThreads[NUM_THREADS];

long * readMatrix(char *filename);
long * getColumn(int col, long *matrix);
long * getRow(int row, long *matrix);
int getLock();
int releaseLock(int lock);
long dotProduct(long *vec1, long *vec2);
long * multiply(long *matA, long *matB);
int saveResultMatrix(long *result);

struct argsThread{
    int row;
    int col;
    long *matA;
    long *matB;
};

int saveResultMatrix(long *result){
    FILE *fileName;
    fileName = fopen(nameFile, "w+");
    if(fileName==NULL){
        return -1;
    }
    int size=N*N;
    for(int i = 0; i < size; i++) {
        fprintf(fileName, "%ld\n", result[i]);
    }
    fclose(fileName);
    return 0;
}

long multiplyPart2(struct argsThread *numbers){
    int bufferA=-1;
    int bufferB=-1;
    
    while(bufferA==-1 || bufferB==-1){
        if(bufferA==-1){
            bufferA=getLock();
        }if(bufferB==-1){
            bufferB=getLock();
        }
    }
    
    buffers[bufferA]=getRow(numbers->row,numbers->matA);
    buffers[bufferB]=getColumn(numbers->col,numbers->matB);
    
    long res=dotProduct(buffers[bufferA],buffers[bufferB]);
    
    free(buffers[bufferA]);
    free(buffers[bufferB]);
    free(numbers);
    
    releaseLock(bufferA);
    releaseLock(bufferB);
    
    return res;    
}

long *multiply(long *matA, long *matB){
    
    long *matrixR=malloc(N*N*sizeof(long));
   
    for(int i=0;i<N;i++){
        
        for(int j=0;j<N;j++){
            struct argsThread *argsThreadStruct=malloc(sizeof(struct argsThread));
            argsThreadStruct->matA=matA;
            argsThreadStruct->matB=matB;
            argsThreadStruct->row=i;
            argsThreadStruct->col=j;
            pthread_create(&numThreads[j],NULL,(void * (*) (void *))multiplyPart2,(void *)argsThreadStruct);
        }
        
        for(int j=0;j<NUM_THREADS;j++){
            void *result;
            pthread_join(numThreads[j],&result);
            matrixR[NUM_THREADS*j+i]=(long)result;
        }
    }

    return matrixR;
}

long dotProduct(long *vec1, long *vec2){
    long dotproduct=0;
    for(int x = 0;x < N; x++)
        dotproduct+=vec1[x]*vec2[x];
    return dotproduct;
}

int releaseLock(int lock){
    return pthread_mutex_unlock(&mutexes[lock]);
}

int getLock(){
    for(int x = 0; x < NUM_BUFFERS; x++){
        if(pthread_mutex_trylock(&mutexes[x])==0){
            return x;
        }   
    }
    return -1;
}

long * getRow(int row, long *matrix){
    long *rW=malloc(N*sizeof(long));
    int start=N*(row);
    for(int x = 0;x < N;x++){
        rW[x]=matrix[start+x];
    }
    return rW;
}

long * getColumn(int col, long *matrix){
    long *cL=malloc(N*sizeof(long));
    for(int x=0;x<N;x++){
        cL[x]=matrix[col];
        col+=N;
    }
    return cL;

}

long * readMatrix(char *filename){
    long *matrix=(long *)malloc(N*N*sizeof(long));
    int matrixFile;
    int line;
    int p=0; 
    int concat=0;
    char actualChar;
    char *charConcat=(char*) malloc(sizeof(long)*sizeof(char));
    
    if((matrixFile=open(filename,O_RDONLY))==-1){
        errorf("ERROR WHILE OPENING MATRIX FILE");
    }

    while((line=read(matrixFile,&actualChar,1)>0)){
        if(actualChar!='\n'){
            charConcat[concat++]=actualChar;
        }else{
            charConcat[concat++]='\0';
            concat=0;
            matrix[p++]=atol(charConcat);
        }
    }
    
    if(concat>0){
        matrix[p++]=atol(charConcat);
    }
    
    close(matrixFile);
    
    return matrix;


    // int tsm=0;
    // FILE *file =fopen(filename,"r");
    // if(f==NULL){
    //     exit(2);
    // }
    // char c;
    // while((c=fgetc(f))!=EOF){
    //     if(c=='\n')
    //         tsm++;
    // }
    // rewind(file);
    // long *matrix=malloc(size*sizeof(long));
    // int po=0;
    // while(fscanf(file,"%ld",&matrix[pos])!= EOF){
    //     pos++;
    // }
    // fclose(file);
    // return matrix;   

}

int main(int argc,char **argv){
    if(argc != 5){
        errorf("WRONG PARAMETERS -> ./${APP_NAME} -n NUMBER# -out NAME.txt");
        return -1;
    }else if (strcmp(argv[1], "-n") == 0 && strcmp(argv[3], "-out") == 0){
        NUM_BUFFERS = atoi(argv[2]);
        nameFile = argv[4];
    }else if(strcmp(argv[1], "-out") == 0 && strcmp(argv[3], "-n") == 0){
        NUM_BUFFERS = atoi(argv[4]);
        nameFile = argv[2];
    }else{
        errorf("RUN PROGRAM FAIL CHECK PARAMETERS -> ./${APP_NAME} -n NUMBER# -out NAME.txt");
        return -1;
    }
    buffers = malloc(NUM_BUFFERS * sizeof(long *));
    mutexes = malloc(NUM_BUFFERS * sizeof(pthread_mutex_t));

    for(int i = 0; i < NUM_BUFFERS; i++)
        pthread_mutex_init(&mutexes[i], NULL);

    long *matA = readMatrix("matA.dat");
    long *matB = readMatrix("matB.dat");
   
    warnf("Starting multiplication!!!");
    long *res = multiply(matA, matB);
   
    warnf("Starting saving results");
    saveResultMatrix(res);
    infof("Results saved with success!!");
   
    free(matA);
    free(matB);
    free(mutexes);
    free(buffers);

    return 0;
}
