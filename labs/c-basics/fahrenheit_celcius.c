#include <stdio.h>    
#include <stdlib.h>

/* print Fahrenheit-Celsius table */
/* Nicole Carrillo Capristan  */
int main(int argc, char **argv)
{
    int fahr;

   if(argc==2){
       fahr=atoi(argv[1]);
       printf("Fahrenheit: %3d, Celcius: %6.1f\n", fahr, (5.0/9.0)*(fahr-32));
   }else{
       int lower=atoi(argv[1]);
    int upper=atoi(argv[2]);
    int step=atoi(argv[3]);
       for (fahr = lower; fahr <= upper; fahr = fahr + step)
	   printf("Fahrenheit: %3d, Celcius: %6.1f\n", fahr, (5.0/9.0)*(fahr-32));
   }
    return 0;
}