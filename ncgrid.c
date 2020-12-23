#include <stdio.h> 
#include <string.h>
#include <netcdf.h>

#define NCFILE "wrfout.nc"

int main() {

	void check(int status) 
	{
		if ( status != NC_NOERR ) {
			printf("Error: %s\n", nc_strerror(status)); 
		}
	}

	int status; // io for fortran

	int ncid;
	int latid, lonid;
	char latname, lonname;
	size_t latlen, lonlen;

	// Open the nc file	
	check(nc_open(NCFILE, NC_NOWRITE, &ncid));

	// get lon lat ids
	check(nc_inq_dimid(ncid, "west_east", &lonid));
	check(nc_inq_dimid(ncid, "south_north", &latid));

	// get dims (nx ny) of lat lon dimensions
	check(nc_inq_dimlen(ncid, lonid, &lonlen));
	check(nc_inq_dimlen(ncid, latid, &latlen));

}