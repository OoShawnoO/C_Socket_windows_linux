#include <iostream>
#include <winsock2.h>


#pragma comment(lib, "ws2_32.lib")//ע�����ӿ��ļ�

using namespace std;

#define IP "0.0.0.0"
#define PORT 9999

int main(int argc, char* argv[])
{
	//����DLL
	WSADATA wsaData;
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
	{
		cout << "��̬�������ʧ��" << endl;
		WSACleanup();
		exit(-1);
	}
	cout << "��̬�����ӳɹ�" << endl;
 
	unsigned long ul=1;
	SOCKET sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	// ioctlsocket(sock,FIONBIO,(unsigned long *)&ul);

	SOCKADDR_IN sockAddr;
	memset(&sockAddr, 0, sizeof(sockAddr));
	sockAddr.sin_family = AF_INET;                            
	sockAddr.sin_addr.S_un.S_addr = inet_addr(IP);
	sockAddr.sin_port = htons(PORT);                      
	if (connect(sock, (SOCKADDR*)&sockAddr, sizeof(SOCKADDR)) == SOCKET_ERROR)
	{
		cout << "����ʧ��" << WSAGetLastError() <<  endl;
		exit(-1);
	}
	cout << "���ӳɹ�..." << endl;
    char sendBuf[1024],receive[1024];
	memset(sendBuf, 0, 1024);
	memset(receive, 0, 1024);
	
	while(1){
		cout << "����������:";
		cin >> sendBuf;
		send(sock, sendBuf, sizeof(sendBuf), 0);//��������

		int ret = recv(sock,receive,sizeof(receive),0);
		if(ret>0){
			cout << "server :" << receive <<endl;
		}
		if(strcmp(sendBuf,"end")==0) break;
		memset(sendBuf, 0, 1024);
		memset(receive, 0, 1024);
	}
	

    closesocket(sock);
    WSACleanup();//�����̬������
    while (1);
    return 0;
}


