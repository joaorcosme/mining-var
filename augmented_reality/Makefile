include ../Makefile.vars

PRG = ar_app
OBJS = MainAR.o SensorSimulator.o

OPENCV = `pkg-config opencv --cflags --libs`
DEPS = -lpthread $(OPENCV)

$(PRG): $(OBJS)
	@echo Linking...
	$(GCC) $^ -o $@ $(DEPS)

%.o : %.cpp
	@echo Compiling $(^)...
	$(GCC) $(CFLAGS) $^

.PHONY: clean

clean:
	rm -f $(OBJS) $(PRG) *~
