#include <stdio.h>
#include <string.h>

void main() {

	FILE *arl;

	char header_label[50]; 
	char header[3072]; 

	int year, month, day, hour, ifc;

	char vdesc[4];

	float pole_lat, pole_lon, ref_lat, ref_lon, grid_size, orient; 
	float tan_lat, sync_x, sync_y, sync_lat, sync_lon, reserved; 
	int nx, ny, nz; 


	arl = fopen("wrf.arl", "r");
	//int arl;

	//header_label = fseek(arl, 158L, SEEK_SET);

	fgets(header, 158, arl);

	//printf("%s", header_label);

	sscanf(header, "%2d %2d %2d %2d %2d %*d %4s", 
		&year, &month, &day, &hour, &ifc, vdesc);






 	printf("%s\n", header);
	//printf("%d, %d, %d, %d\n", year, month, day, hour);	
	//sscanf() 
	



}