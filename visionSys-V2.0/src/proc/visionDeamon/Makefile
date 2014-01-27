TARGET = ../../../output/visionDeamon

OBJECTS = visionDeamon.o

CFLAGS = -g -Wall -I../../../include -I../../../third

LDFLAGS = -Wl,-rpath,./ -L../../../output/lib -L../../../output  -lplatform -lvo -lmt -llog -lxml -lpthread -lrt

CXX = arm-linux-g++ 

$(TARGET) : $(OBJECTS)
	$(CXX) -o $(TARGET) $(OBJECTS) $(LDFLAGS)
	
visionDeamon.o : visionDeamon.cpp visionDeamon.h
	$(CXX) $(CFLAGS) -c visionDeamon.cpp visionDeamon.h
	
clean:
	rm *.o