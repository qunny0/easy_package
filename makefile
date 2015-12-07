
CC = gcc

AR = ar
CRV = crv

CPPFLAGS = 	-I ./src/ep_writer/header \
			-I ./src/ep_reader/header \
			-I ./src/common

# OBJ = main.o ep_manager.o
# LIBOBJ_W = ep_writer.o ep_utils.o
# LIBOBJ_R = ep_reader.o ep_utils.o

# CPPFLAGS = 	-I ./src/ep_writer/header 	\
# 			-I ./src/ep_reader/header	\
# 			-I ./src/common

# vpath %.h ./src/ep_writer/header
# vpath %.h ./src/ep_reader/header
# vpath % ./src/common
# vpath %.cpp ./src/ep_writer/src
# vpath %.cpp ./src/ep_reader/src
# vpath %.cpp ./src/easy_package
# vpath %.a ./external/zlib/lib

# ep_package: $(OBJ) libep_writer.a libep_reader.a
# 	$(CC) $(OBJ) -o ep_package -L. -lep_writer -lep_reader

# %.o:%.cpp
# 	$(CC) $(CPPFLAGS) -c $<

# libep_writer.a: $(LIBOBJ_W)
# 	ar crv libep_writer.a $(LIBOBJ_W) -L. -lz

# libep_reader.a: $(LIBOBJ_R)
# 	ar crv libep_reader.a $(LIBOBJ_R) -L. -lz

OBJ_W = ep_package.o ep_writer.o ep_utils.o
OBJ_R = ep_package.o ep_reader.o ep_utils.o

vpath %.cpp ./src/ep_writer/src ./src/common ./src/ep_reader/src
vpath %.c  ./src/common

main:ep_writer ep_reader

ep_writer:$(OBJ_W)
	$(AR) $(CRV) libep_writer.a $(OBJ_W)

ep_reader:$(OBJ_R)
	$(AR) $(CRV) libep_reader.a $(OBJ_R)

%.o:%.cpp
	$(CC) $(CPPFLAGS) -c $<
