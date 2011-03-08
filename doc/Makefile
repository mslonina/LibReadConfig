all: lrc-example lrc-example-hdf

lrc-example:
	mpicc -c lrc-example.c -o lrc-example.o
	mpicc lrc-example.o -o lrc-example -lreadconfig

lrc-example-hdf:
	mpicc -c lrc-example.c -o lrc-example.o -DHAVE_HDF5_H
	mpicc lrc-example.o -o lrc-example-hdf -lreadconfig -lhdf5

clean:
	rm -f *.o lrc-example lrc-example-hdf
