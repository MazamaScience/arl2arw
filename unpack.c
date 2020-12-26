#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define A 8

struct header {
    char src[A];        char icx[A];        char mn[A];
    char polelat[A];    char polelon[A];    char reflat[A]; 
    char reflon[A];     char size[A];       char orient[A]; 
    char tanlat[A];     char syncxp[A];     char syncyp[A]; 
    char synclat[A];    char synclon[A];    char res[A]; 
    char nx[A];         char ny[A];         char nz[A];
    char kflag[A];      char lenh[A];
};

struct label {

};

void hdecode(struct header *h, char *str)
{
    sscanf(str, 
    "%4c%3c%2c%7c%7c%7c%7c%7c%7c%7c%7c%7c%7c%7c%7c%3c%3c%3c%2c%4c", 
        h->src,     h->icx,     h->mn,
        h->polelat, h->polelon, h->reflat, 
        h->reflon,  h->size,    h->orient, 
        h->tanlat,  h->syncxp,  h->syncyp, 
        h->synclat, h->synclon, h->res, 
        h->nx,      h->ny,      h->nz, 
        h->kflag,   h->lenh);
} 

int main() {

    FILE* fparl;

    fparl = fopen("wrf.arl", "rb"); 

    char label[50];
    char header[3072];

    struct header *head = (struct header *)malloc(sizeof(struct header));

    // Read the standard portion of the label(50) and header(108),
    // decode the header to get dims for recl.
    fread(label, sizeof(char), 50, fparl); 
    fread(header, sizeof(char), 108, fparl);

    hdecode(head, header);

    puts(head->syncyp);

    
    fclose(fparl);

    // Read 
    return 0;
    
}

char ldecode() {

}



