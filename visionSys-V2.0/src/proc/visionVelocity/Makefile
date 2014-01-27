TARGET = ../../../output/visionVelocity

OBJECTS = visionVelocity.o

CFLAGS = -g -Wall -I../../../include -I../../../third

LDFLAGS = -Wl,-rpath,./ -L../../../output/lib -L../../../output  -lplatform -lvo -lmt -llog -lxml -lpthread -lrt

#LDFLAGS = -Wl,-rpath,./ -L../../../output/lib -L../../../output -lplatform -lmt -lxml -lpthread -lrt

CXX = arm-linux-g++ 

$(TARGET) : $(OBJECTS)
	$(CXX) -o $(TARGET) $(OBJECTS) $(LDFLAGS)
	
visionVelocity.o : visionVelocity.cpp visionVelocity.h
	$(CXX) $(CFLAGS) -c visionVelocity.cpp visionVelocity.h
	
clean:
	rm *.o *.gch