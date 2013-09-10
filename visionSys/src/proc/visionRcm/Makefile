TARGET = ../../../output/visionRcm

OBJECTS = visionRcm.o

CFLAGS = -g -Wall -I../../../include -I../../../third

LDFLAGS = -Wl,-rpath,./ -L../../../output/lib -L../../../output -lplatform -lmt -lxml -lpthread -lrt

CXX = g++

$(TARGET) : $(OBJECTS)
	$(CXX) -o $(TARGET) $(OBJECTS) $(LDFLAGS)
	
visionRcm.o : visionRcm.cpp visionRcm.h
	$(CXX) $(CFLAGS) -c visionRcm.cpp visionRcm.h
	
clean:
	rm *.o