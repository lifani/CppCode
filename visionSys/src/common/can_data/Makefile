TARGET = ../../../output/libcan.so
OBJECTS = can_data.o
CFLAGS = -Wall -fPIC -I../../../include -I../../../third/can/include -I../../../third/canutils-4.0.6/include
LDFLAGS = -shared -Wl,-rpath,./ -L../../../third/can/lib -lsocketcan

CXX = arm-linux-g++

$(TARGET) : $(OBJECTS)
	$(CXX) -o $(TARGET) $(OBJECTS) $(LDFLAGS)
	
can_data.o : can_data.cpp can_data.h
	$(CXX) $(CFLAGS) -c can_data.cpp can_data.h
	
clean:
	rm *.o