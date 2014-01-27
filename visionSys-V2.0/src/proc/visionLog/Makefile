TARGET = ../../../output/visionLog

OBJECTS = visionLog.o

CFLAGS = -g -Wall -I../../../include -I../../../third

LDFLAGS = -Wl,-rpath,./ -L../../../output -lxml

CXX = arm-linux-g++

$(TARGET) : $(OBJECTS)
	$(CXX) -o $(TARGET) $(OBJECTS) $(LDFLAGS)
	
visionLog.o : visionLog.cpp
	$(CXX) $(CFLAGS) -c visionLog.cpp

clean:
	rm *.o