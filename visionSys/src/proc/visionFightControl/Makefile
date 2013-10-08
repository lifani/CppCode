TARGET = ../../../output/visionFightCtl

OBJECTS = visionFightCtl.o

CFLAGS = -g -Wall -I../../../include -I../../../third

LDFLAGS = -Wl,-rpath,./ -L../../../output/lib -L../../../output -lplatform -lmt -lxml -lpthread -lrt

CXX = arm-linux-g++

$(TARGET) : $(OBJECTS)
	$(CXX) -o $(TARGET) $(OBJECTS) $(LDFLAGS)
	
visionFightCtl.o : visionFightCtl.cpp visionFightCtl.h
	$(CXX) $(CFLAGS) -c visionFightCtl.cpp visionFightCtl.h
	
clean:
	rm *.o
