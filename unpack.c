#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <netcdf.h>
#include <cmapf.h>

#define LABSIZE 50

int numX(char str[]);
int numY(char str[]);
char *varDesc(char str[]);
long numRecs(long size, long recl);
double numExp(char str[]);
double numVar1(char str[]);
void unpack(double nexp, double var1, size_t nx, size_t ny,
            char cdata[], double rdata[nx][ny]);

int main(int argc, char *argv[])
{

    FILE *arl;  // ARL file stream
    long fsize; // file byte size

    char *label;  // standard label 50-bytes
    char *header; // header 108-bytes
    char *hindex; // header index desc

    int nx, ny;     // x, y dims
    long nxy, recl; // num xy, record length
    double nexp;    // scaling integer
    double var1;    // variable at (1,1)
    
    // Cmapf
    maparam stcprm; // Map parameter struct

    //stcm1p()


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

    puts(header); // print header

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

    return 0;
}

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
            if (i < 5 && j < 25)
                printf("(%ld,%ld): %lf ", i, j, rdata[i][j]);
        }
        vold = rdata[0][j];
        if (j < 25)
            printf(" ...\n");
    }
}

// Be simple.
// Pull string from header
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
