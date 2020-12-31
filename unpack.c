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
    
    
    maparam stcprm;


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
int numX(char str[])
{
    int ix;
    char cx[4]; // +1
    int xpos = 93;

    strncpy(cx, str + xpos, 3);
    cx[3] = '\0';
    ix = atoi(cx);

    return ix;
}

int numY(char str[])
{
    int iy;
    char cy[4]; // +1
    int ypos = 96;

    strncpy(cy, str + ypos, 3);
    cy[3] = '\0';
    iy = atoi(cy);

    return iy;
}

char *varDesc(char str[])
{
    char *desc;
    int dpos = 14;

    desc = malloc(5); // +1
    strncpy(desc, str + dpos, 4);
    desc[4] = '\0';

    return desc;
}

long numRecs(long size, long recl)
{
    return size / recl;
}

double numExp(char str[])
{
    char nexp[5]; // +1
    int nexpos = 18;
    int exp;

    strncpy(nexp, str + nexpos, 4);
    nexp[4] = '\0';
    exp = atof(nexp);

    return exp;
}

double numVar1(char str[])
{
    char var1[18]; // +1
    int var1pos = 36;
    double var;

    strncpy(var1, str + var1pos, 17);
    var1[17] = '\0';
    var = atof(var1);

    return var;
}
