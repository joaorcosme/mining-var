GCC = @g++-6
CFLAGS = -g -Wpedantic -std=c++17
PRG = aaaar

OPENCV = `pkg-config opencv --cflags --libs`
DEPS = -lpthread $(OPENCV)

$(PRG): main.cpp
	$(GCC) $(CFLAGS) -o $@ $^ $(DEPS)

.PHONY: clean tags

clean:
	rm -f $(PRG).o $(PRG) *~

format:
	clang-format -style=file -i	`find -name "*.h" -or -name "*.cpp"`
