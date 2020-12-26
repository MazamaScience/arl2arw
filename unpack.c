#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define HBYTE 8

struct header 
{
    char src[HBYTE];        char icx[HBYTE];        char mn[HBYTE];
    char polelat[HBYTE];    char polelon[HBYTE];    char reflat[HBYTE]; 
    char reflon[HBYTE];     char size[HBYTE];       char orient[HBYTE]; 
    char tanlat[HBYTE];     char syncxp[HBYTE];     char syncyp[HBYTE]; 
    char synclat[HBYTE];    char synclon[HBYTE];    char res[HBYTE]; 
    char nx[HBYTE];         char ny[HBYTE];         char nz[HBYTE];
    char kflag[HBYTE];      char lenh[HBYTE];
};

struct label 
{
    char iyr[HBYTE]; 
    char imo[HBYTE]; 
    char ida[HBYTE]; 
    char ihr[HBYTE];
    char ifc[HBYTE];
    char kvar[HBYTE];
};

void hdecode();
void ldecode();
long numberXY(char str[]);


int main() {

    FILE* fparl;

    fparl = fopen("wrf.arl", "rb"); 

    char label[50];
    char header[3072];

    struct header *hp = (struct header *)malloc(sizeof(struct header));
    struct label *lp = (struct label *)malloc(sizeof(struct label)); 

    long nxy, recl;

    char *cdata;

    // Read the standard portion of the label(50) and header(108),
    fread(label, sizeof(char), 50, fparl); 
    fread(header, sizeof(char), 108, fparl);
    
    // Decode the header
    hdecode(hp, header);
    ldecode(lp, label);

    nxy = numberXY(header);
    recl = nxy + 50; 

    //nxy = atol((*hp).nx) * atol((*hp).ny);
    //recl = nxy + 50; 

    //Allocate array space
    cdata = (char *)malloc(recl * sizeof(char));


    
    fclose(fparl);

    return 0;
    
}

void hdecode(struct header *h, char *str)
{
    sscanf(str, "%4c%3c%2c%7c%7c%7c%7c%7c%7c%7c%7c%7c%7c%7c%7c%3c%3c%3c%2c%4c", 
        h->src,     h->icx,     h->mn,
        h->polelat, h->polelon, h->reflat, 
        h->reflon,  h->size,    h->orient, 
        h->tanlat,  h->syncxp,  h->syncyp, 
        h->synclat, h->synclon, h->res, 
        h->nx,      h->ny,      h->nz, 
        h->kflag,   h->lenh);
} 

void ldecode(struct label *l, char *str)
{
    sscanf(str, "%2c%2c%2c%2c%2c%4c", 
    l->iyr, l->imo, l->ida, l->ihr, l->ifc, l->kvar);
}


// Be simple. 
long numberXY(char str[])
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



