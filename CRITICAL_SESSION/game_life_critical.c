#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include <sys/time.h>

#define max_quantity 2048
#define num_threads 8

int survivor = 10;

typedef struct{
    int x;
    int y;
}neighbor;

typedef struct {
    int is_alive;
    double how_many;
}bric_alive;

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

bric_alive count_population(double **cell, int x, int y){
    
    bric_alive bric;
    bric.how_many = 0.0;
    bric.is_alive = 0;

    neighbor *neighborhood = calloc(8, sizeof(neighbor));
    where_is_my_neighbors(neighborhood,  x,  y);

    for(int count = 0; count < 8; count++){
        if(cell[neighborhood[count].x][neighborhood[count].y] != 0){
            bric.is_alive = bric.is_alive + 1;
        }
        bric.how_many = bric.how_many + cell[neighborhood[count].x][neighborhood[count].y];
    }
    bric.how_many = bric.how_many / 8;

    free(neighborhood);

    return bric;
}

void natural_selection(double **grid, double **newgrid, int x, int y){
    bric_alive bric = count_population(grid, x, y);

    double how_many = bric.how_many;
    int isalive = bric.is_alive;

    if((isalive == 3) || (grid[x][y] > 0 && isalive == 2)){
        if(grid[x][y] == 0.0){
            #pragma omp critical (revive_survivor)
            {
                survivor = survivor + 1;
            }
            newgrid[x][y] = how_many;
        }
        else{
            newgrid[x][y] = grid[x][y] ;
        }
    }
    else{
        newgrid[x][y] = 0.0;
        if(grid[x][y] > 0.0){
            #pragma omp critical (kill_survivor)
            {
                survivor = survivor - 1;
            }
        }
    }

}

int main(){

    double **grid = calloc(max_quantity, sizeof(double*));
    double **newgrid = calloc(max_quantity, sizeof(double*));
    double **aux = calloc(max_quantity, sizeof(double*));

    struct timeval start, end;    
    long seconds;

    for(int count = 0; count < max_quantity; count++){
        grid[count] = calloc(max_quantity, sizeof(double));
        newgrid[count] = calloc(max_quantity, sizeof(double));
        aux[count] = calloc(max_quantity, sizeof(double));
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

    omp_set_num_threads(num_threads);

    gettimeofday(&start, NULL);


    for(int count_total = 0; count_total < 2000; count_total++){
        
        #pragma omp parallel for
        
        for(int count_x = 0; count_x < max_quantity; count_x++){
            for(int count_y = 0; count_y < max_quantity; count_y++){
                natural_selection(grid, newgrid, count_x, count_y);
            }
        }

            
        aux = grid;
        grid = newgrid;
        newgrid = aux;
    }

    gettimeofday(&end, NULL);
    seconds  = end.tv_sec  - start.tv_sec;

    printf("\n\n\n");
    printf("O TEMPO DECORRIDO FOI DE %ld", seconds);

    return 1;
}