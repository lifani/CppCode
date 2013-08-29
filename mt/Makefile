TARGET = ../output/libmt.so
OBJECTS = mt.o transdata.o vSem.o
CFLAGS = -g -Wall
LDFLAGS = -shared -fPIC

CXX = g++

$(TARGET) : $(OBJECTS)
	$(CXX) -o $(TARGET) $(OBJECTS) $(LDFLAGS)

vSem.o : vSem.cpp vSem.h
	$(CXX) $(CFLAGS) -c vSem.cpp vSem.h

transdata.o : transdata.cpp transdata.h
	$(CXX) $(CFLAGS) -c transdata.cpp transdata.h

mt.o : mt.cpp mt.h
	$(CXX) $(CFLAGS) -c mt.cpp mt.h
	
clean:
	rm *.o