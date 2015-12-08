
CC = gcc

AR = ar
CRV = crv

CPPFLAGS = 	-I ./src/ep_writer/header \
			-I ./src/ep_reader/header \
			-I ./src/easy_package	\
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

# ./ep_package /Users/fubei/Desktop/ep_test.ep /Users/fubei/Parkour/project/Parkour/Run/client/iPhone/armatures

OBJ_P = ep_manager.o main.o
OBJ_W = ep_package.o ep_writer.o ep_utils.o
OBJ_R = ep_package.o ep_reader.o ep_utils.o

vpath %.cpp ./src/ep_writer/src \
			./src/common \
			./src/ep_reader/src \
			./src/easy_package
vpath %.c  ./src/common

ep_package:ep_writer ep_reader $(OBJ_P)
	$(CC) $(OBJ_P) -o ep_package -L. -lep_writer -lep_reader -lz -lstdc++

ep_writer:$(OBJ_W)
	$(AR) $(CRV) libep_writer.a $(OBJ_W)

ep_reader:$(OBJ_R)
	$(AR) $(CRV) libep_reader.a $(OBJ_R)

%.o:%.cpp
	$(CC) $(CPPFLAGS) -c $<
