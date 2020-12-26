#include <stdio.h>
#include <string.h>
#include <stdlib.h>


long numXY(char str[]);
char *varDesc(char str[]);
long numRecs(long size, long recl);


int main() {

    FILE* arl;

    char *cdata;
    float *rdata; 
    
    char *label;
    char *header;

    long nxy, recl, fsize;

    int rec;

    // Open ARL to file stream
    arl = fopen("wrf.arl", "rb"); 

    // Get the file size
    fseek(arl, 0, SEEK_END); 
    fsize = ftell(arl); 
    rewind(arl); 

    // Read the standard portion of the label(50) and header(108),
    label = (char *)malloc(50);
    header = (char *)malloc(3072);

    fread(label, sizeof(char), 50, arl); 
    fread(header, sizeof(char), 108, arl);
    
    // Get grid dimensions, record length
    nxy = numXY(header);
    recl = nxy + 50; 

    // Allocate array space
    cdata = (char *)malloc(recl * sizeof(char));
    
    // Goto beginning
    rewind(arl);
    rec = 0;
    while ( rec < numRecs(fsize, recl) ) 
    {   
        ++rec;
        fread(label, sizeof(char), 50, arl);
        fread(cdata, sizeof(char), nxy, arl);

    }

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



