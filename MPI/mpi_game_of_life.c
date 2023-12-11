#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <mpi.h>

#define max_quantity 2048

int survivor = 0;

float **grid;
float **newgrid;
float **aux;

typedef struct
{
    int x;
    int y;
} neighbor;

typedef struct
{
    int p;
    int c;
} brics;

void where_is_my_neighbors(neighbor *neighborhood, int x, int y)
{

    int xless = x - 1;
    int yless = y - 1;
    int xmore = x + 1;
    int ymore = y + 1;

    if (xless < 0)
    {
        xless = max_quantity - 1;
    }
    if (yless < 0)
    {
        yless = max_quantity - 1;
    }
    if (xmore == max_quantity)
    {
        xmore = 0;
    }
    if (ymore == max_quantity)
    {
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

float count_population(int x, int y)
{

    float how_many = 0.0;

    neighbor *neighborhood = calloc(8, sizeof(neighbor));
    where_is_my_neighbors(neighborhood, x, y);

    for (int count = 0; count < 8; count++)
    {
        how_many = how_many + grid[neighborhood[count].x][neighborhood[count].y];
    }

    free(neighborhood);

    return how_many;
}

void natural_selection(int x, int y)
{
    float how_many = count_population(x, y);

    if ((how_many == 3) || (grid[x][y] == 1 && how_many == 2))
    {
        newgrid[x][y] = 1;
    }
    else
    {
        newgrid[x][y] = 0;
    }
}

void through_the_ages(int rank, int num_proc)
{

    struct timeval start, end;
    long seconds;

    gettimeofday(&start, NULL);

    int left_limit, right_limit, next, previous;
    int block_size = max_quantity / num_proc;

    float my_right_limit[max_quantity], my_left_limit[max_quantity];
    float your_right_limit[max_quantity], your_left_limit[max_quantity];

    if (rank == 0)
    {
        right_limit = block_size;
        left_limit = 0;
        next = rank + 1;
        previous = num_proc - 1;
    }
    else
    {
        if (rank == num_proc - 1)
        {
            right_limit = max_quantity;
            left_limit = block_size * rank;
            next = 0;
            previous = rank - 1;
        }
        else
        {
            right_limit = block_size * (rank + 1);
            left_limit = block_size * rank;
            next = rank + 1;
            previous = rank - 1;
        }
    }

    for (int count_generation = 0; count_generation < 2000; count_generation++)
    {
        for (int count_x = left_limit; count_x < right_limit; count_x++)
        {
            for (int count_y = 0; count_y < max_quantity; count_y++)
            {
                natural_selection(count_x, count_y);
            }
        }

        aux = grid;
        grid = newgrid;
        newgrid = aux;

        // TROCA de LIMITES da base de dados
        for (int count_line = 0; count_line < max_quantity; count_line++)
        {
            my_left_limit[count_line] = grid[left_limit][count_line];
            my_right_limit[count_line] = grid[right_limit - 1][count_line];
        }

        if (rank % 2 == 0)
        {
            MPI_Send(my_left_limit, max_quantity, MPI_FLOAT, next, 0, MPI_COMM_WORLD);
            MPI_Recv(your_right_limit, max_quantity, MPI_FLOAT, previous, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        }
        else
        {
            MPI_Recv(your_right_limit, max_quantity, MPI_FLOAT, previous, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            MPI_Send(my_left_limit, max_quantity, MPI_FLOAT, next, 0, MPI_COMM_WORLD);
        }

        if (rank % 2 == 0)
        {
            MPI_Send(my_right_limit, max_quantity, MPI_FLOAT, next, 0, MPI_COMM_WORLD);
            MPI_Recv(your_left_limit, max_quantity, MPI_FLOAT, previous, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        }
        else
        {
            MPI_Recv(your_left_limit, max_quantity, MPI_FLOAT, previous, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            MPI_Send(my_right_limit, max_quantity, MPI_FLOAT, next, 0, MPI_COMM_WORLD);
        }

        for (int count_line = 0; count_line < max_quantity; count_line++)
        {
            if (rank == 0)
            {
                grid[max_quantity - 1][count_line] = your_right_limit[count_line];
                grid[right_limit][count_line] = your_left_limit[count_line];
            }
            else
            {
                if (rank == num_proc - 1)
                {
                    grid[left_limit - 1][count_line] = your_right_limit[count_line];
                    grid[0][count_line] = your_left_limit[count_line];
                }
                else
                {
                    grid[left_limit - 1][count_line] = your_right_limit[count_line];
                    grid[right_limit][count_line] = your_left_limit[count_line];
                }
            }
        }
        MPI_Barrier(MPI_COMM_WORLD);
    }

    

    survivor = 0;
    for (int count_x = left_limit; count_x < right_limit; count_x++)
    {
        for (int count_y = 0; count_y < max_quantity; count_y++)
        {
            if (grid[count_x][count_y] == 1)
            {
                survivor = survivor + 1;
            }
        }
    }
    int final_survivor;

    MPI_Reduce(&survivor, &final_survivor, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);

    if (rank == 0)
    {
        printf("Number of survivor: %d", final_survivor);

        gettimeofday(&end, NULL);
        seconds = end.tv_sec - start.tv_sec;

        printf("\n\n\n");
        printf("O TEMPO DECORRIDO FOI DE %ld", seconds);
    }

    
}

int main(int argc, char *argv[])
{

    MPI_Init(&argc, &argv);

    int rank, num_proc;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &num_proc);

    grid = calloc(max_quantity, sizeof(float *));
    newgrid = calloc(max_quantity, sizeof(float *));
    aux = calloc(max_quantity, sizeof(float *));

    for (int count = 0; count < max_quantity; count++)
    {
        grid[count] = calloc(max_quantity, sizeof(float));
        newgrid[count] = calloc(max_quantity, sizeof(float));
        aux[count] = calloc(max_quantity, sizeof(float));
    }

    // GLIDER
    int lin = 1, col = 1;
    grid[lin][col + 1] = 1.0;
    grid[lin + 1][col + 2] = 1.0;
    grid[lin + 2][col] = 1.0;
    grid[lin + 2][col + 1] = 1.0;
    grid[lin + 2][col + 2] = 1.0;
    // R-pentomino
    lin = 10;
    col = 30;
    grid[lin][col + 1] = 1.0;
    grid[lin][col + 2] = 1.0;
    grid[lin + 1][col] = 1.0;
    grid[lin + 1][col + 1] = 1.0;
    grid[lin + 2][col + 1] = 1.0;

    through_the_ages(rank, num_proc);

    MPI_Finalize();
    return 0;
}