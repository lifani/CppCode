TARGET = ../output/visionProc
OBJECTS = visionProcess.o visionNode.o main.o tools.o visionMonitor.o

CFLAGS = -g
LDFLAGS = -Wl,-rpath,./ -Wl,-rpath,../opencv/lib -L../output -L../opencv/lib -linterface -lopencv_core -lopencv_calib3d -lpthread
CXX = g++

$(TARGET) : $(OBJECTS)
	$(CXX) -o $(TARGET) $(OBJECTS) $(LDFLAGS)
	
visionProcess.o : visionProcess.cpp
	$(CXX) $(CFLAGS) -c visionProcess.cpp

visionNode.o : visionNode.cpp
	$(CXX) $(CFLAGS) -c visionNode.cpp

tools.o : tools.cpp
	$(CXX) $(CFLAGS) -c tools.cpp

visionMonitor.o : visionMonitor.cpp
	$(CXX) $(CFLAGS) -c visionMonitor.cpp
	
main.o : main.cpp
	$(CXX) $(CFLAGS) -c main.cpp

clean:
	rm $(OBJECTS) $(TARGET)

