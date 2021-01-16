#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <netcdf.h>

#include <arl2arw.h>

void unpack4(double nexp, double var1, int lvl, size_t nx, size_t ny,
            char *cdata, float rdata[lvl][nx][ny])
{
    int indx = 0;
    double vold = var1;
    double scale = pow(2.0, (7.0 - nexp));
    for (size_t j = 0; j < ny; ++j)
    {
        for (size_t i = 0; i < nx; ++i)
        {

            rdata[lvl][i][j] = (cdata[indx] - 127.0) / scale + vold;
            vold = rdata[lvl][i][j];
            ++indx;
            if (i < 4 && j < 15)
                printf("(%ld,%ld): %lf ", i, j, rdata[lvl][i][j]);
        }
        vold = rdata[lvl][0][j];
        if (j < 15)
            printf(" ...\n");
    }
    printf(" ...\n");
}

Grid2 unpack3(double nexp, double var1,  int lvl, size_t nx, size_t ny, char *cdata)
{
    Grid2 rdata;
    int indx = 0;
    double vold = var1;
    double scale = pow(2.0, (7.0 - nexp));

    for (size_t j = 0; j < ny; ++j)
    {
        for (size_t i = 0; i < nx; ++i)
        {
            rdata.var[lvl][i][j] = (cdata[indx] - 127.0) / scale + vold;
            vold = rdata.var[lvl][i][j];
            ++indx;
            // if (i < 4 && j < 15)
            //     printf("(%ld,%ld): %lf ", i, j, rdata.val[i][j]);
        }
        vold = rdata.var[lvl][0][j];
        // if (j < 15)
        //     printf(" ...\n");
    }
    // printf(" ...\n");
    return rdata; 
}

/* ARL FUNCTIONS */
Grid unpack2(double nexp, double var1,  size_t nx, size_t ny, char *cdata)
{
    Grid rdata;
    int indx = 0;
    double vold = var1;
    double scale = pow(2.0, (7.0 - nexp));

    for (size_t j = 0; j < ny; ++j)
    {
        for (size_t i = 0; i < nx; ++i)
        {
            rdata.val[i][j] = (cdata[indx] - 127.0) / scale + vold;
            vold = rdata.val[i][j];
            ++indx;
            // if (i < 4 && j < 15)
            //     printf("(%ld,%ld): %lf ", i, j, rdata.val[i][j]);
        }
        vold = rdata.val[0][j];
        // if (j < 15)
        //     printf(" ...\n");
    }
    // printf(" ...\n");
    return rdata; 
}
void unpack1(double nexp, double var1, size_t nx, size_t ny,
            char *cdata)
{
    int indx = 0;
    double vold = var1;
    double scale = pow(2.0, (7.0 - nexp));
    float rdata[nx][ny]; 

    for (size_t j = 0; j < ny; ++j)
    {
        //rdata = malloc(sizeof(float) * ny);
        for (size_t i = 0; i < nx; ++i)
        {
            //*rdata = malloc(sizeof(float) * nx);
            rdata[i][j] = (cdata[indx] - 127.0) / scale + vold;
            vold = rdata[i][j];
            ++indx;
            if (i < 4 && j < 15)
                printf("(%ld,%ld): %lf ", i, j, rdata[i][j]);
        }
        vold = rdata[0][j];
        if (j < 15)
            printf(" ...\n");
    }
    printf(" ...\n");
}
// Unpacking
void unpack(double nexp, double var1, size_t nx, size_t ny,
            char *cdata, double rdata[nx][ny])
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
            // if (i < 4 && j < 15)
            //     printf("(%ld,%ld): %lf ", i, j, rdata[i][j]);
        }
        vold = rdata[0][j];
        // if (j < 15)
        //     printf(" ...\n");
    }
    // printf(" ...\n");
}

// Be simple.
// Pull string from string
char *pullStr(char *str, int pos, int len)
{
    char *sptr;
    sptr = malloc(len + 1);
    strncpy(sptr, str + pos, len);
    sptr[len] = '\0';
    return sptr;
}

int numX(char *str)
{
    int ix;
    char *cx;
    cx = pullStr(str, 93, 3);
    ix = atoi(cx);
    return ix;
}

int numY(char *str)
{
    int iy;
    char *cy;
    cy = pullStr(str, 96, 3);
    iy = atoi(cy);
    return iy;
}

char *varDesc(char *str)
{
    char *desc;
    desc = pullStr(str, 14, 4);
    return desc;
}

long numRecs(long size, long recl)
{
    return size / recl;
}

double numExp(char *str)
{
    int exp;
    char *nexp;
    nexp = pullStr(str, 18, 4);
    exp = atof(nexp);
    return exp;
}

double numVar1(char *str)
{
    char *var1;
    double var;
    var1 = pullStr(str, 36, 17);
    var = atof(var1);
    return var;
}

double tanLat(char *str)
{
    char *ctlat;
    double tlat;
    ctlat = pullStr(str, 50, 8);
    tlat = atof(ctlat);
    return tlat;
}

double refLon(char *str)
{
    char *crlon;
    double rlon;
    crlon = pullStr(str, 30, 8);
    rlon = atof(crlon);
    return rlon;
}

int curLvl(char *label)
{
    char *clvl; 
    int lvl; 
    clvl = pullStr(label, 10, 2);
    lvl = atoi(clvl);
    return lvl;
}

int numLvls(char *header)
{
    char *clvl; 
    int lvl; 
    clvl = pullStr(header, 99, 3);
    lvl = atoi(clvl);
    return lvl;
}

