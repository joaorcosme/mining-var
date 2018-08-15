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

all: tags $(PRG)

$(PRG): $(OBJS)
	@echo Linking...
	$(GCC) $^ -o $@ $(DEPS)
	@echo ----------------------
	@echo Done!

%.o : %.cpp
	@echo Compiling $(^)...
	$(GCC) $(CFLAGS) $^

.PHONY: clean tags

tags:
	@echo Generating tags...
	@ctags -R --languages=c++

clean:
	rm -f $(OBJS) $(PRG) *~

format:
	clang-format -style=file -i	`find -name "*.h" -or -name "*.cpp"`
