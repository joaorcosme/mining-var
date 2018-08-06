CFLAGS=-g -Wall -lpthread
PRG=test
OBJS=CANTest.o CANproChannel.o

all: $(PRG)

$(PRG): $(OBJS)
	g++-6 -o $@ $^ $(CFLAGS) -L/usr/local/lib -lSoftingCan

CANTest.o : CANTest.cpp
	g++-6 -c $^

CANproChannel.o : CANproChannel.cpp
	g++-6 -c $^

.PHONY: clean

clean:
	rm -f $(OBJS) $(PRG) *~
