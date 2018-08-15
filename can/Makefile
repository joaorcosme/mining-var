GCC=g++-6
CFLAGS=-g -Wall -std=c++17
PRG=test
OBJS=BSFrameHandler.o CANproChannel.o CANUtils.o CANTest.o DetectionGUI.o
DEPS=-L/usr/local/lib \
	 -lSoftingCan \
	 -lnana \
	 -lpthread \
	 -lstdc++fs \
	 -lX11 \
	 -lrt \
	 -lXft \
	 -lpng \
	 -lasound \
	 -lfontconfig

all: $(PRG) tags

$(PRG): $(OBJS)
	$(GCC) -o $@ $^ $(CFLAGS) -I/usr/local/include $(DEPS)

DetectionGUI.o : DetectionGUI.cpp
	$(GCC) -c $^

BSFrameHandler.o : BSFrameHandler.cpp
	$(GCC) -c $^

CANproChannel.o : CANproChannel.cpp
	$(GCC) -c $^

CANUtils.o : CANUtils.cpp
	$(GCC) -c $^

CANTest.o : CANTest.cpp
	$(GCC) -c $^

.PHONY: clean tags

tags:
	ctags -R --languages=c++

clean:
	rm -f $(OBJS) $(PRG) *~

format:
	clang-format -style=file -i `find -name "*.h" -or -name "*.cpp"
