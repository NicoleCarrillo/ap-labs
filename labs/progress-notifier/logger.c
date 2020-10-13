#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <syslog.h>
#include <time.h>
#include "logger.h"  
#define RESET		0
#define BRIGHT 		1
#define BLACK 		0
#define RED		    1
#define YELLOW		3
#define MAGENTA		5
#define CYAN		6
#define	WHITE		7
int type =1;
void textcolor(int attr, int fg, int bg);
int initLogger(char *logType);
char* print_time();

int initLogger(char *logType) {
    if(strcmp(logType,"stdout")==0||strcmp(logType,"")==0){
        type = 1;
    }else if(strcmp(logType,"syslog")==0){
        type = 2;
    }
    printf("Initializing Logger on: %s\n", logType);
    return 0;
}

int infof(const char *format, ...){
    printf("\n");
    textcolor(BRIGHT, CYAN, BLACK);	
    printf("%s",print_time()); 
    va_list arg;
    va_start (arg, format);  
    if(type == 1){
        vprintf (format, arg);
    }else{
    	textcolor(BRIGHT, CYAN, BLACK);
        openlog("logger", LOG_PID, LOG_USER);		
        vsyslog(LOG_INFO, format, arg);
        closelog();
    }  
    va_end (arg);
    printf("\n");
	textcolor(RESET, WHITE, BLACK);
    return 0;
}

int warnf(const char *format, ...){
    printf("\n");
    textcolor(BRIGHT, YELLOW, BLACK);
    printf("%s",print_time());	
	va_list arg;
    va_start (arg, format);
    if(type == 1){
        vprintf (format, arg);
    }else{
        openlog("logger", LOG_PID | LOG_CONS, LOG_SYSLOG);
        textcolor(BRIGHT, YELLOW, BLACK);	
        vsyslog(LOG_WARNING, format, arg);
        closelog();
    }
    va_end (arg);
    printf("\n");
	textcolor(RESET, WHITE, BLACK);	
    return 0;
}

int errorf(const char *format, ...){
    printf("\n");
    textcolor(BRIGHT, RED, BLACK);	
    printf("%s",print_time());
	va_list arg;
    va_start (arg, format);
    if(type == 1){
        vprintf (format, arg);
    }else{
        openlog("logger", LOG_PID | LOG_CONS, LOG_SYSLOG);
        textcolor(BRIGHT, RED, BLACK);	
        vsyslog(LOG_ERR, format, arg);
        closelog();
    }
    va_end (arg);
    printf("\n");
	textcolor(RESET, WHITE, BLACK);	
    return 0;
}

int panicf(const char *format, ...){
    printf("\n");
    textcolor(BRIGHT, MAGENTA, BLACK);
    printf("%s",print_time());	
	va_list arg;
    va_start (arg, format);
    if(type == 1){
        vprintf (format, arg);
    }else {
        openlog("logger", LOG_PID | LOG_CONS, LOG_SYSLOG);
        textcolor(BRIGHT, MAGENTA, BLACK);	
        syslog(LOG_EMERG,format, arg);
        closelog ();
    }
    va_end (arg);
    printf("\n");
	textcolor(RESET, WHITE, BLACK);	
    return 0;
}

void textcolor(int attr, int fg, int bg)
{	char command[13];
	sprintf(command, "%c[%d;%d;%dm", 0x1B, attr, fg + 30, bg + 40);
	printf("%s", command);
}

char* print_time(){
    struct tm* local; 
    time_t t = time(NULL);  
    local = localtime(&t); 
    return  asctime(local); 
}

