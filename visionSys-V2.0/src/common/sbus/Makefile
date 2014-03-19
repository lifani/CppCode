TARGET = ../../../output/libsbus.so
OBJECTS = sbus.o
CFLAGS = -g -rdynamic -Wall -fPIC -I../../../include 
LDFLAGS = -shared -Wl,-rpath,./ -L../../../third/can/lib -lsocketcan

CXX = arm-linux-g++

$(TARGET) : $(OBJECTS)
	$(CXX) -o $(TARGET) $(OBJECTS) $(LDFLAGS)
	
sbus.o : sbus.cpp
	$(CXX) $(CFLAGS) -c sbus.cpp

clean:
	rm *.o