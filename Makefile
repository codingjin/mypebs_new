CC = g++
CFLAGS = -w -fPIC
INCLUDES = -I/usr/local/include
LIBS = -lm -lpthread -ldl -lpfm

OBJ1 = pebs
SRC1 = main.c pebs.c

OBJ2 = calts
SRC2 = calts.c

OBJ3 = calp
SRC3 = calp.c

all: $(SRC1) $(SRC2) $(SRC3)
	$(CC) $(CFLAGS) $(INCLUDES) -o $(OBJ1) $(SRC1) $(LIBS)
	$(CC) $(CFLAGS) $(INCLUDES) -o $(OBJ2) $(SRC2)
	$(CC) $(CFLAGS) $(INCLUDES) -o $(OBJ3) $(SRC3)

.PHONY: clean

clean:
	rm -f $(OBJ1) $(OBJ2) $(OBJ3)




