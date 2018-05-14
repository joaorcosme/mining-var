CC = g++ -std=c++11
CFLAGS = -Wall
OPENCV = `pkg-config opencv --cflags --libs`

.PHONY : all

t : main.cpp
	$(CC) $(CFLAGS) -o t main.cpp $(OPENCV)
