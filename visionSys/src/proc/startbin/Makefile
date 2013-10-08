TARGET = ../../../output/start.bin

OBJECTS = start.o

CFLAGS = -g -Wall -I../../../include

LDFLAGS = -Wl,-rpath,./

CXX = arm-linux-g++

$(TARGET) : $(OBJECTS)
	$(CXX) -o $(TARGET) $(OBJECTS) $(LDFLAGS)
	
start.o : start.cpp
	$(CXX) $(CFLAGS) -c start.cpp
	
clean:
	rm *.o