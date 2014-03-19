TARGET = ../../../output/visionRcm

OBJECTS = visionRcm.o

CFLAGS = -g -rdynamic -Wall -I../../../include -I../../../third -I../../../third/can/include  -I../../../third/canutils-4.0.6/include

LDFLAGS = -Wl,-rpath,./ -L../../../output/lib -L../../../output -L../../../third/can/lib -lplatform -lmt -lxml -llog  -lsocketcan -lcan -lsbus -lpthread -lrt

#LDFLAGS = -Wl,-rpath,./ -L../../../output/lib -L../../../output -lplatform -lmt -lxml -lpthread -lrt

CXX = arm-linux-g++

$(TARGET) : $(OBJECTS)
	$(CXX) -o $(TARGET) $(OBJECTS) $(LDFLAGS)
	
visionRcm.o : visionRcm.cpp visionRcm.h
	$(CXX) $(CFLAGS) -c visionRcm.cpp visionRcm.h
	
clean:
	rm *.o *.gch