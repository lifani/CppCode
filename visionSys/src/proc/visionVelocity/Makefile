TARGET = ../../../output/visionVelocity

OBJECTS = visionVelocity.o

CFLAGS = -g -Wall -I../../../include -I../../../third

LDFLAGS = -Wl,-rpath,./ -Wl,-rpath,./opencv/lib -L../../../output/lib -L../../../output -L../../../third/opencv/lib -lplatform -lvo -lmt -llog -lxml -lopencv_core -lopencv_calib3d -lopencv_highgui  -lpthread -lrt

CXX = arm-linux-g++ 

$(TARGET) : $(OBJECTS)
	$(CXX) -o $(TARGET) $(OBJECTS) $(LDFLAGS)
	
visionVelocity.o : visionVelocity.cpp visionVelocity.h
	$(CXX) $(CFLAGS) -c visionVelocity.cpp visionVelocity.h
	
clean:
	rm *.o