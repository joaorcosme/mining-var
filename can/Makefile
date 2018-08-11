CFLAGS=-g -Wall -lpthread -std=c++17
PRG=test
OBJS=CANTest.o CANproChannel.o CANUtils.o

all: $(PRG) tags

$(PRG): $(OBJS)
	g++-6 -o $@ $^ $(CFLAGS) -L/usr/local/lib -lSoftingCan

CANTest.o : CANTest.cpp
	g++-6 -c $^

CANproChannel.o : CANproChannel.cpp
	g++-6 -c $^

CANUtils.o : CANUtils.cpp
	g++-6 -c $^

.PHONY: clean tags

tags:
	ctags -R --languages=c++

clean:
	rm -f $(OBJS) $(PRG) *~
