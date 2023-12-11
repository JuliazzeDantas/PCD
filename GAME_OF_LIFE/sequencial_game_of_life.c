#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

#define max_quantity 2048

int survivor = 10;

typedef struct{
    int x;
    int y;
}neighbor;

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

float count_population(float **cell, int x, int y){
    
    float how_many = 0.0;

    neighbor *neighborhood = calloc(8, sizeof(neighbor));
    where_is_my_neighbors(neighborhood,  x,  y);

    for(int count = 0; count < 8; count++){
        how_many = how_many + cell[neighborhood[count].x][neighborhood[count].y];
    }

    free(neighborhood);

    return how_many;
}

void natural_selection(float **grid, float **newgrid, int x, int y){
    float how_many = count_population(grid, x, y);

    if((how_many == 3) || (grid[x][y] == 1 && how_many == 2)){
        newgrid[x][y] = 1;
        if(grid[x][y] == 0){
            survivor = survivor + 1;
        }
    }
    else{
        newgrid[x][y] = 0;
        if(grid[x][y] == 1){
            survivor = survivor - 1;
        }
    }

}

int main(){

    float **grid = calloc(max_quantity, sizeof(float*));
    float **newgrid = calloc(max_quantity, sizeof(float*));
    float **aux = calloc(max_quantity, sizeof(float*));

    struct timeval start, end;
    long seconds;

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

    gettimeofday(&start, NULL);

    for(int count_total = 0; count_total < 2000; count_total++){

        /*
        for(int count_col = 0; count_col < max_quantity; count_col++){
            for(int count_line = 0; count_line < max_quantity; count_line++){
                printf("%.0f  ", grid[count_line][count_col]);
            }
            printf("\n");
        }*/ 

        printf("\n");

        for(int count_x = 0; count_x < max_quantity; count_x++){
            for(int count_y = 0; count_y < max_quantity; count_y++){
                natural_selection(grid, newgrid, count_x, count_y);
            }
        }

        printf("O número de sobreviventes da geração %d foi de %d \n", count_total, survivor);
        
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