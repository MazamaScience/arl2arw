#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <netcdf.h>

#define LABSIZE 50

int numX(char str[]);
int numY(char str[]);
char *varDesc(char str[]);
long numRecs(long size, long recl);
double numExp(char str[]);
double numVar1(char str[]);
double tanLat(char str[]);
double refLon(char str[]);
void unpack(double nexp, double var1, size_t nx, size_t ny,
            char cdata[], double rdata[nx][ny]);
void check(int status);
void pullGrid(size_t nx, size_t ny, float grid[nx][ny], float lons[nx*ny], float lats[nx*ny]);

int main(int argc, char *argv[])
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
            unpack(nexp, var1, (size_t)nx, (size_t)ny, cdata, *rdata);
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
    float(*grid)[lon_diml][lat_diml];
    grid = malloc(sizeof(float[(size_t)lon_diml][(size_t)lat_diml]));

    // Pull the grid (?)
    pullGrid(lon_diml, lat_diml, *grid, lons, lats);

    return 0;
}

/* ARL FUNCTIONS */
// Unpacking
void unpack(double nexp, double var1, size_t nx, size_t ny,
            char cdata[], double rdata[nx][ny])
{
    int indx = 0;
    double vold = var1;
    double scale = pow(2.0, (7.0 - nexp));

    for (size_t j = 0; j < ny; ++j)
    {
        for (size_t i = 0; i < nx; ++i)
        {
            rdata[i][j] = (cdata[indx] - 127.0) / scale + vold;
            vold = rdata[i][j];
            ++indx;
            if (i < 4 && j < 15)
                printf("(%ld,%ld): %lf ", i, j, rdata[i][j]);
        }
        vold = rdata[0][j];
        if (j < 15)
            printf(" ...\n");
    }
    printf(" ...\n");
}

// Be simple.
// Pull string from string
char *pullStr(char str[], int pos, int len)
{
    char *sptr;
    sptr = malloc(len + 1);
    strncpy(sptr, str + pos, len);
    sptr[len] = '\0';
    return sptr;
}

int numX(char str[])
{
    int ix;
    char *cx;
    cx = pullStr(str, 93, 3);
    ix = atoi(cx);
    return ix;
}

int numY(char str[])
{
    int iy;
    char *cy;
    cy = pullStr(str, 96, 3);
    iy = atoi(cy);
    return iy;
}

char *varDesc(char str[])
{
    char *desc;
    desc = pullStr(str, 14, 4);
    return desc;
}

long numRecs(long size, long recl)
{
    return size / recl;
}

double numExp(char str[])
{
    int exp;
    char *nexp;
    nexp = pullStr(str, 18, 4);
    exp = atof(nexp);
    return exp;
}

double numVar1(char str[])
{
    char *var1;
    double var;
    var1 = pullStr(str, 36, 17);
    var = atof(var1);
    return var;
}

double tanLat(char str[])
{
    char *ctlat;
    double tlat;
    ctlat = pullStr(str, 50, 8);
    tlat = atof(ctlat);
    return tlat;
}

double refLon(char str[])
{
    char *crlon;
    double rlon;
    crlon = pullStr(str, 30, 8);
    rlon = atof(crlon);
    return rlon;
}

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