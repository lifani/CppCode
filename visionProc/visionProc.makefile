objects = visionProcess.o visionNode.o main.o
CXX = g++

visionProc : $(objects)
	$(CXX) -g -L../output -linterface -Wl,-rpath,./ -Wl,-rpath,../opencv/lib -L../opencv/lib -lopencv_core -lopencv_calib3d -lpthread -o ../output/visionProc $(objects)

visionProcess.o : visionProcess.cpp
	$(CXX) -g -c visionProcess.cpp

visionNode.o : visionNode.cpp
	$(CXX) -g -c visionNode.cpp

main.o : main.cpp
	$(CXX) -g -c main.cpp

clean:
	rm $(objects) ../output/visionProc

