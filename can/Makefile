include ../Makefile.defines

PRG = can_test
OUT_LIB = libcan.a
OUT_OBJS = BSFrameHandler.o CANproChannel.o \
		   CANUtils.o DetectionGUI.o
OBJS = $(OUT_OBJS) CANTest.o

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

$(PRG): $(OBJS)
	@echo Creating $(OUT_LIB)...
	@ar rcs $(OUT_LIB) $(OUT_OBJS)
	@echo Linking...
	$(GCC) $^ -o $@ $(DEPS)

%.o : %.cpp
	@echo Compiling $(^)...
	$(GCC) $(CFLAGS) $^

.PHONY: clean

clean:
	rm -f $(OBJS) $(PRG) *~
