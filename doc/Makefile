CC=mpicc
all: lrc-example lrc-example-hdf

lrc-example:
	$(CC) -g -c lrc-example.c -o lrc-example.o
	$(CC) lrc-example.o -o lrc-example -lreadconfig

lrc-example-hdf:
	$(CC) -g -c lrc-example.c -o lrc-example-hdf.o -DHAVE_HDF5_H
	$(CC) lrc-example-hdf.o -o lrc-example-hdf -lreadconfig -lhdf5 -DHAVE_HDF5_H

clean:
	rm -f *.o lrc-example lrc-example-hdf
