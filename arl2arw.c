#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <netcdf.h>
#include <arl2arw.h>

// Allocate data memory on heap
static double data[1][100][500][500]; // (time(1), level, lat, long)

int main(int argc, char **argv)
{

    /* ARL */
    // Open ARL to file stream
    FILE *arl = fopen(argv[1], "rb");

    // Get the file size
    fseek(arl, 0, SEEK_END);
    long fsize = ftell(arl);
    rewind(arl);

    // Allocate label and ARL header array space
    char *label = malloc(sizeof(char) * LABSIZE);
    char *header = malloc(sizeof(char) * 3072); // Overflow

    // Read the standard portion of the label(50) and header(108)
    fread(label, sizeof(char), LABSIZE, arl);
    fread(header, sizeof(char), 108, arl);

    // Header INDX label
    char *hindx = varDesc(label);

    // Get grid dimensions, record length, and number of levels
    int nx = numX(header);
    int ny = numY(header);
    long nxy = nx * ny;
    long recl = nxy + LABSIZE;
    int nLvls = numLvls(header);

    // Allocate packed char array space
    char *cdata = malloc(sizeof(char) * ((size_t)nxy));

    int nrecs = numRecs(fsize, recl);

    // Count records per level (including 0th)
    rewind(arl);
    int current_rec = 0;
    int current_lvl = 0;
    // Store records per level
    int *rec_count = calloc(nLvls, sizeof(int));
    while (current_rec < nrecs)
    {
        ++current_rec;
        fread(label, sizeof(char), LABSIZE, arl);
        fread(cdata, sizeof(char), (size_t)nxy, arl);
        rec_count[current_lvl] += 1;
        if (curLvl(label) != current_lvl)
        {
            ++current_lvl;
        }
    }

    // NC vars ids
    int ncin_id;
    int times_var_id;
    int xlat_var_id;
    int xlong_var_id;

    // times array access (time, datestrlen)
    size_t time_start[] = {0, 0};
    size_t time_count[] = {1, 18};
    // grid array access (time, xlat, xlong)
    size_t grid_start[] = {0, 0, 0};
    size_t grid_count[] = {1, ny, nx}; // 1 time dim! y x order!

    // Allocate nc in var space
    char *times = malloc(sizeof(char *) * 1 * 19);
    float *xlats = malloc(sizeof(float *) * 1 * ny * nx);
    float *xlongs = malloc(sizeof(float *) * 1 * ny * nx);

    // Open in NC
    check(nc_open(argv[2], NC_NOWRITE, &ncin_id));

    check(nc_inq_varid(ncin_id, TIME_VAR, &times_var_id));
    check(nc_get_vara(ncin_id, times_var_id, time_start, time_count, times));

    check(nc_inq_varid(ncin_id, XLAT, &xlat_var_id));
    check(nc_get_vara_float(ncin_id, xlat_var_id, grid_start, grid_count, xlats));

    check(nc_inq_varid(ncin_id, XLONG, &xlong_var_id));
    check(nc_get_vara_float(ncin_id, xlat_var_id, grid_start, grid_count, xlongs));

    // Close in NC //
    check(nc_close(ncin_id));

    // NC dim ids
    int times_dim_id;
    int dsl_dim_id;
    int xlat_dim_id;
    int xlong_dim_id;
    int lvls_dim_id;

    int xlat_stag_dim_id;
    int xlong_stag_dim_id;
    int lvls_stag_dim_id;

    // Create NC file
    int ncout_id;
    check(nc_create(NC_OUT, NC_CLOBBER, &ncout_id));

    // Define dimensions
    check(nc_def_dim(ncout_id, TIME_NAME, NC_UNLIMITED, &times_dim_id));
    check(nc_def_dim(ncout_id, DATESTRLEN_NAME, 19, &dsl_dim_id)); // Look into
    check(nc_def_dim(ncout_id, WEST_EAST, ny, &xlat_dim_id));
    check(nc_def_dim(ncout_id, SOUTH_NORTH, nx, &xlong_dim_id));
    check(nc_def_dim(ncout_id, BOTTOM_TOP, nLvls - 1, &lvls_dim_id));
    check(nc_def_dim(ncout_id, BOTTOM_TOP_STAG, nLvls, &lvls_stag_dim_id));
    check(nc_def_dim(ncout_id, WEST_EAST_STAG, nx + 1, &xlong_stag_dim_id));
    check(nc_def_dim(ncout_id, SOUTH_NORTH_STAG, ny + 1, &xlat_stag_dim_id));

    // Time variables
    int times_dimids[] = {times_dim_id, dsl_dim_id};
    check(nc_def_var(ncout_id, TIME_VAR, NC_CHAR, 2, times_dimids, &times_var_id));

    // Latitude longitude variables
    int grid_dimids[] = {times_dim_id, xlat_dim_id, xlong_dim_id};
    check(nc_def_var(ncout_id, XLAT, NC_FLOAT, 3, grid_dimids, &xlat_var_id));
    check(nc_def_var(ncout_id, XLONG, NC_FLOAT, 3, grid_dimids, &xlong_var_id));

    // Unpacked varibales
    // int std_dim_ids[] = {times_dim_id, lvls_dim_id, xlat_dim_id, xlong_dim_id};
    // int std_var_ids[nLvls];

    // write data
    rewind(arl);
    for (int z = 0; z < nLvls; ++z) // For every level z
    {
        char *desc;
        for (int rec = 0; rec < rec_count[z]; ++rec) // For every record in level z
        {
            fread(label, sizeof(char), LABSIZE, arl);
            fread(cdata, sizeof(char), (size_t)nxy, arl);
            desc = varDesc(label);
            if (strcmp(desc, hindx) != 0)
            {
                double nexp = numExp(label);
                double var1 = numVar1(label);
                unpack(nexp, var1, ny, nx, cdata, data[0][z]); // unpack to data
                printf("\t%f\n", data[0][z][0][0]); // DEBUG
            }
        }
        // check(nc_def_var(ncout_id, desc, NC_FLOAT, 4, std_dim_ids, &std_var_ids[z]));
        for (int j = 0; j < ny; ++j)
        {
            for (int i = 0; i < nx; ++i)
            {

                // printf("\t%f\n", data[0][z][j][i]);
            }
        }
    }

    // end def
    check(nc_enddef(ncout_id));

    // Write NC times, lats, longs
    check(nc_put_vara(ncout_id, times_var_id, time_start, time_count, times));
    check(nc_put_vara(ncout_id, xlat_var_id, grid_start, grid_count, xlats));
    check(nc_put_vara(ncout_id, xlong_var_id, grid_start, grid_count, xlongs));

    check(nc_close(ncout_id)); // CLOSE nc out

    fclose(arl); // close ARL

    return 0;
}
