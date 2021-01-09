objects = mysig.o


all: mysig 

mysig: mysig.o
	gcc -o mysig mysig.o

mysig.o: mysig.c
	gcc -c mysig.c

clean:
	rm mysig $(objects)
