CFLAGS=-g -Wall -std=c++17
PRG=test
OBJS=BSFrameHandler.o CANproChannel.o CANUtils.o CANTest.o

all: $(PRG) tags

$(PRG): $(OBJS)
	g++-6 -o $@ $^ $(CFLAGS) -I/usr/local/include -L/usr/local/lib -lSoftingCan -lnana -lstdc++fs -lX11 -lpthread -lrt -lXft -lpng -lasound -lfontconfig

BSFrameHandler.o : BSFrameHandler.cpp
	g++-6 -c $^

CANproChannel.o : CANproChannel.cpp
	g++-6 -c $^

CANUtils.o : CANUtils.cpp
	g++-6 -c $^

CANTest.o : CANTest.cpp
	g++-6 -c $^

.PHONY: clean tags

tags:
	ctags -R --languages=c++

clean:
	rm -f $(OBJS) $(PRG) *~
