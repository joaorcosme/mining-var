GCC = @g++-6
CFLAGS = -c -g -Wpedantic -std=c++17
PRG = aaaar

OBJS = MainAR.o

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
