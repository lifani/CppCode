TARGET = ../../../output/visionDaemon

OBJECTS = visionDaemon.o

CFLAGS = -g -rdynamic -Wall -I../../../include -I../../../third

LDFLAGS = -Wl,-rpath,./ -L../../../output/lib -L../../../output -L../../../third/can/lib -lplatform -lvo -lmt -llog -lcan -lsbus -lsocketcan -lxml -lpthread -lrt

CXX = arm-linux-g++ 

$(TARGET) : $(OBJECTS)
	$(CXX) -o $(TARGET) $(OBJECTS) $(LDFLAGS)
	
visionDaemon.o : visionDaemon.cpp visionDaemon.h
	$(CXX) $(CFLAGS) -c visionDaemon.cpp visionDaemon.h
	
clean:
	rm *.o *.gch