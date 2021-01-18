#define MAXSIZE 500

#define NTIME 1
#define XMAX 500
#define YMAX 500
#define ZMAX 100
#define RMAX 100


#define LABSIZE 50

#define NC_OUT "output.nc"

#define LAT_NAME "latitude"
#define LON_NAME "longitude"
#define WEST_EAST "west_east"
#define SOUTH_NORTH "south_north"
#define TIME_NAME "Time"
#define LVL_NAME "level"

#define TIMES "Times"
#define XLAT "XLAT"
#define XLONG "XLONG"
#define DATESTRLEN_NAME "DateStrLen"
#define BOTTOM_TOP "bottom_top"

#define BOTTOM_TOP_STAG "bottom_top_stag"
#define WEST_EAST_STAG "west_east_stag"
#define SOUTH_NORTH_STAG "south_north_stag"

#define UNITS "units"
#define DEGRESS_EAST "degrees_east"
#define DEGREES_NORTH "degrees_north"

#define LAT_UNITS "degrees_north"
#define LON_UNITS "degrees_west"
#define TEMP_UNITS "celsius"

#define PRES "P"
#define TEMP "TEMP"
#define UWND "U"
#define VWND "V"

/* ARL FUNC HEADERS */
int numX(char *str);
int numY(char *str);
int numLvls(char *header);
char *varDesc(char *str);
long numRecs(long size, long recl);
double numExp(char *str);
double numVar1(char *str);
double tanLat(char *str);
double refLon(char *str);
int curLvl(char *label);
void unpack(double nexp, double var1, size_t ny, size_t nx, 
            char *cdata, float rdata[ny][nx]);

/* WRF NC FUNC HEADERS */
void check(int status);


