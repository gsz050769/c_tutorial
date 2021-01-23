objects = libjson.o libjson_qual.o libjson_write.o libjson_read.o libjson_print.o libjson_free.o


all: libjson

libjson: $(objects)
	gcc -o libjson $(objects)

libjson.o: libjson.c libjson.h libjson_qual.h libjson_write.h libjson_read.h
	gcc -c libjson.c

libjson_qual.o: libjson_qual.c libjson_qual.h libjson.h
	gcc -c libjson_qual.c

libjson_write.o: libjson_write.c libjson_write.h libjson.h
	gcc -c libjson_write.c

libjson_read.o: libjson_read.c libjson_read.h libjson.h
	gcc -c libjson_read.c

libjson_print.o: libjson_print.c libjson_print.h libjson.h
	gcc -c libjson_print.c

libjson_free.o: libjson_free.c libjson_free.h libjson.h
	gcc -c libjson_free.c

clean:
	rm libjson $(objects)
