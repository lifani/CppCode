/*
 * sbus.h
 *
 *  Created on: 2013年9月29日
 *      Author: vincent.wuyyuwei
 */

#ifndef SBUS_H_
#define SBUS_H_

#define UART_PORT1		1
#define UART_PORT2		2
#define UART_PORT3		3

#define SIZEOFSBUSPACKET 25
#define NUMOFCHANNEL 16

#define TAKE_BIT_HI( src, nBit ) ( (src) >> ( 8 - (nBit) ) )
#define TAKE_BIT_LO( src, nBit ) ( (src) & ( ~(0xFF << (nBit) )) )

int uart_init(char port,int baudrate,char data_bits,char parity_bits,char stop_bits);
int uart_config(int fd, int nSpeed, int nBits, char nEvent, int nStop);
int uart_close(void);
int uart_read(unsigned char *buf,int len);
int uart_write(unsigned char *buf,int len);
bool seekHead();
int readn( unsigned char *buf, int len );

extern int uart_fd;

#endif /* SBUS_H_ */
