CC=g++

FLAGS=-mavx2

all: Q1 Q2 Q3

Q1: Q1.o 
	$(CC) Q1.o -o Q1

Q2: Q2.o 
	$(CC) Q2.o -o Q2

Q3: Q3.o
	$(CC) Q3.o -o Q3

Q1.o: Q1.cpp
	$(CC) $(FLAGS) -c Q1.cpp

Q2.o: Q2.cpp
	$(CC) $(FLAGS) -c Q2.cpp

Q3.o: Q3.cpp
	$(CC) $(FLAGS) -c Q3.cpp

clean:
	rm -f *.o Q1 Q2 Q3
