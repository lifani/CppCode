TARGET = ../../../output/visionBm

OBJECTS = visionBm.o

CFLAGS = -g -rdynamic -Wall -I../../../include -I../../../third

LDFLAGS = -Wl,-rpath,./ -L../../../output/lib -L../../../output  -lplatform -lvo -lmt -llog -lxml -lpthread -lrt


CXX = arm-linux-g++ 

$(TARGET) : $(OBJECTS)
	$(CXX) -o $(TARGET) $(OBJECTS) $(LDFLAGS)
	
visionBm.o : visionBm.cpp visionBm.h
	$(CXX) $(CFLAGS) -c visionBm.cpp visionBm.h
	
clean:
	rm *.o *.gch