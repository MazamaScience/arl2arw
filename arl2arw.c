#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <netcdf.h>
#include <arl2arw.h>
static double data[500][500];
int main(int argc, char **argv)
{

    /* ARL */
    FILE *arl;  // ARL file stream
    long fsize; // file byte size

    char *label;  // standard label 50-bytes
    char *header; // header 108-bytes
    char *hindex; // header index desc

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

    // Get grid dimensions, record length, and number of levels
    int nx = numX(header);
    int ny = numY(header);
    long nxy = nx * ny;
    long recl = nxy + LABSIZE;
    int nLvls = numLvls(header);

    // Allocate packed char array space
    char *cdata = malloc(sizeof(char) * ((size_t)nxy));
    // rdata = malloc(sizeof(double[(size_t)nx][(size_t)ny]));

    int nrecs = numRecs(fsize, recl);

    // Count records per level (including 0th)
    rewind(arl);
    int rec = 0;
    int lvl = 0;
    int *rec_count = calloc(nLvls, sizeof(int));
    while (rec < nrecs)
    {
        ++rec;
        fread(label, sizeof(char), LABSIZE, arl);
        fread(cdata, sizeof(char), (size_t)nxy, arl);
        rec_count[lvl] += 1;
        if (curLvl(label) != lvl)
        {
            ++lvl;
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
    
    // Create NC file
    int ncout_id;
    check(nc_create(NC_OUT, NC_CLOBBER, &ncout_id));

    // Define dimensions
    check(nc_def_dim(ncout_id, TIME_NAME, NC_UNLIMITED, &times_dim_id));
    check(nc_def_dim(ncout_id, DATESTRLEN_NAME, 19, &dsl_dim_id)); // Look into
    check(nc_def_dim(ncout_id, WEST_EAST, ny, &xlat_dim_id));
    check(nc_def_dim(ncout_id, SOUTH_NORTH, nx, &xlong_dim_id));
    check(nc_def_dim(ncout_id, BOTTOM_TOP, nLvls, &lvls_dim_id));
    
    // Time variables
    int times_dimids[] = {times_dim_id, dsl_dim_id}; 
    check(nc_def_var(ncout_id, TIME_VAR, NC_CHAR, 2, times_dimids, &times_var_id));

    // Latitude longitude variables
    int grid_dimids[] = {times_dim_id, xlat_dim_id, xlong_dim_id};
    check(nc_def_var(ncout_id, XLAT, NC_FLOAT, 3, grid_dimids, &xlat_var_id));
    check(nc_def_var(ncout_id, XLONG, NC_FLOAT, 3, grid_dimids, &xlong_var_id));

    // Unpacked varibales
    int std_dim_ids[] = {times_dim_id, lvls_dim_id, xlat_dim_id, xlong_dim_id};
    int std_var_ids[nLvls];
    // write data
    rewind(arl);
    for (int z = 0; z < nLvls; ++z) // For every level z
    {
        char *desc;
        for (rec = 0; rec < rec_count[z]; ++rec) // For every record in level z
        {
            fread(label, sizeof(char), LABSIZE, arl);
            fread(cdata, sizeof(char), (size_t)nxy, arl);
            desc = varDesc(label);
            if (strcmp(desc, hindex) != 0)
            {
                double nexp = numExp(label);
                double var1 = numVar1(label);
                unpack(nexp, var1, nx, ny, cdata, data); // unpack to data
                // printf("\t%f\n", data[0][0]);
            }
        }
        check(nc_def_var(ncout_id, desc, NC_FLOAT, 4, std_dim_ids, &std_var_ids[z]));
        for ( int j= 0; j < ny; ++j)
        {
            for ( int i = 0; i < nx; ++i)
            {
                
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
    

    // rewind(arl);
    // for ( int lvl = 0; lvl < nLvls; ++lvl)
    // {

    // }

    // Unpack each record described by its 50-byte record label
    // rewind(arl);
    // int rec = 0;
    // while (rec < nrecs)
    // {
    //     ++rec;
    //     fread(label, sizeof(char), LABSIZE, arl);
    //     fread(cdata, sizeof(char), (size_t)nxy, arl);
    //     if (strcmp(varDesc(label), hindex) != 0)
    //     {
    //         double nexp = numExp(label);
    //         double var1 = numVar1(label);
    //         int lvl = curLvl(label);

    //         // unpack4(nexp, var1, lvl, nx, ny, cdata, data.var);

    //         //Grid2 p = unpack3(nexp, var1, lvl, (size_t)nx, (size_t)ny, cdata);

    //         // data[rec] = unpack2(nexp, var1, (size_t)nx, (size_t)ny, cdata);
    //         // data[rec].level = curLvl(label);
    //         // strncpy(data[rec].label, label, 50);
    //         // printf("z: %d\n\tdesc: %s\n", data[rec].level, data[rec].label);
    //     }
    // }

    // printf("%f", data.var[0][0][0]);

    // rewind(arl);
    // // fread(label, sizeof(char), LABSIZE, arl);
    // // fread(cdata, sizeof(char), (size_t)nxy, arl);
    // int currentLevel = 0;//curLvl(label);

    // for ( int z = 0; z < nLvls ; ++z)
    // {
    //     printf("z: %d\n", z);

    //     while ( currentLevel == z )
    //     {
    //         // NOTE: DOES NOT CAPTURE LAST LEVEL!
    //         if ( currentLevel == nLvls - 1) break;

    //         printf("\tclvl: %d, desc: %s\n", currentLevel, varDesc(label));
    //         fread(label, sizeof(char), LABSIZE, arl);
    //         fread(cdata, sizeof(char), (size_t)nxy, arl);

    //         currentLevel = curLvl(label);

    //     }

    //     // while ( z == currentLevel + 1)
    //     // {

    //     //     fread(label, sizeof(char), LABSIZE, arl);
    //     //     fread(cdata, sizeof(char), (size_t)nxy, arl);
    //     //     printf("\tclvl: %d, desc: %s\n", currentLevel, varDesc(label));
    //     //     currentLevel = curLvl(label);

    //     // }
    // }

    // Close ARL file stream
    // fclose(arl);

    //     /* NETCDF */
    //     int ncid;
    //     int lat_dimid, lat_varid;
    //     int lon_dimid, lon_varid;

    //     size_t lat_diml, lon_diml;

    //     float *lats, *lons;

    //     // Open the netcdf
    //     check(nc_open(argv[2], NC_NOWRITE, &ncid));

    //     // Check the dimensions
    //     check(nc_inq_dimid(ncid, "south_north", &lat_dimid));
    //     check(nc_inq_dimlen(ncid, lat_dimid, &lat_diml));

    //     check(nc_inq_dimid(ncid, "west_east", &lon_dimid));
    //     check(nc_inq_dimlen(ncid, lon_dimid, &lon_diml));

    //     if ((int)lat_diml != ny || (int)lon_diml != nx)
    //         return 2;

    //     // Allocate coordinate space
    //     lats = malloc(sizeof(float) * lat_diml * lon_diml);
    //     lons = malloc(sizeof(float) * lat_diml * lon_diml);

    //     // Pull the coordinate grid (!)
    //     check(nc_inq_varid(ncid, "XLAT", &lat_varid));
    //     check(nc_get_var_float(ncid, lat_varid, &lats[0]));

    //     check(nc_inq_varid(ncid, "XLONG", &lon_varid));
    //     check(nc_get_var_float(ncid, lon_varid, &lons[0]));

    //     // Create lat and lon grids
    //     Grid glons = makeGrid(nx, ny, lons);
    //     Grid glats = makeGrid(nx, ny, lats);

    //     int ncid_out;
    //     int lon_dimid_out;
    //     int lat_dimid_out;
    //     int lon_varid_out;
    //     int lat_varid_out;
    //     int time_dimid;
    //     int time_varid;
    //     int datestrlen_dimid;
    //     int datestrlen_varid;

    //     // Create NC file
    //     check(nc_create(NC_OUT, NC_CLOBBER, &ncid_out));

    //     // Define dimensions
    //     check(nc_def_dim(ncid_out, TIME_NAME, NC_UNLIMITED, &time_dimid));
    //     check(nc_def_dim(ncid_out, DATESTRLEN_NAME, 19, &datestrlen_dimid));
    //     check(nc_def_dim(ncid_out, WEST_EAST, lon_diml, &lon_dimid_out));
    //     check(nc_def_dim(ncid_out, SOUTH_NORTH, lat_diml, &lat_dimid_out));

    //     int lvl_dimid;
    //     check(nc_def_dim(ncid_out, BOTTOM_TOP, nLvls, &lvl_dimid));

    //     // Time variable
    //     int times_dimids[] = {time_dimid, datestrlen_dimid};
    //     check(nc_def_var(ncid_out, TIME_VAR, NC_CHAR, 2, times_dimids, &time_varid));

    //     // Latitude variable
    //     int std_dimids[] = {time_dimid, lat_dimid_out, lon_dimid_out};
    //     check(nc_def_var(ncid_out, XLAT, NC_FLOAT, 3, std_dimids, &lat_varid_out));

    //    // Longitude variable
    //     // int xlong_dimids[] = {time_dimid, lat_dimid_out, lon_dimid_out};
    //     check(nc_def_var(ncid_out, XLONG, NC_FLOAT, 3, std_dimids, &lon_varid_out));

    //     // Pressure variable
    //     // int prss_varid;
    //     // check(nc_def_var(ncid_out, "PRSS", NC_FLOAT, 3, std_dimids, &prss_varid));

    //     // End define dims
    //     check(nc_enddef(ncid_out));

    //     // // Close the app
    //     check(nc_close(ncid_out));

    return 0;
}
