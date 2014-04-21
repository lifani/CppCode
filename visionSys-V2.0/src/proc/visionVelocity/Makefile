TARGET = ../../../output/visionVelocity

OBJECTS = visionVelocity.o

CFLAGS = -g -rdynamic -Wall -I../../../include -I../../../third

LDFLAGS = -Wl,-rpath,./ -L../../../output/lib -L../../../output -L../../../third/can/lib -lplatform -lvo -lmt -llog -lxml -lcan -lsbus -lsocketcan -lpthread -lrt


CXX = arm-linux-g++ 

$(TARGET) : $(OBJECTS)
	$(CXX) -o $(TARGET) $(OBJECTS) $(LDFLAGS)
	
visionVelocity.o : visionVelocity.cpp visionVelocity.h
	$(CXX) $(CFLAGS) -c visionVelocity.cpp visionVelocity.h
	
clean:
	rm *.o *.gch