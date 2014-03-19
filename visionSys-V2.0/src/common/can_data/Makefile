TARGET = ../../../output/libcan.so
OBJECTS = CHF.o queueCtrl.o canPacket.o comPacket.o abstractCanCtrl.o newProtocolCanCtrl.o 388CanCtrl.o 108CanCtrl.o sndCanCtrl.o 090CanCtrl.o crc16.o MF_CRC8.o
CFLAGS = -g -rdynamic -Wall -fPIC -I../../../include -I../../../third/can/include  -I../../../third/canutils-4.0.6/include
LDFLAGS = -shared -Wl,-rpath,./ -L../../../output -L../../../third/can/lib -lsocketcan -lsbus

CXX = arm-linux-g++

$(TARGET) : $(OBJECTS)
	$(CXX) -o $(TARGET) $(OBJECTS) $(LDFLAGS)
	
090CanCtrl.o : 090CanCtrl.cpp 090CanCtrl.h
	$(CXX) $(CFLAGS) -c 090CanCtrl.cpp 090CanCtrl.h
	
sndCanCtrl.o : sndCanCtrl.cpp sndCanCtrl.h
	$(CXX) $(CFLAGS) -c sndCanCtrl.cpp sndCanCtrl.h
	
108CanCtrl.o : 108CanCtrl.cpp 108CanCtrl.h
	$(CXX) $(CFLAGS) -c 108CanCtrl.cpp 108CanCtrl.h
	
388CanCtrl.o : 388CanCtrl.cpp 388CanCtrl.h
	$(CXX) $(CFLAGS) -c 388CanCtrl.cpp 388CanCtrl.h
	
abstractCanCtrl.o : abstractCanCtrl.cpp abstractCanCtrl.h
	$(CXX) $(CFLAGS) -c abstractCanCtrl.cpp abstractCanCtrl.h
	
newProtocolCanCtrl.o : newProtocolCanCtrl.cpp newProtocolCanCtrl.h
	$(CXX) $(CFLAGS) -c newProtocolCanCtrl.cpp newProtocolCanCtrl.h
	
canPacket.o : canPacket.cpp canPacket.h
	$(CXX) $(CFLAGS) -c canPacket.cpp canPacket.h
	
comPacket.o : comPacket.cpp comPacket.h
	$(CXX) $(CFLAGS) -c comPacket.cpp comPacket.h
	
CHF.o : CHF.cpp CHF.h
	$(CXX) $(CFLAGS) -c CHF.cpp CHF.h

queueCtrl.o : queueCtrl.cpp queueCtrl.h
	$(CXX) $(CFLAGS) -c queueCtrl.cpp queueCtrl.h

crc16.o : crc16.cpp crc16.h
	$(CXX) $(CFLAGS) -c crc16.cpp crc16.h

MF_CRC8.o : MF_CRC8.cpp MF_CRC8.h
	$(CXX) $(CFLAGS) -c MF_CRC8.cpp MF_CRC8.h

clean:
	rm *.o *.gch