#include <stdio.h>
#include <string.h>
#include <stdlib.h>

struct header {
    char src[4]; char icx[3]; char mn[2];
    char polelat[7]; char polelon[7]; char reflat[7]; 
    char reflon[7]; char size[7]; char orient[7]; 
    char tanlat[7]; char syncxp[7]; char syncyp[7]; 
    char synclat[7]; char synclon[7]; char res[7]; 
    char nx[3]; char ny[3]; char nz[3];
    char kflag[2]; char lenh[4];
};

struct header hdecode(struct header h, char *str)
{

    memcpy(&h, str, 108);
    return h;

} 

int main() {

    FILE* fparl;

    fparl = fopen("wrf.arl", "rb"); 

    char label[50];
    char header[3072];
    size_t lenh;

    struct header head;

    // Read the standard portion of the label(50) and header(108),
    // decode the header to get dims for recl.
    fread(label, sizeof(char), 50, fparl); 
    fread(header, sizeof(char), 108, fparl);

    head = hdecode(head, header);

    //printf("%s\n%d\n%d\n%f\n%f\n%f",head.src, head.icx, head.mn, head.polelat, head.polelon, head.reflat);
    //printf("%s\n%s\n", head.src, head.nx);
    fclose(fparl);

    // Read 
    return 0;
    
}

char ldecode() {

}



