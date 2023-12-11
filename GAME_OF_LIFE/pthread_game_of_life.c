#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/time.h>

#define max_quantity 2048
#define num_threads 1

int piece_survivor[num_threads];
int survivor = 10;

float **grid;
float **newgrid;
float **aux;

typedef struct{
    int x;
    int y;
}neighbor;

typedef struct{
    int p;
    int c;
}brics;

void where_is_my_neighbors(neighbor *neighborhood, int x, int y){

    int xless = x - 1;
    int yless = y - 1;
    int xmore = x + 1;
    int ymore = y + 1;

    if(xless < 0){
        xless = max_quantity - 1;
    }
    if(yless < 0){
        yless = max_quantity - 1;
    }
    if(xmore == max_quantity){
        xmore = 0;
    }
    if(ymore == max_quantity){
        ymore = 0;
    }
 
    neighborhood[0].x = xless;
    neighborhood[0].y = ymore;

    neighborhood[1].x = x;
    neighborhood[1].y = ymore;

    neighborhood[2].x = xmore;
    neighborhood[2].y = ymore;

    neighborhood[3].x = xless;
    neighborhood[3].y = y;

    neighborhood[4].x = xmore;
    neighborhood[4].y = y;

    neighborhood[5].x = xless;
    neighborhood[5].y = yless;

    neighborhood[6].x = x;
    neighborhood[6].y = yless;

    neighborhood[7].x = xmore;
    neighborhood[7].y = yless;
}

float count_population(int x, int y){
    
    float how_many = 0.0;

    neighbor *neighborhood = calloc(8, sizeof(neighbor));
    where_is_my_neighbors(neighborhood,  x,  y);

    for(int count = 0; count < 8; count++){
        how_many = how_many + grid[neighborhood[count].x][neighborhood[count].y];
    }

    free(neighborhood);

    return how_many;
}

void natural_selection(int x, int y, int piece){
    float how_many = count_population(x, y);

    if((how_many == 3) || (grid[x][y] == 1 && how_many == 2)){
        newgrid[x][y] = 1;
        piece_survivor[piece] = piece_survivor[piece] + 1;
    }
    else{
        newgrid[x][y] = 0;
    }

}

void *run_process(void* b){
    
    brics *bric = (brics*)b;

    int piece = (int)bric->p;
    int count_total = (int)bric->c;
    
    float range = (max_quantity) / (num_threads);

    //printf("------------------ GEN %d da thread %d iniciou a execução \n", count_total, piece);
    

    for(int count_x = (piece * range); count_x < ((piece + 1) * range); count_x++){
        for(int count_y = 0; count_y < max_quantity; count_y++){
            natural_selection(count_x, count_y, piece);
        }
    }

    //printf("------------------ GEN %d da thread %d finalizou a execução \n", count_total, piece);

    pthread_exit(NULL);
}

void through_the_ages(){

    struct timeval start, end;
    long seconds;
    brics bric[num_threads];

    gettimeofday(&start, NULL);

    for(int count_total = 0; count_total < 2000; count_total++){
        survivor = 0;
        pthread_t t[num_threads];

        for(int count_threads = 0; count_threads < num_threads; count_threads ++){
            bric[count_threads].p = count_threads;
            bric[count_threads].c  = count_total;
            pthread_create(&t[count_threads], NULL, run_process, &bric[count_threads]);
        }

        for (int count_threads = 0; count_threads < num_threads; count_threads++) {
            pthread_join(t[count_threads], NULL);
        }

        printf("\n");

        for (int count_threads = 0; count_threads < num_threads; count_threads++) {
            survivor = survivor + piece_survivor[count_threads];
            piece_survivor[count_threads] = 0;
        }
        printf("Generation %d - Number of survivor: %d\n", count_total + 1, survivor);

        /*for(int count_col = 0; count_col < max_quantity; count_col++){
            for(int count_line = 0; count_line < max_quantity; count_line++){
                printf("%.0f  ", grid[count_line][count_col]);
            }
            printf("\n");
        }*/

        aux = grid;
        grid = newgrid;
        newgrid = aux;
    }

    gettimeofday(&end, NULL);
    seconds  = end.tv_sec  - start.tv_sec;

    printf("\n\n\n");
    printf("O TEMPO DECORRIDO FOI DE %ld", seconds);

}


int main(){

    grid = calloc(max_quantity, sizeof(float*));
    newgrid = calloc(max_quantity, sizeof(float*));
    aux = calloc(max_quantity, sizeof(float*));

    for(int count = 0; count < max_quantity; count++){
        grid[count] = calloc(max_quantity, sizeof(float));
        newgrid[count] = calloc(max_quantity, sizeof(float));
        aux[count] = calloc(max_quantity, sizeof(float));
    }

    //GLIDER
    int lin = 1, col = 1;
    grid[lin  ][col+1] = 1.0;
    grid[lin+1][col+2] = 1.0;
    grid[lin+2][col  ] = 1.0;
    grid[lin+2][col+1] = 1.0;
    grid[lin+2][col+2] = 1.0;

    //R-pentomino
    lin =10; col = 30;
    grid[lin  ][col+1] = 1.0;
    grid[lin  ][col+2] = 1.0;
    grid[lin+1][col  ] = 1.0;
    grid[lin+1][col+1] = 1.0;
    grid[lin+2][col+1] = 1.0;

    through_the_ages();

    return 1;
}