#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>

int numX (char *str);
int numY (char *str);
char *varDesc (char *str);
long numRecs (long size, long recl);
double numExp (char *str);
double numVar1 (char *str);
float unpack (char *cdata, double *rdata, int nx, int ny, double nexp, double var1);

int main ( int argc, char **argv )
{
  FILE *arl;			  // ARL file stream 
  char fpath[80];
  char *cdata;			// packed character 
  double *rdata;		// unpacked real  

  char *label;			// standard label 50-bytes
  char *header;			// header 108-bytes
  char *hindex;			//header index

  long fsize;			  // file size
  int nx, ny;			  // x, y dims
  long nxy, recl;   // num xy, record length
  int nexp;				  // scaling integer 
  double var1;			// variable at (1,1) 
  int rec;				  // record indicie

  // Open ARL to file stream
  arl = fopen (argv[1], "rb");

  // Get the file size
  fseek (arl, 0, SEEK_END);
  fsize = ftell (arl);
  rewind (arl);

  // Allocate label and ARL header array space
  label = (char *) malloc (sizeof (char) * 50);
  header = (char *) malloc (sizeof (char) * 3072);	// Overflow 

  // Read the standard portion of the label(50) and header(108)
  fread (label, sizeof (char), 50, arl);
  fread (header, sizeof (char), 108, arl);

  // Store header index label
  hindex = varDesc (label);

  // Get grid dimensions & record length
  nx = numX (header);
  ny = numY (header);
  nxy = nx * ny;
  recl = nxy + 50;

  // Allocate array space
  cdata = (char *) malloc (sizeof (char) * nxy);
  rdata = (double *) malloc (sizeof (double) * nx * ny);

  rewind (arl);
  rec = 0;
  while (rec < 2)//numRecs (fsize, recl)) //  DEBUG
  {
    ++rec;
    fread (label, sizeof (char), 50, arl);
    fread (cdata, sizeof (char), nxy, arl);
    if (strcmp (varDesc (label), hindex) != 0)
    {
      nexp = numExp (label);
      var1 = numVar1 (label);
      unpack (cdata, rdata, nx, ny, nexp, var1);
    }
  }

  // Close ARL file stream
  fclose (arl);

  return 0;
}

// Be simple. 
int numX (char *str)
{
  int ix;
  char cx[4];			// +1 
  int xpos = 93;
  strncpy (cx, str + xpos, 3);
  cx[3] = '\0';
  ix = atoi (cx);
  return ix;
}

int numY (char *str)
{
  int iy;
  char cy[4];
  int ypos = 96;
  strncpy (cy, str + ypos, 3);
  cy[3] = '\0';
  iy = atoi (cy);
  return iy;
}

char *varDesc (char *str)
{
  char *desc;
  int dpos = 14;
  desc = malloc (5);		// extra byte
  strncpy (desc, str + dpos, 4);
  desc[4] = '\0';
  return desc;
}

long numRecs (long size, long recl)
{
  return size / recl;
}

double numExp (char *str)
{
  char nexp[5];			// +1
  int nexpos = 18;
  int exp;
  strncpy (nexp, str + nexpos, 4);
  nexp[4] = '\0';
  exp = atof (nexp);
  return exp;
}

double numVar1 (char *str)
{
  char var1[8];			// +1
  int var1pos = 22;
  double var;
  strncpy (var1, str + var1pos, 7);
  var = atof (var1);
  return var;
}

// needs work
float unpack (char *cdata, double *rdata, int nx, int ny, double nexp, double var1)
{
  double scale, vold;
  int idx, i, j;
  scale = pow (2.0, 7.0 - nexp);
  vold = var1;			// Save prev value
  idx = 0;

  for (j = 1; j < ny; ++j)
  {
    for (i = 1; i < nx; ++i)
    {
      if (i < 1 && j < 1) 
      {
        //get var 1
      } 
      printf (": %lf", vold);
      *(rdata + j * ny + i) = ((int) cdata[idx] - 127.0) / scale + vold;
      vold = *(rdata + j * nx + i);
      printf("\n INDEX: %d\n",idx);
      ++idx;
      // Whhat about edge cases like index 1 or end of??

    }
    vold = *(rdata + j * nx);
  }

  return 0;
}
