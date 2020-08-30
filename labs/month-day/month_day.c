#include <stdio.h>   
#include <stdlib.h>
#define NUMBER_OF_STRING 13
#define MAX_STRING_SIZE 20
int dias[13]={0,31,28,31,30,31,30,31,31,30,31,30,31};
int diasbi[13]={0,31,29,31,30,31,30,31,31,30,31,30,31};
int meses[13]={0,1,2,3,4,5,6,7,8,9,10,11,12};
char mesesL[NUMBER_OF_STRING][MAX_STRING_SIZE]={"Ilegal Day","Jan","Feb","Mar","April","May","June","July","Agust","Sep","Oct","Nov","Dec"};

void month_day(int year, int yearday, int *pmonth, int *pday){
    int d=*pday;
    int contador=0;
    if(year==0||yearday>366||year<=999){
        printf("ILegal Date");
        printf("\n");
    }else if(((year%4 == 0) && (year%100 != 0)) || (year%400 == 0)){
        while(yearday>d){
            yearday=yearday-d;
            pday=&diasbi[contador];
            pmonth=&meses[contador];
            d=*pday;
            contador++;
        }
        printf("%s%s%i%s%i%s\n",mesesL[*pmonth]," ",yearday,",",year," "); 
    }else{
        while(yearday>d){
            yearday=yearday-d;
            pday=&dias[contador];
            pmonth=&meses[contador];
            d=*pday;
            contador++;
        }if(*pday==0||*pmonth==0){
            printf("ILegal Date");
            printf(" ");
        }else{
        printf("%s%s%i%s%i%s\n",mesesL[*pmonth]," ",yearday,",",year," "); 
        }  
    }
};

int main(int argc,char **argv) {
    int *pday;
    int *pmonth;
    pmonth=&meses[0];
    pday=&dias[0];
    if(argc<3){
        printf("Please enter correct form --> ");
        printf("./month_day <year> <yearday>");
        return -1;
    }
    month_day(atoi(argv[1]),atoi(argv[2]),pmonth,pday);
    return 0;
}