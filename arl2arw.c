#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <netcdf.h>
#include <arl2arw.h>

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
    char *cdata; // packed 1-d char array

    // Allocate array space
    cdata = malloc(sizeof(char) * ((size_t)nxy));
    // rdata = malloc(sizeof(double[(size_t)nx][(size_t)ny]));

    int nrecs = numRecs(fsize, recl);

    Grid *data = malloc(sizeof(Grid) * nrecs * 2); // LOOK!

    // Unpack each record described by its 50-byte record label
    rewind(arl);
    int rec = 0;
    while (rec < nrecs)
    {
        ++rec;
        fread(label, sizeof(char), LABSIZE, arl);
        fread(cdata, sizeof(char), (size_t)nxy, arl);
        if (strcmp(varDesc(label), hindex) != 0)
        {
            nexp = numExp(label);
            var1 = numVar1(label);
            data[rec] = unpack2(nexp, var1, (size_t)nx, (size_t)ny, cdata);
            strncpy(data[rec].label, label, 50);
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

    Grid glons = makeGrid(nx, ny, lons);
    Grid glats = makeGrid(nx, ny, lats);

    // Grid data = makeGrid(nx, ny, lons);
    // printf("%f, %f: %f", glons.val[0][0], glats.val[0][0], data.val[3][7]); // test
    for (int r = 0; r < nrecs; ++r)
    {
        printf("\t\t%s\t\t\n", data[r].label);
        for (int j = 0; j < ny; ++j)
        {
            for (int i = 0; i < nx; ++i)
            {
                printf("(%d,%d) %f, %f: %f  ", i, j, glons.val[i][j], glats.val[i][j], data[r].val[i][j]);
            }
        }
    }
    int idout;
    int t_dimid;
    int lon1_dimid, lat1_dimid;
    int lon1_varid, lat1_varid;

    // Create the NC file
    check(nc_create("output.nc", NC_CLOBBER, &idout));

    // Define the dimensions nx ny
    check(nc_def_dim(idout, "TIME", NC_UNLIMITED, &t_dimid));
    check(nc_def_dim(idout, "XLONG", nx, &lon1_dimid));

    // Define the variables
    check(nc_def_var(idout, "XLONG", NC_FLOAT, 1, &lon1_dimid, &lon1_varid));

    // End define mode
    check(nc_enddef(idout));

    // Put the variables
    check(nc_put_var_float(idout, lon1_varid, glons.val[0]));

    // Close the app
    check(nc_close(idout));

    return 0;
}
