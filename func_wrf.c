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

/* 
   WIP: The logic here is funky; this is due to the storage model of netcdf variables. 
   Though coordinates technically should live on a grid, they do not and instead must be accessed as i, j(?)
*/

void pullGrid(size_t nx, size_t ny, float grid[nx][ny], float lons[nx*ny], float lats[nx*ny])
{
    int indx = 0;
    for (size_t j = 0; j < ny; ++j)
    {
        int cx = 0;
        for (size_t i = 0; i < nx; ++i)
        {
            grid[i][j] = lons[indx];
            ++indx;
            
            //printf("%ld, %ld: %f ", i, j, grid[i][j]);
        }
        grid[cx][j] = lats[indx];
        printf("%f \n", grid[cx][j]);
        ++cx;
    }
}