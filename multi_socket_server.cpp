#include <iostream>
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/stat.h>
#include <signal.h>
#include <wait.h>

using namespace std;

void recycleChild(int arg){
    while(1){
        int ret = waitpid(-1,NULL,WNOHANG);
        if(ret == -1){
            break;
        }else if(ret==0){
            break;
        }else if(ret>0){
            cout << "子进程" << ret << "被回收了" <<endl;
        }
    }
}

int main(){
    struct sigaction act;
    act.sa_flags = 0;
    sigemptyset(&act.sa_mask);
    act.sa_handler = recycleChild;
    //注册信号捕捉
    sigaction(SIGCHLD,&act,NULL);

    //创建socket
    int sockfd = socket(AF_INET,SOCK_STREAM,0);
    if(sockfd == -1){
        perror("socket");
        exit(-1);
    }

    //绑定
    struct sockaddr_in saddr;
    saddr.sin_family = AF_INET;
    saddr.sin_port = htons(9999);
    saddr.sin_addr.s_addr = INADDR_ANY;

    int ret = bind(sockfd,(struct sockaddr*)&saddr,sizeof(saddr));
    if(ret == -1){
        perror("bind");
        exit(-1);
    }

    ret = listen(sockfd,128);
    if(ret == -1){
        perror("listen");
        exit(-1);
    }

    while(1){
        struct sockaddr_in clientaddr;
        int len = sizeof(clientaddr);
        int clientfd = accept(sockfd,(struct sockaddr*)&clientaddr,(socklen_t*)&len);

        if(clientfd == -1){
            if(errno==EINTR){
                continue;
            }
            perror("accept");
            exit(-1);
        }

        //每一个连接进入，创建一个子进程跟客户端通信;
        pid_t pid = fork();
        if(pid == 0){
            char clientIP[16];
            inet_ntop(AF_INET,&clientaddr.sin_addr.s_addr,clientIP,sizeof(clientIP));
            unsigned short clientPort = ntohs(clientaddr.sin_port);
            cout << "client IP is:" << clientIP << ",client Port is:" << clientPort <<endl;
            char recvBuf[1024] = {0};
            while(1){
                int len = read(clientfd,recvBuf,sizeof(recvBuf));

                if(len == -1){
                    perror("read");
                    exit(-1);
                }
                else if(len>0){
                    cout << "recv client data:"<< recvBuf <<endl;
                }else{
                    cout << "client closed.."<<endl;
                }
                write(clientfd,recvBuf,sizeof(recvBuf));
            }
            close(clientfd);
            exit(0);
            
        }else if(pid > 0){

        }

    }
}