TARGET = ../../../output/libcan.so
OBJECTS = can_data.o MF_CRC8.o crc16.o
CFLAGS = -Wall -fPIC -I../../../include -I../../../third/can/include -I../../../third/canutils-4.0.6/include
LDFLAGS = -shared -Wl,-rpath,./ -L../../../third/can/lib -lsocketcan

CXX = arm-linux-g++

$(TARGET) : $(OBJECTS)
	$(CXX) -o $(TARGET) $(OBJECTS) $(LDFLAGS)
	
can_data.o : can_data.cpp can_data.h
	$(CXX) $(CFLAGS) -c can_data.cpp can_data.h
	
crc16.o : crc16.cpp crc16.h
	$(CXX) $(CFLAGS) -c crc16.cpp crc16.h

MF_CRC8.o : MF_CRC8.cpp MF_CRC8.h
	$(CXX) $(CFLAGS) -c MF_CRC8.cpp MF_CRC8.h

clean:
	rm *.o