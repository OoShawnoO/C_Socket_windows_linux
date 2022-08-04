#include <iostream>
#include <winsock2.h>


#pragma comment(lib, "ws2_32.lib")//注意链接库文件

using namespace std;

#define IP "0.0.0.0"
#define PORT 9999

int main(int argc, char* argv[])
{
	//连接DLL
	WSADATA wsaData;
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
	{
		cout << "动态库库连接失败" << endl;
		WSACleanup();
		exit(-1);
	}
	cout << "动态库连接成功" << endl;
 
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
		cout << "连接失败" << WSAGetLastError() <<  endl;
		exit(-1);
	}
	cout << "连接成功..." << endl;
    char sendBuf[1024],receive[1024];
	memset(sendBuf, 0, 1024);
	memset(receive, 0, 1024);
	
	while(1){
		cout << "请输入命令:";
		cin >> sendBuf;
		send(sock, sendBuf, sizeof(sendBuf), 0);//发送数据

		int ret = recv(sock,receive,sizeof(receive),0);
		if(ret>0){
			cout << "server :" << receive <<endl;
		}
		if(strcmp(sendBuf,"end")==0) break;
		memset(sendBuf, 0, 1024);
		memset(receive, 0, 1024);
	}
	

    closesocket(sock);
    WSACleanup();//清除动态库连接
    while (1);
    return 0;
}


