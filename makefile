
CC = gcc
OBJ = main.o ep_manager.o
LIBOBJ = ep_writer.o

CPPFLAGS = 	-I ./src/ep_writer/header \
			-I ./src/common

vpath %.h ./src/ep_writer/header
vpath % ./src/common
vpath %.cpp ./src/ep_writer/src
vpath %.cpp ./src/easy_package

main: main.o
	CC main.o -o main

ep_package: $(OBJ) libep_writer.a 
	$(CC) $(LIBOBJ) -o ep_package -L. -lep_writer

%.o:%.cpp
	$(CC) $(CPPFLAGS) -c $<

libep_writer.a: $(LIBOBJ)
	ar crv libep_writer.a $(LIBOBJ)


