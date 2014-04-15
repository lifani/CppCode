TARGET = ../../../output/visionRcm

OBJECTS = visionRcm.o

CFLAGS = -g -rdynamic -Wall -I../../../include -I../../../third -I../../../third/can/include  -I../../../third/canutils-4.0.6/include

LDFLAGS = -Wl,-rpath,./ -Wl,-rpath,./opencv/lib -L../../../output/lib -L../../../output -L../../../third/opencv/lib -L../../../third/can/lib -lplatform -lmt -lxml -llog -lbm -lsocketcan -lcan -lsbus -lopencv_core  -lopencv_imgproc -lopencv_highgui -lpthread -lrt


CXX = arm-linux-g++

$(TARGET) : $(OBJECTS)
	$(CXX) -o $(TARGET) $(OBJECTS) $(LDFLAGS)
	
visionRcm.o : visionRcm.cpp visionRcm.h
	$(CXX) $(CFLAGS) -c visionRcm.cpp visionRcm.h
	
clean:
	rm *.o *.gch