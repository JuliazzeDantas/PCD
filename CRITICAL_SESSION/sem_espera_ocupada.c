#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <omp.h>
#include <sys/time.h>
#include <time.h>

int main(){

    srand(time(0));  

    int x = 0;

    omp_set_num_threads(2);

    #pragma omp parallel for shared(x)
        for(int count = 0; count < 100; count++){
            int num = omp_get_thread_num();
            int local = x;
            sleep(rand() % 2);
            x = local + 1;
            printf("Thread %d - Valor: %d\n", num, x);
        }
        
    return 1;
}
