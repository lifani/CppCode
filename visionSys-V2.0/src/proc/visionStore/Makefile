TARGET = ../../../output/visionStore
OBJECTS = visionStore.o
CFLAGS = -g -Wall -I../../../include -I../../../third
LDFLAGS = -Wl,-rpath,./ -L../../../output/lib -L../../../output  -lplatform -lmt -llog -lxml -lpthread -lrt

CXX = arm-linux-g++ 

$(TARGET) : $(OBJECTS)
	$(CXX) -o $(TARGET) $(OBJECTS) $(LDFLAGS)
	
visionStore.o : visionStore.cpp visionStore.h
	$(CXX) $(CFLAGS) -c visionStore.cpp visionStore.h
	
clean:
	rm *.o *.gch