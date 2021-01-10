#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <netcdf.h>
#include <arl2arw.h>

#define LABSIZE 50

// typedef struct {
//     double **longitude; 
//     double **latitude; 
//     double *data;
//     char *label;

// } Grid;

int main(int argc, char **argv)
{

    /* ARL */
    FILE *arl;  // ARL file stream
    long fsize; // file byte size

    char *label;  // standard label 50-bytes
    char *header; // header 108-bytes
    char *hindex; // header index desc

    int nx, ny;     // x, y dims
    long nxy, recl; // num xy, record length
    double nexp;    // scaling integer
    double var1;    // variable at (1,1)

    // Open ARL to file stream
    arl = fopen(argv[1], "rb");

    // Get the file size
    fseek(arl, 0, SEEK_END);
    fsize = ftell(arl);
    rewind(arl);

    // Allocate label and ARL header array space
    label = malloc(sizeof(char) * LABSIZE);
    header = malloc(sizeof(char) * 3072); // Overflow

    // Read the standard portion of the label(50) and header(108)
    fread(label, sizeof(char), LABSIZE, arl);
    fread(header, sizeof(char), 108, arl);

    // Header INDX label
    hindex = varDesc(label);

    // Get grid dimensions & record length
    nx = numX(header);
    ny = numY(header);
    nxy = nx * ny;
    recl = nxy + LABSIZE;

    // Define data arrays
    char *cdata;                            // packed 1-d char array
    double(*rdata)[(size_t)nx][(size_t)ny]; // unpacked 2-d real array

    // Allocate array space
    cdata = malloc(sizeof(char) * ((size_t)nxy));
    rdata = malloc(sizeof(double[(size_t)nx][(size_t)ny]));

    Grid data; // LOOK! 
    // Unpack each record described by its 50-byte record label
    rewind(arl);
    int rec = 0;
    while (rec < numRecs(fsize, recl))
    {
        ++rec;
        fread(label, sizeof(char), LABSIZE, arl);
        fread(cdata, sizeof(char), (size_t)nxy, arl);
        if (strcmp(varDesc(label), hindex) != 0)
        {
            nexp = numExp(label);
            var1 = numVar1(label);
            printf("\n%s\n", label);
            // unpack(nexp, var1, (size_t)nx, (size_t)ny, cdata, *rdata);
            data = unpack2(nexp, var1, (size_t)nx, (size_t)ny, cdata);

        }
    }

    // Close ARL file stream
    fclose(arl);

    /* NETCDF */
    int ncid;
    int lat_dimid, lat_varid;
    int lon_dimid, lon_varid;

    size_t lat_diml, lon_diml;

    float *lats, *lons;

    // Open the netcdf
    check(nc_open(argv[2], NC_NOWRITE, &ncid));

    // Check the dimensions
    check(nc_inq_dimid(ncid, "south_north", &lat_dimid));
    check(nc_inq_dimlen(ncid, lat_dimid, &lat_diml));

    check(nc_inq_dimid(ncid, "west_east", &lon_dimid));
    check(nc_inq_dimlen(ncid, lon_dimid, &lon_diml));

    if ((int)lat_diml != ny || (int)lon_diml != nx)
        return 2;

    // Allocate coordinate space
    lats = malloc(sizeof(float) * lat_diml * lon_diml);
    lons = malloc(sizeof(float) * lat_diml * lon_diml);

    // Pull the coordinate grid (!)
    check(nc_inq_varid(ncid, "XLAT", &lat_varid));
    check(nc_get_var_float(ncid, lat_varid, &lats[0]));

    check(nc_inq_varid(ncid, "XLONG", &lon_varid));
    check(nc_get_var_float(ncid, lon_varid, &lons[0]));

    // Each XLAT and XLONG are accessed at their respective points i,j
    // Create 2-d array to store coordinate vars for lats and lons
    // float (*lon_grid)[lon_diml][lat_diml];
    // lon_grid = malloc(sizeof(float[(size_t)lon_diml][(size_t)lat_diml]));

    // float (*lat_grid)[lon_diml][lat_diml];
    // lat_grid = malloc(sizeof(float[(size_t)lon_diml][(size_t)lat_diml]));

    //Grid grid;
    // grid.nx = nx; 
    // grid.ny = ny; 
    // Grid *gridptr = &grid; 

    // pullGrid3(nx, ny, gridptr, lons);

    // printf("%lf", grid.longitude[0]);

    // double **poo = pullGrid2(lon_diml, lat_diml, lons);
    //float (*lg)[nx][ny];

    // Pull the grids of both lat and lon nc vars
    // pullGrid(lon_diml, lat_diml, lon_grid, lons);
    // pullGrid(lon_diml, lat_diml, lat_grid, lats);

    // printf("%f", *lat_grid[2][1]);

    Grid glons = makeGrid(nx, ny, lons);
    Grid glats = makeGrid(nx, ny, lats);
    // Grid data = makeGrid(nx, ny, lons);
    printf("%f, %f: %f", glons.val[400][280], glats.val[400][280], data.val[400][280]);


    return 0;
}
