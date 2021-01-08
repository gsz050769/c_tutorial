srv_objects = srv_datetime.o
cl_objects = cl_datetime.o

objects = $(srv_objects) $(cl_objects)

all: server client 

server: $(srv_objects)
	gcc -o datetime_srv srv_datetime.o

client: $(cl_objects)
	gcc -o datetime_cl cl_datetime.o

srv_datetime.o: srv_datetime.c
	gcc -c srv_datetime.c

cl_datetime.o: cl_datetime.c
	gcc -c cl_datetime.c

clean:
	rm datetime_srv datetime_cl $(objects)
