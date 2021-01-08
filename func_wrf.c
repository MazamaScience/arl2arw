#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <netcdf.h>

#include <arl2arw.h>

/* NETCDF FUNCTIONS */
void check(int status)
{
    if (status != NC_NOERR)
    {
        printf("Error: %s\n", nc_strerror(status));
    }
}

// This function pulls the 1-d Variable of NC and converts into a 2-d grid. Currently just prints. 
void pullGrid(size_t nx, size_t ny, float grid[nx][ny], float var[nx*ny])
{
    int indx = 0;
    for (size_t j = 0; j < ny; ++j)
    {
        for (size_t i = 0; i < nx; ++i)
        {
            grid[i][j] = var[indx];
            ++indx;
            
            printf("%ld, %ld: %f\n", i, j, grid[i][j]);
        }
    }
}