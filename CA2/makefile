CC=g++

FLAGS=-mavx2

PNG=-llodepng

all: Q1 Q2 Q3 Q4

Q1: Q1.o 
	$(CC) Q1.o -o Q1

Q2: Q2.o 
	$(CC) Q2.o -o Q2

Q3: Q3.o
	$(CC) Q3.o -o Q3

Q4: Q4.o 
	$(CC) Q4.o $(PNG) -o Q4

Q1.o: Q1.cpp
	$(CC) $(FLAGS) -c Q1.cpp

Q2.o: Q2.cpp
	$(CC) $(FLAGS) -c Q2.cpp

Q3.o: Q3.cpp
	$(CC) $(FLAGS) -c Q3.cpp

Q4.o: Q4.cpp
	$(CC) $(FLAGS) -c Q4.cpp

clean:
	rm -f *.o Q1 Q2 Q3 Q4
