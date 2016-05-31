
lib = -ltcmalloc

OS := $(shell uname)

ifeq ($(OS), Darwin)
else
	lib = -ltcmalloc -ldispatch -lBlocksRuntime
endif

	
darwin: libspartanX.a
	clang -o proxy -fblocks main.c SXString.c SXVector.c -L"./" -I"./" -lspartanX -L"/usr/local/lib" -I"/usr/local/include" $(lib)

bsd: libspartanX.a
	clang -o proxy -fblocks main.c SXString.c SXVector.c -L"./" -I"./" -lspartanX -L"/usr/local/lib" -I"/usr/local/include" $(lib)

libspartanX.a: SXServer.o SXClient.o SXQueue.o SXSocket.o
	ar rsc libspartanX.a SXClient.o SXServer.o SXQueue.o SXSocket.o

SXClient.o:
	clang -c -o SXClient.o -fblocks SXClient.c -L"/usr/local/lib" -I"/usr/local/include" $(lib)

SXServer.o:
	clang -c -o SXServer.o -fblocks SXServer.c -L"/usr/local/lib" -I"/usr/local/include" $(lib)

SXQueue.o:
	clang -c -o SXQueue.o -fblocks SXQueue.c -L"/usr/local/lib" -I"/usr/local/include" $(lib)

SXSocket.o:
	clang -c -o SXSocket.o -fblocks SXSocket.c -L"/usr/local/lib" -I"/usr/local/include" $(lib)

clean:
	rm proxy *.o *.a
