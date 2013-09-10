#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <iostream>

//#include "../../../include/can_interface.h"

using namespace std;

/*typedef struct _MSG
{
	char flg;
	char data[10];
} MSG;*/

//void client();

//void server();

typedef struct _can_head
{
	unsigned char head[4];
	unsigned short cmd_code;
	unsigned short data_len;
} can_head;

int main(int argc, char* argv[])
{
	can_head head = {{0x55, 0xaa, 0x55, 0xaa}, 0x1005, 8};
	
	for (int i = 0; i < sizeof(can_head); ++i)
	{
		printf("0x%02x ", *((unsigned char*)&head + i));
	}
	
	cout << endl;
	
	return 0;
}
/*
void client()
{
	CMt mt;
	
	int key = mt.shm_tm_init(sizeof(MSG));
	
	MSG msg;
	while (1)
	{
		int len = mt.shm_tm_recv(key, (char*)&msg, sizeof(MSG));
		if (sizeof(MSG) == len)
		{
			if (msg.flg == '1')
			{
				cout << "client recv data end." << endl;
				break;
			}
			else
			{
				cout << msg.data << endl;
			}
		}
		else if (0 != len)
		{
			cout << "client recv data error " << endl;
			break;
		}
		else
		{
			cout << "recv 0" << endl;
			sleep(2);
		}
		
		
	}
	
	mt.shm_tm_destory(key);

}

void server()
{
	CMt mt;
	
	int key = mt.shm_tm_init(sizeof(MSG));
	
	string strIn;
	
	MSG msg;
	int i = 0;
	while (i <= 10)
	{
		if (i == 10)
		{
			msg.flg = '1';
		}
		else
		{
			msg.flg = '0';
		}
		
		sprintf(msg.data, "%d123456789\0", i % 10);
		
		cout << msg.data << endl;
		
		int len = sizeof(MSG) < strIn.size() ? sizeof(MSG) : strIn.size(); 
		
		memcpy((char*)&msg, strIn.c_str(), len);
		
		int nRet = mt.shm_tm_send(key, (char*)&msg, sizeof(MSG));
		if (nRet == -1)
		{
			cout << "server send msg error" << endl;
			break;
		}
		
		i++;
		sleep(2);
	}
	
	cout << "send end." << endl;
	
	
	mt.shm_tm_destory(key);
}*/
