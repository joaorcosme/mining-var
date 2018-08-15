GCC = @g++-6
CFLAGS = -c -g -Wpedantic -std=c++17
PRG = can_test

OBJS = BSFrameHandler.o CANproChannel.o \
	   CANUtils.o CANTest.o DetectionGUI.o

DEPS = -lpthread \
	   -lSoftingCan \
	   -lnana \
	   -lstdc++fs \
	   -lX11 \
	   -lrt \
	   -lXft \
	   -lpng \
	   -lasound \
	   -lfontconfig

all: $(PRG)

$(PRG): $(OBJS)
	@echo Linking...
	$(GCC) $^ -o $@ $(DEPS)

%.o : %.cpp
	@echo Compiling $(^)...
	$(GCC) $(CFLAGS) $^

.PHONY: clean

clean:
	rm -f $(OBJS) $(PRG) *~
