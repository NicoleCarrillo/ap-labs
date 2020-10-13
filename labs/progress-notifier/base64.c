#include <stdio.h>
#include "logger.h"
#include <inttypes.h>
#include <string.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#define WHITESPACE 64
#define EQUALS     65
#define INVALID    66
#define BUFFER_SIZE 10000

char *elements[10000];
int size = 0;
long full=0;    //^ variables for controlling progress
long progress=0;
int mystrlen(char *str);
char *substring(char *line,int start,int end);
//~ ENCODING AND DECODING FROM https://en.wikibooks.org/wiki/Algorithm_Implementation/Miscellaneous/Base64

//? BASE64 ENCONDING FUNCTION
int base64encode(const void* data_buf, size_t dataLength, char* result, size_t resultSize){
   const char base64chars[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
   const uint8_t *data = (const uint8_t *)data_buf;
   size_t resultIndex = 0;
   size_t x;
   uint32_t n = 0;
   int padCount = dataLength % 3;
   uint8_t n0, n1, n2, n3;
   full=dataLength; //^ progress

   //^increment over the length of the string, three characters at a time 
   for (x = 0; x < dataLength; x += 3) {
      //^ these three 8-bit (ASCII) characters become one 24-bit number 
      n = ((uint32_t)data[x]) << 16; //& parenthesis needed, compiler depending on flags can do the shifting before conversion to uint32_t, resulting to 0
      
      if((x+1) < dataLength)
         n += ((uint32_t)data[x+1]) << 8;//& parenthesis needed, compiler depending on flags can do the shifting before conversion to uint32_t, resulting to 0
      
      if((x+2) < dataLength)
         n += data[x+2];

      //^ this 24-bit number gets separated into four 6-bit numbers 
      n0 = (uint8_t)(n >> 18) & 63;
      n1 = (uint8_t)(n >> 12) & 63;
      n2 = (uint8_t)(n >> 6) & 63;
      n3 = (uint8_t)n & 63;
        
       //* if we have one byte available, then its encoding is spread
       //* out over two characters
      if(resultIndex >= resultSize) return 1;   //! indicate failure: buffer too small 
      result[resultIndex++] = base64chars[n0];
      if(resultIndex >= resultSize) return 1;   //! indicate failure: buffer too small 
      result[resultIndex++] = base64chars[n1];

       //* if we have only two bytes available, then their encoding is
       //* spread out over three chars
      if((x+1) < dataLength){
         if(resultIndex >= resultSize) return 1;   //! indicate failure: buffer too small 
         result[resultIndex++] = base64chars[n2];
      }

       //* if we have all three bytes available, then their encoding is spread
       //* out over four characters
      if((x+2) < dataLength){
         if(resultIndex >= resultSize) return 1;   //! indicate failure: buffer too small 
         result[resultIndex++] = base64chars[n3];
      }
      progress=progress+3;  //^ progress
      //sleep(1); 
   }
    //* create and add padding that is required if we did not have a multiple of 3
    //* number of characters available

   if (padCount > 0) { 
      for (; padCount < 3; padCount++) { 
         if(resultIndex >= resultSize) return 1;   //! indicate failure: buffer too small 
         result[resultIndex++] = '=';
      } 
   }
   if(resultIndex >= resultSize) return 1;   //! indicate failure: buffer too small 
   result[resultIndex] = 0;
   return 0;   //^ indicate success 
}

//? BASE64 DECONDING FUNCTION
static const unsigned char d[] = {
    66,66,66,66,66,66,66,66,66,66,64,66,66,66,66,66,66,66,66,66,66,66,66,66,66,
    66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,62,66,66,66,63,52,53,
    54,55,56,57,58,59,60,61,66,66,66,65,66,66,66, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9,
    10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,66,66,66,66,66,66,26,27,28,
    29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,45,46,47,48,49,50,51,66,66,
    66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,
    66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,
    66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,
    66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,
    66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,
    66,66,66,66,66,66
};

int base64decode (char *in, size_t inLen, unsigned char *out, size_t *outLen) { 
    char *end = in + inLen;
    char iter = 0;
    uint32_t buf = 0;
    size_t len = 0;
    full=inLen; //^progress  
    while (in < end) {
        unsigned char c = d[*in++];
        progress=progress+(long)in; //^ progress
        switch (c) {
        case WHITESPACE: continue;   /* skip whitespace */
        case INVALID:    return 1;   /* invalid input, return error */
        case EQUALS:                 /* pad character, end of data */
            in = end;
            continue;
        default:
            buf = buf << 6 | c;
            iter++; // increment the number of iteration
            //*If the buffer is full, split it into bytes 
            if (iter == 4) {
                if ((len += 3) > *outLen) return 1; //! buffer overflow 
                *(out++) = (buf >> 16) & 255;
                *(out++) = (buf >> 8) & 255;
                *(out++) = buf & 255;
                buf = 0; iter = 0;
            }   
        }
    }
    if (iter == 3) {
        if ((len += 2) > *outLen) return 1; //! buffer overflow 
        *(out++) = (buf >> 10) & 255;
        *(out++) = (buf >> 2) & 255;
    }
    else if (iter == 2) {
        if (++len > *outLen) return 1; //! buffer overflow 
        *(out++) = (buf >> 4) & 255;
    }
    *outLen = len; /* modify to reflect the actual output size */
    return 0;
}


void initEncoding(char* fileName){
    FILE *fileForRead;
    FILE *fileForWrite;
    if ((fileForRead = fopen(fileName,"r")) == NULL){
        errorf("FILE DOESN'T EXIST->%s", fileName); //! error with file
    }else{
        int l=mystrlen(fileName);
        char* newName=substring(fileName,0,l-4);
        char* newName2=strcat(newName,"-encoded.txt");
        fileForWrite = fopen(newName2,"w");

        size_t pos = ftell(fileForRead);
        fseek(fileForRead, 0, SEEK_END);
        size_t size_file = ftell(fileForRead);
        fseek(fileForRead, pos, SEEK_SET);
    
        char* data = malloc(size_file);
        fread(data, 1, size_file, fileForRead);

        char* code = (char*)malloc(sizeof(char) * strlen(data) * 100 * (BUFFER_SIZE));
   
        base64encode(data,size_file,code,sizeof(char) * strlen(data) * 100 * (BUFFER_SIZE));
        
        fprintf(fileForWrite,"%s",code);  
        if(fclose(fileForWrite)||fclose(fileForRead)){
            errorf("ERROR COULDN'T CLOSE THE FILE");    //! error with file
        }
        infof("FINISH ENCODING PROCESS");
        free(data);
        free(code);
    }
}

void initDecoding(char* fileName){
    FILE *fileForRead;
    FILE *fileForWrite;
    if ((fileForRead = fopen(fileName,"r")) == NULL){   //! error with file
        errorf("FILE DOESN'T EXIST->%s",fileName); //! error with file
    }else{
        int l=mystrlen(fileName);
        char* newName=substring(fileName,0,l-12);
        char* newName2=strcat(newName,"-decoded.txt");
        fileForWrite = fopen(newName2,"w");

        size_t pos = ftell(fileForRead);
        fseek(fileForRead, 0, SEEK_END);
        size_t size_file = ftell(fileForRead);
        fseek(fileForRead, pos, SEEK_SET);

        char* data = malloc(size_file );
        fread(data, 1, size_file, fileForRead);

        unsigned char* code = (unsigned char*)malloc(sizeof(char) * size_file);
        size_t outLen = sizeof(unsigned char *) * size_file;
    
        base64decode(data,size_file,code,&outLen);

        fprintf(fileForWrite,"%s",code);

        if(fclose(fileForWrite)||fclose(fileForRead)){
            errorf("ERROR COULDN'T CLOSE THE FILE");    //! error with file
        }
        infof("FINISH DECODING PROCESS");
        free(data);
        free(code);
    }
}

void sigHandler1(int sig){
    if (sig == SIGUSR1) {
        float pro=((float)(progress*100)/(float)full);
        infof("TOTAL: %d",full);
        infof("PROGRESS: %d",progress);
        infof("PRO %f %%",pro);
        infof("DATA REMAINING %d",full-progress);
    }
}

void sigHandler2(int sig){
    if (sig == SIGINT) {
        float pro=((float)(progress*100)/(float)full);
        infof("TOTAL: %d",full);
        infof("PROGRESS: %d",progress);
        infof("PRO %f %%",pro);
        infof("DATA REMAINING %d",full-progress);
    }
}

//& metodo de stringlib lab
char *substring(char *line,int start,int end){  //* METHOD FOR ADJUSTING A STRING 
    char *palabra=malloc(sizeof(char)*(end-start+1));
    for (int x = 0; x<end-start; x++){
        palabra[x] = line[x+start];
    }
    palabra[end-start] = '\0';
    return palabra;
}

//& metodo de stringlib lab
int mystrlen(char *str){  //* METHOD FOR GETTING THE LENGTH OF SOME WORD
    int contador=0;
    while (*str != '\0') {
        contador++,str++;
    }
    return contador;
}

int main(int argc, char *argv[]){
    initLogger("stdout");
    signal(SIGUSR1, sigHandler1);   //^ USER DEFINED SIGNAL 
    signal(SIGINT, sigHandler2);    //^ INTERRUPS THE PROCESS SIGNAL 
    if(argc != 3) {
        warnf("MISSING ARGUMENTS -> ./base64 [--encode,--decode] input.txt");
        return 0;
    }else if (strcmp(argv[1],"--encode")==0){
        infof("INITIATING ENCODING PROCESS");
        initEncoding(argv[2]);
    }else if (strcmp(argv[1],"--decode")==0){
        infof("INITIATING DECODING PROCESS");
        initDecoding(argv[2]);
    }else {
        warnf("REVIEW YOUR INPUT");
        return 0; 
    }
    return 0;
}