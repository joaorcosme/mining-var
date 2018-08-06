CFLAGS=-g -Wall -lpthread
PRG=can
SRC=can.cpp
OBJ=can.o

all: $(PRG)

$(PRG): $(OBJ)
	g++-6 -o $@ $^ $(CFLAGS) -L/usr/local/lib -lSoftingCan

$(OBJ) : $(SRC) 
	g++-6 -c $^

.PHONY: clean

clean:
	rm -f $(OBJ) $(PRG) *~
