CC=clang
XFLAG=-Xclang -fopenmp
CFLAGS=-I/opt/homebrew/opt/libomp/include
LDFLAGS=-L/opt/homebrew/opt/libomp/lib
LIBS=-lomp
PNG_LIB=-llodepng

all: Q1 Q2 Q3 Q4

Q1: Q1.c
	$(CC) $(XFLAG) $(LDFLAGS) $(CFLAGS) $(LIBS) -lomp Q1.c -o Q1

Q2: Q2.c
	$(CC) $(XFLAG) $(LDFLAGS) $(CFLAGS) $(LIBS) -lomp Q2.c -o Q2

Q3: Q3.c
	$(CC) $(XFLAG) $(LDFLAGS) $(CFLAGS) $(LIBS) -lomp Q3.c -o Q3

Q4: Q3.c
	$(CC) $(XFLAG) $(LDFLAGS) $(CFLAGS) $(LIBS) -lomp Q4.c -o Q4


clean:
	rm -f *.o Q1 Q2 Q3 Q4
