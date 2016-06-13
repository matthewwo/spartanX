
lib = -ltcmalloc

darwin: libspartanX.a
	clang -O3 -o proxy -fblocks main.c SXString.c SXVector.c -L"./" -I"./" -lspartanX -L"/usr/local/lib" -I"/usr/local/include" $(lib)


clean:
	rm proxy *.o *.a
