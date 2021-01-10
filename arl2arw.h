#define MAXSIZE 500

typedef struct {
    float val[MAXSIZE][MAXSIZE]; // unpacked 2-d real array
} Grid;

/* ARL FUNC HEADERS */
int numX(char *str);
int numY(char *str);
char *varDesc(char *str);
long numRecs(long size, long recl);
double numExp(char *str);
double numVar1(char *str);
double tanLat(char *str);
double refLon(char *str);
void unpack(double nexp, double var1, size_t nx, size_t ny,
            char *cdata, double rdata[nx][ny]);

Grid unpack2(double nexp, double var1, size_t nx, size_t ny, char *cdata);
/* WRF NC FUNC HEADERS */
void check(int status);
void pullGrid(size_t nx, size_t ny, float grid[nx][ny],  float var[nx*ny]);


Grid makeGrid(size_t nx, size_t ny, float var[nx*ny]);
//CoordGrid pullGrids(size_t nx, size_t ny, float var[nx*ny]);
// double **pullGrid2(size_t nx, size_t ny, float *var);

// void pullGrid3(size_t nx, size_t ny, Grid *grid, float *var );
