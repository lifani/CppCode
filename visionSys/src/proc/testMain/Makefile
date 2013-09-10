TARGET = ../../../output/a.out
OBJECTS = main.o
CFLAGS = -g -Wall
#LDFLAGS = -Wl,-rpath,./ -L../../../output -L../../../third/can/lib -lsocketcan -lcan
LDFLAGS=

CXX = g++

$(TARGET) : $(OBJECTS)
	$(CXX) -o $(TARGET) $(OBJECTS) $(LDFLAGS)
	
main.o : main.cpp
	$(CXX) $(CFLAGS) -c main.cpp

clean:
	rm *.o