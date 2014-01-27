TARGET = ../../../output/libmt.so
OBJECTS = mt.o shmTrans.o vSem.o msgTrans.o
CFLAGS = -g -Wall -fPIC -I../../../include
LDFLAGS = -shared

CXX = arm-linux-g++

$(TARGET) : $(OBJECTS)
	$(CXX) -o $(TARGET) $(OBJECTS) $(LDFLAGS)

vSem.o : vSem.cpp vSem.h
	$(CXX) $(CFLAGS) -c vSem.cpp vSem.h

shmTrans.o : shmTrans.cpp shmTrans.h
	$(CXX) $(CFLAGS) -c shmTrans.cpp shmTrans.h
	
msgTrans.o : msgTrans.cpp msgTrans.h
	$(CXX) $(CFLAGS) -c msgTrans.cpp msgTrans.h

mt.o : mt.cpp
	$(CXX) $(CFLAGS) -c mt.cpp
	
clean:
	rm *.o *.gch