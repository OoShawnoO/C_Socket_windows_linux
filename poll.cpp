#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <time.h>
#include <poll.h>
#include <iostream>

using namespace std;

int main(){
    int lfd = socket(PF_INET,SOCK_STREAM,0);
    struct sockaddr_in saddr;
    saddr.sin_port = htons(9999);
    saddr.sin_family = AF_INET;
    saddr.sin_addr.s_addr = INADDR_ANY;

    //绑定
    bind(lfd,(struct sockaddr*)&saddr,sizeof(saddr));

    //监听
    listen(lfd,8);

    //初始化检测的文件描述符数组
    struct pollfd fds[1024];
    for(int i=0;i<1024;i++){
        fds[i].fd = -1;
        fds[i].events = POLLIN;
    }
    fds[0].fd = lfd;

    int nfds = 0;

    while(1){
        
        //调用poll系统函数
        int ret = poll(fds,nfds+1,-1);
        if(ret == -1){
            perror("poll");
            exit(-1);
        }else if(ret == 0){
            continue; // 设置时间后 时间结束没有文件描述符发生改变
        }else{
            //说明有文件描述符发生了变化
            if(fds[0].revents & POLLIN){
                //表示有新的客户端连接进来了
                struct sockaddr_in clientaddr;
                socklen_t len = sizeof(clientaddr);
                int cfd = accept(lfd,(struct sockaddr*)&clientaddr,&len);
                
                //将新的文件描述符加入到集合中
                for(int i=0;i<1024;i++){
                    if(fds[i].fd == -1){
                        fds[i].fd = cfd;
                        fds[i].events = POLLIN;
                        break;
                    }
                }

                //更新最大文件描述符
                nfds = nfds > cfd ? nfds : cfd;

            }

            for(int i = lfd + 1; i<=nfds;i++){
                if(fds[i].revents & POLLIN){
                    //说明客户端发来了数据
                    char buf[1024] = {0};
                    int len = read(fds[i].fd,buf,sizeof(buf));
                    if(len == -1){
                        perror("read");
                        exit(-1);
                    }else if(len==0){
                        cout << "client closed.."<<endl;
                        close(i);
                        fds[i].fd = -1;
                    }else if(len >0){
                        cout << "client data:"<< buf <<endl;
                        write(i,buf,sizeof(buf));
                    }
                }
            }
        }
    }
    close(lfd);
}