all: srv_datetime.o cl_datetime.o
	gcc -o datetime_srv srv_datetime.o
	gcc -o datetime_cl cl_datetime.o

server: srv_datetime.o
	gcc -o datetime_srv srv_datetime.o

client: cl_datetime.o
	gcc -o datetime_cl cl_datetime.o

srv_datetime.o: srv_datetime.c
	gcc -c srv_datetime.c

cl_datetime.o: cl_datetime.c
	gcc -c cl_datetime.c

clean:
	rm datetime_srv datetime_cl srv_datetime.o cl_datetime.o
