TARGET = ../../output/lib/libplatform.a

OBJECTS = base_vision.o monitor.o queueCtrl.o platform.o

CFLAGS = -g -rdynamic -Wall -fPIC -I../../include -I../../third

LDFLAGS = ar -r

CXX = arm-linux-g++

$(TARGET) : $(OBJECTS)
	$(LDFLAGS) $(TARGET) $(OBJECTS)

base_vision.o : base_vision.cpp
	$(CXX) $(CFLAGS) -c base_vision.cpp
	
monitor.o : monitor.cpp
	$(CXX) $(CFLAGS) -c monitor.cpp

queueCtrl.o : queueCtrl.cpp
	$(CXX) $(CFLAGS) -c queueCtrl.cpp
	
platform.o : platform.cpp
	$(CXX) $(CFLAGS) -c platform.cpp
	
clean:
	rm *.o