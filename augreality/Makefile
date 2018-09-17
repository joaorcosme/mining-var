include ../Makefile.defines

PRG1 = ar_app1
OBJS1 = MainAR1.o SensorSimulator.o BarGraph.o

PRG2 = ar_app2
OBJS2 = MainAR2.o SensorSimulator.o BarGraph.o


OPENCV = `pkg-config opencv --cflags --libs`
DEPS = -lpthread $(OPENCV) \
	   -L../can -lcan -lSoftingCan

all: $(PRG1) $(PRG2)

$(PRG1): $(OBJS1)
	@echo Linking...
	$(GCC) $^ -o $@ $(DEPS)

$(PRG2): $(OBJS2)
	@echo Linking...
	$(GCC) $^ -o $@ $(DEPS)

%.o : %.cpp
	@echo Compiling $(^)...
	$(GCC) $(CFLAGS) $^

.PHONY: clean

clean:
	rm -f $(OBJS1) $(OBJS2) $(PRG1) $(PRG2) *~
