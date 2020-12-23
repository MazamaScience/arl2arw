#include <stdio.h>
#include <string.h>

struct Arl {

	char header_label[50]; 
	char header[158];

	int year, month, day, hour, ifc;
	char vdesc[4];

	char model[4];
	int grid_num; 
	int z_coord;
	float pole_lat, pole_lon, ref_lat, ref_lon, grid_size, orient; 
	float tan_lat, sync_x, sync_y, sync_lat, sync_lon, reserved; 
	int nx, ny, nz; 
	int kflg, lenh;

}; 

void main() {

	struct Arl arl;

	FILE * arl_file; // arl file

	// open the file
	arl_file = fopen("wrf.arl", "rb");

	printf("Header Label: \n");
	//fseek(arl_file, 0, SEEK_SET);
	fread(&arl, sizeof(arl.header_label), 1, arl_file);
	sscanf(arl.header_label, "%2d%2d%2d%2d%2d%*d%4s", 
		&arl.year, &arl.month, &arl.day, &arl.hour, &arl.ifc, arl.vdesc);
	printf("%s\n", arl.header_label);

	printf("Header: \n");
	fseek(arl_file, 0, SEEK_SET);
	fread(&arl, sizeof(arl.header), 1, arl_file);
	sscanf(arl.header, "%4s%3d%1d%f%f%f%f%f%f%f%f%f%f%f%f%3d%3d%3d", 
		arl.model,      &arl.grid_num,  &arl.z_coord, 
		&arl.pole_lat,  &arl.pole_lon,  &arl.ref_lat, 
		&arl.ref_lon,   &arl.grid_size, &arl.orient,   
		&arl.tan_lat,   &arl.sync_x,    &arl.sync_y,    
		&arl.sync_lat,  &arl.sync_lon,  &arl.reserved, 
		&arl.nx, 		&arl.ny, 		&arl.nz);

	printf("%d\n", arl.nx);
	
	fclose(arl_file);


}