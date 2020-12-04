.SUFFIXES: 
	FC = gfortran

	SOURCES = arl2arw.f90

arl2arw: $(subst .f90, .o, $(SOURCES))
	$(FC) -o $@ $+