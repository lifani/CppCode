TARGET = ../../../output/libmt.so
OBJECTS = mt.o shmtrans.o vSem.o msgtrans.o
CFLAGS = -g -Wall -fPIC -I../../../include
LDFLAGS = -shared

CXX = g++

$(TARGET) : $(OBJECTS)
	$(CXX) -o $(TARGET) $(OBJECTS) $(LDFLAGS)

vSem.o : vSem.cpp vSem.h
	$(CXX) $(CFLAGS) -c vSem.cpp vSem.h

shmtrans.o : shmtrans.cpp shmtrans.h
	$(CXX) $(CFLAGS) -c shmtrans.cpp shmtrans.h
	
msgtrans.o : msgtrans.cpp msgtrans.h
	$(CXX) $(CFLAGS) -c msgtrans.cpp msgtrans.h
mt.o : mt.cpp
	$(CXX) $(CFLAGS) -c mt.cpp
	
clean:
	rm *.o