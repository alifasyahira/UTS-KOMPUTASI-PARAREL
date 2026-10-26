CC = gcc
CFLAGS = -O3 -Wall
LDFLAGS = -lm

all: serial pthreads openmp

serial: src/serial.c
	$(CC) $(CFLAGS) src/serial.c -o serial.exe $(LDFLAGS)

pthreads: src/pthreads.c
	$(CC) $(CFLAGS) src/pthreads.c -o pthreads.exe -fopenmp $(LDFLAGS)

openmp: src/openmp.c
	$(CC) $(CFLAGS) src/openmp.c -o openmp.exe -fopenmp $(LDFLAGS)

clean:
	rm -f *.exe

test: all
	./serial.exe
	./pthreads.exe
	./openmp.exe