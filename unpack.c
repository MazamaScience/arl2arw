#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>


long numXY(char str[]);
char *varDesc(char str[]);
long numRecs(long size, long recl);
int numExp(char *str);
float numVar1(char *str); 
float unpack(char *cdata, float *rdata, long nx, long ny, int nexp, float var1);

int main() {

    FILE *arl;

    char *cdata; 
    float *rdata; 
    
    char *label;
    char *header;

    long nxy, recl, fsize;

    int rec;

    char *hindex; //header index

    int nexp;
    float var1; 

    // Open ARL to file stream
    arl = fopen("wrf.arl", "rb"); 

    // Get the file size
    fseek(arl, 0, SEEK_END); 
    fsize = ftell(arl); 
    rewind(arl); 

    // Allocate label and ARL header array space
    label = (char *)malloc(sizeof(char) * 50);
    header = (char *)malloc(sizeof(char) * 3072);


    // Read the standard portion of the label(50) and header(108)
    fread(label, sizeof(char), 50, arl); 
    fread(header, sizeof(char), 108, arl);
    
    // Store header index label
    hindex = varDesc(label);

    // Get grid dimensions & record length
    nxy = numXY(header);
    recl = nxy + 50; 

    // Allocate array space
    cdata = (char *)malloc(sizeof(char) * nxy);
    rdata = (float *)malloc(sizeof(float) * 1); // DEBUG 
    
    rewind(arl);
    rec = 0;
    while ( rec < numRecs(fsize, recl) ) 
    {   
        ++rec;
        fread(label, sizeof(char), 50, arl);
        fread(cdata, sizeof(char), nxy, arl);
        if ( strcmp(varDesc(label), hindex) != 0 ) 
        {

            nexp = numExp(label);
            var1 = numVar1(label);

            printf("%d\t%f\n", nexp, var1);

            //unpack(cdata, )
            
            
        } 
        
        
    }

    // Close ARL file stream
    fclose(arl);

    return 0;

}


// Be simple. 
long numXY(char str[])
{
    int nxy;
    char nx[4]; // extra byte
    char ny[4]; 
    int nxpos = 93;
    int nypos = nxpos + 3; 
    
    // copy three literal char 
    strncpy(nx, str + nxpos, 3);
    strncpy(ny, str + nypos, 3);

    // add null char to extra buffer
    nx[3] = '\0';
    ny[3] = '\0';

    nxy = atol(nx) * atol(ny);

    return nxy; 
}

char *varDesc(char str[]) 
{
    char *desc; 
    int dpos = 14;
    
    desc = malloc(5); // extra byte
    strncpy(desc, str + dpos, 4); 
    desc[4] = '\0';

    return desc;
}

long numRecs(long size, long recl)
{   
    return size / recl;
}

int numExp(char *str) {
    char nexp[5]; // +1
    int nexpos = 18; 
    int exp;

    strncpy(nexp, str + nexpos, 4);
    nexp[4] = '\0';

    exp = atoi(nexp); 
    return exp; 
}

float numVar1(char *str) {
    char var1[8]; // +1
    int var1pos = 22; 
    float var;
    strncpy(var1, str + var1pos, 7); 
    var = atof(var1);
    return var;
}

float unpack(char *cdata, float *rdata, long nx, long ny, int nexp, float var1) 
{
    //float *rdata;
    float scale, vold;
    int indx, i, j;
    
    scale = pow(2.0, 7-nexp);
    printf("%f", scale);

    return 0; 
}



