IDIR=$(ROOTDIR)/fuse-2.9.3/include
CC=gcc
FLAGS=-D_FILE_OFFSET_BITS=64 -I$(IDIR)
OBJ=shfs.o
LIBS=-lpthread -lfuse -lrt -ldl

all:shfs

%.o: %.c
	$(CC) -c -o $@ $< $(FLAGS)

shfs:	$(OBJ)
	$(CC) -o $@ $^ $(FLAGS) $(LIBS)

clean:
	rm shfs shfs.o