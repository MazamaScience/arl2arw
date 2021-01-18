CC = gcc
CFLAGS = -I . -Wall
LIBS = -lnetcdf -lm 

compile: arl2arw.c fun_arl2arw.c
	$(CC) -o arl2arw arl2arw.c fun_arl2arw.c $(LIBS) $(CFLAGS)
