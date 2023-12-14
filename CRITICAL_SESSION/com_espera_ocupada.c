#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <omp.h>
#include <sys/time.h>
#include <time.h>

#define num_threads 3

int main()
{
    srand(time(0));

    int x = 0;
    int request = 1;
    int i = 1;
    int respond = 1;

    omp_set_num_threads(3);

#pragma omp parallel for shared(x, i, request, respond)
    for (int count = 0; count < 100; count++)
    {
        printf("%d", omp_get_thread_num());
        if (omp_get_thread_num() == 0)
        {
            while (request == 0)
            {   
                printf("OK");
            }
            respond = request;
            while (respond != 0)
            {
            }
            request = 0;
        }
        else
        {
            
            while (respond != omp_get_thread_num())
            {
                request = omp_get_thread_num();
            }
            
            int num = omp_get_thread_num();
            int local = x;
            sleep(rand() % 2);
            x = local + 1;
            printf("Thread %d - Valor: %d\n", num, x);
            
            respond = 0;
        }
        
    }
    return 1;
}
