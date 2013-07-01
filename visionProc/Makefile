TARGET = ../output/visionProc
OBJECTS = visionProcess.o visionNode.o main.o tools.o visionMonitor.o visionImu.o visionStore.o

CFLAGS = -g -I../can/include -I../canutils-4.0.6/include
LDFLAGS = -Wl,-rpath,./ -Wl,-rpath,../opencv/lib -L../output -L../opencv/lib -L../can/lib -lsocketcan -lvo -lopencv_core -lopencv_calib3d -lpthread
CXX = arm-linux-g++

$(TARGET) : $(OBJECTS)
	$(CXX) -o $(TARGET) $(OBJECTS) $(LDFLAGS)
	
visionProcess.o : visionProcess.cpp
	$(CXX) $(CFLAGS) -c visionProcess.cpp

visionNode.o : visionNode.cpp
	$(CXX) $(CFLAGS) -c visionNode.cpp
	
visionImu.o : visionImu.cpp
	$(CXX) $(CFLAGS) -c visionImu.cpp

tools.o : tools.cpp
	$(CXX) $(CFLAGS) -c tools.cpp

visionMonitor.o : visionMonitor.cpp
	$(CXX) $(CFLAGS) -c visionMonitor.cpp
	
visionStore.o : visionStore.cpp
	$(CXX) $(CFLAGS) -c visionStore.cpp
	
main.o : main.cpp
	$(CXX) $(CFLAGS) -c main.cpp

clean:
	rm $(OBJECTS) $(TARGET)

