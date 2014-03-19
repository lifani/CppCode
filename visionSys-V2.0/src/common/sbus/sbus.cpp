/*
 * sbuc.c
 *
 *  Created on: 2013年9月29日
 *      Author: vincent.wuyyuwei
 */

#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <errno.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <termios.h>
#include <stdlib.h>
#include <linux/serial.h>
#include <sys/ioctl.h>
#include <sbus.h>

#define __DEBUG__ 1

#if __DEBUG__
#define PRINTFILELINE printf( "%s %d\n", __FILE__, __LINE__ );
#else
#define PRINTFILELINE ;
#endif

static int st_baud[]=
{
		B4800,
		B9600,
		B19200,
		B38400,
		B57600,
		B115200,
		B1000000,
		B1152000,
		B3000000,
};
static int baudrate[]=
{
		4800,
		9600,
		19200,
		38400,
		57600,
		115200,
		1000000,
		1152000,
		3000000,
};

int uart_fd = -1;

int uart_config(int fd, int nSpeed, int nBits, char nEvent, int nStop)
{
	int i,j;
	struct termios newtio, oldtio;
	/*保存测试现有串口参数设置，在这里如果串口号等出错，会有相关的出错信息*/
	if (tcgetattr(fd, &oldtio) != 0) {
		perror("SetupSerial 1");
		return -1;
	}
	bzero(&newtio, sizeof(newtio));
	/*步骤一，设置字符大小*/
	newtio.c_cflag |= CLOCAL | CREAD;
	newtio.c_cflag &= ~CSIZE;
	/*设置停止位*/
	switch (nBits)
	{
	case 7:
		newtio.c_cflag |= CS7;
		break;
	case 8:
		newtio.c_cflag |= CS8;
		break;
	}
	/*设置奇偶校验位*/
	switch (nEvent)
	{
	case 'O': //奇数
	case 'o': //奇数
		newtio.c_cflag |= PARENB;
		newtio.c_cflag |= PARODD;
//		newtio.c_iflag |= (INPCK | ISTRIP);
		break;
	case 'E': //偶数
	case 'e': //偶数
//		newtio.c_iflag |= (INPCK | ISTRIP);
		newtio.c_cflag |= PARENB;
		newtio.c_cflag &= ~PARODD;
		break;
	case 'N':  //无奇偶校验位
	case 'n':  //无奇偶校验位
		newtio.c_cflag &= ~PARENB;
		break;
	}
	/*设置波特率*/
	j = sizeof(baudrate)/4;
	for(i=0;i<j;i++)
	{
		if(baudrate[i] == nSpeed)
		{
			/* set standard baudrate */
			cfsetispeed(&newtio, st_baud[i]);
			cfsetospeed(&newtio, st_baud[i]);
			break;
		}
	}
	/* set non standard baudrate */
	if( (i == j) && (baudrate[i-1]!=nSpeed) )
	{
		/* set base baudrate */
		/* set B4800 here,indicate vl300 omap4 kernel UART driver to set baud 100K */
		cfsetispeed(&newtio, B4800);
		cfsetospeed(&newtio, B4800);
	}
	/*设置停止位*/
    if( nStop == 1 )
       newtio.c_cflag &=  ~CSTOPB;
    else if ( nStop == 2 )
       newtio.c_cflag |=  CSTOPB;
    /*设置等待时间和最小接收字符*/
    newtio.c_cc[VTIME]  = 1;
    newtio.c_cc[VMIN] = 1;
    /* 使用原始数据模式 */
    newtio.c_lflag  &= ~(ICANON | ECHO | ECHOE | ISIG);  /*Input*/
    newtio.c_oflag  &= ~OPOST;   /*Output*/

    /*处理未接收字符*/
    tcflush(fd,TCIFLUSH);
    /*激活新配置*/
    if((tcsetattr(fd,TCSANOW,&newtio))!=0)
    {
       perror("com set error");
       return -1;
    }

#if 0
	/* set non standard baudrate */
	if( (i == j) && (baudrate[i-1]!=nSpeed) )
	{
		struct serial_struct ss;
	    if((ioctl(fd,TIOCGSERIAL,&ss))<0)
	    {
	    	perror("BAUD: error to get the serial_struct info\n");
	        return -1;
	    }
	    ss.flags = (ss.flags & ~ASYNC_SPD_MASK) | ASYNC_SPD_CUST|ASYNC_LOW_LATENCY;
	    ss.custom_divisor = ss.baud_base / nSpeed;
	    printf("ss.custom_divisor = %d,ss.baud_base = %d,nSpeed = %d\n",ss.custom_divisor,ss.baud_base,nSpeed);

	    if((ioctl(fd,TIOCSSERIAL,&ss))<0)
	    {
	    	/*ioctl(fd,TIOCSSERIAL,&ss) return <0,tips "Invaild argument",but can set baud about 100k,why???*/
	    	perror("BAUD: error to set serial_struct\n");
	    	/* return -1; */
	    }

	}
#endif

    printf("set UART port paramster done!\n");
    return 0;
}


int uart_init(char port,int baudrate,char data_bits,char parity_bits,char stop_bits)
{
	char *arm_path[]={"/dev/ttyO0","/dev/ttyO1","/dev/ttyO2"};
	/* 打开串口 */
//	uart_fd = open( "/dev/ttyO1", O_RDWR|O_NOCTTY|O_NDELAY);  		//非阻塞读写
	uart_fd = open( arm_path[port-1], O_RDWR|O_NOCTTY);				//阻塞读写
	if(uart_fd<0)
	{
		 printf("open UART ERROR ???\n");
		 return -1;
	}
	/* 配置串口端口属性 */
	if(uart_config(uart_fd,baudrate,data_bits,parity_bits,stop_bits)<0)
	{
		printf("UART config ERROR ???\n");
		return -1;
	}

	return 0;
}

int uart_close(void)
{
	close(uart_fd);
	uart_fd = -1;
	return 0;
}

int uart_read(unsigned char *buf,int len)
{
	return read(uart_fd,buf,len);
}

int uart_write(unsigned char *buf,int len)
{
	return write(uart_fd,buf,len);
}

/*
提供一个读取n个字节的接口
*/
int readn( unsigned char *buf, int len )
{
	int nTemp = uart_read(buf, len);
	return nTemp;
	/*
	int nReceived = 0;
	int nRet = 0;
	for ( ; nReceived < len; )
	{
		nRet = uart_read( buf+nReceived, len - nReceived );
	//	printf( "nRet:%d  %s %d\n", nRet,__FILE__, __LINE__ );
		if ( nRet > 0 )
		{
			printf( "nRet:%d  %s %d\n", nRet,__FILE__, __LINE__ );
			nReceived += nRet;
		}
		else
		{
			printf( "nRet:%d  %s %d\n", nRet,__FILE__, __LINE__ );
			return -1;
		}		
	}
	printf( "nRet:%d  %s %d\n", nRet,__FILE__, __LINE__ );
	return nReceived;
	*/
}

void printHex( unsigned char *pBuf, int nLen )
{
#if __DEBUG__
	for ( int i = 0; i < nLen; ++i )
	{
		printf( "%02X ", pBuf[i] );
	}
	printf("\n");
#endif	
}

bool seekHead()
{
	unsigned char buffer[250] = {0};
	//先读取并丢弃一个包，因为第一个偶尔会不对
	int nRet = readn( buffer, 25 );
	if ( nRet <= 0 )
	{
		return false;
	}
	
	nRet = readn( buffer, 25 );
	if ( nRet <= 0 )
	{
		return false;
	}
	printHex( buffer, 25 );
	bool bFound = false;
	int nLen = 25;
	for ( int i = 0; i < nLen; ++i )
	{
		if ( 0x0f == buffer[i] )
		{
			PRINTFILELINE
			if ( nLen <= i + 25 )
			{
				PRINTFILELINE
				nRet = readn( buffer+nLen, 25 );//再读一个包，寻找下一个包头，如果找到了，则认为当前这个就是包头，否则，就有可能是内容;
				printHex( buffer+nLen, 25 );
				nLen += 25;
				if ( nRet <=0 )
				{
					PRINTFILELINE
					break;
				}				
				nLen += nRet;
			}
			//00是尾巴
			if ( 0x0f == buffer[i+25] && 0x00 == buffer[i+24] )
			{
				PRINTFILELINE
				int nLeftContent = ( nLen - i )%25;//剩余的内容，不足整包的部分
				if ( nLeftContent > 0 )
				{
					nRet = readn( buffer+nLen, nLeftContent );//读取不足整包的部分，对齐
					if ( nRet > 0 )
					{
						bFound = true;
					}
				}
				else
				{
					bFound = true;
				}
						
				break;
			}
		}				
	}
	PRINTFILELINE
	return bFound;
}
