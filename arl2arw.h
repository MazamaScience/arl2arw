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
void pullGrid(size_t nx, size_t ny, float grid[nx][ny], float var[nx*ny]);
