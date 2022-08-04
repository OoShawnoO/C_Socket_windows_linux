#include <iostream>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>
#define PASSWORD "abcdefg"

using namespace std;


int main(){
    
    // 创建用于监听的套接字
    int sockfd = socket(AF_INET,SOCK_STREAM,0);

    //绑定
    struct sockaddr_in saddr;
    saddr.sin_family = AF_INET;
    inet_pton(AF_INET,"0.0.0.0",&saddr.sin_addr.s_addr);
    saddr.sin_port = htons(9999);
    bind(sockfd,(struct sockaddr*)&saddr,sizeof(saddr));

    //监听
    listen(sockfd,32);

    //接收客户端连接
    struct sockaddr_in clientaddr;
    socklen_t length = sizeof(clientaddr);
    int clientfd = accept(sockfd,(struct sockaddr*)&clientaddr,&length);

    if(clientfd == -1){
        perror("accept : ");
        exit(-1);
    }

    //输出客户端信息
    char clientIP[16];
    inet_ntop(AF_INET,&clientaddr.sin_addr.s_addr,clientIP,sizeof(clientIP));
    unsigned short clientPort = ntohs(clientaddr.sin_port);
    cout << "client ip is :"<<clientIP<<", port is :"<<clientPort<<endl;
    //获取客户端的数据

    //给客户端发送数据
    int pipefd[2];
    int rets = pipe(pipefd);
    if(rets==0){
        cout << "create pipe success!"<<endl;
    }
    else{
        cout << "create pipe failed!"<<endl;
    }

    int pid = fork();
    
    if(pid == 0){
        char reciveBuf[1024] = {0};
        char SonreadBuf[1024] = {0};
        char SonwriteBuf[1024] = {0};
        while(1){
            recv(clientfd,reciveBuf,sizeof(reciveBuf),0);
            strncpy(SonreadBuf,reciveBuf,7);
            if(strcmp(SonreadBuf,PASSWORD)==0){
                strncpy(SonwriteBuf,reciveBuf+7,1024-7);
                cout << "SonwriteBuf : " << SonwriteBuf <<endl;
                int size = write(pipefd[1],SonwriteBuf,sizeof(SonwriteBuf));
                cout << size <<endl;
            }else{
                cout<< "SonreadBuf:" << SonreadBuf <<"  wrong." <<endl;
            }
            memset(SonreadBuf,0,sizeof(SonreadBuf));
            memset(SonwriteBuf,0,sizeof(SonwriteBuf));
            memset(reciveBuf,0,sizeof(reciveBuf));
        }

    }else if(pid>0){
        char sendBuf[1024] = {0};
        char ParentreadBuf[1024] = {0};
        char ParentwriteBuf[1024] = {0};
        while(1){
            int size = read(pipefd[0],ParentreadBuf,sizeof(ParentreadBuf));
            cout <<size<<endl;
            cout << "ParentreadBuf:" << ParentreadBuf << endl;
            if(strcmp(ParentreadBuf,"test")==0){
                int ppid = fork();
                if(ppid == 0){
                    int fd = open("./test.txt",O_WRONLY | O_CREAT,0664);
                    dup2(fd,STDOUT_FILENO);
                    close(fd);
                    execlp("python","python","./test.py",NULL);
                }else if(ppid > 0){
                    strcpy(sendBuf,"ok!");
                }
            }
            else if(strcmp(ParentreadBuf,"break")==0){
                break;
            }else{
                strcpy(sendBuf,"failed!");
            }
            send(clientfd,sendBuf,sizeof(sendBuf),0);
            memset(ParentreadBuf,0,sizeof(ParentreadBuf));
            memset(sendBuf,0,sizeof(sendBuf));
        }
    }
    int wrong = 0;
    int waitgid = 0;

    kill(pid,SIGKILL);

    int waitgid = wait(&wrong);

    cout << waitgid << "回收完成." <<endl;

    close(pipefd[0]);
    close(pipefd[1]);
    close(clientfd);
    close(sockfd);

    exit(0);
    return 0;
}
