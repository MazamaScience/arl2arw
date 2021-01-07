CC = gcc
CFLAGS = -I .
LIBS = -lnetcdf -lm 

compile: arl2arw.c func_arl.c func_wrf.c
	$(CC) -o arl2arw arl2arw.c func_arl.c func_wrf.c $(LIBS) $(CFLAGS)
