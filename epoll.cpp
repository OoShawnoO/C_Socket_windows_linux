#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <time.h>
#include <iostream>
#include <sys/epoll.h>

using namespace std;

int main(){
    //创建socket
    int lfd = socket(PF_INET,SOCK_STREAM,0);
    struct sockaddr_in saddr;
    saddr.sin_port = htons(9999);
    saddr.sin_family = AF_INET;
    saddr.sin_addr.s_addr = INADDR_ANY;

    //绑定
    bind(lfd,(struct sockaddr*)&saddr,sizeof(saddr));

    //监听
    listen(lfd,8);

    //调用epoll_create 创建一个epoll实例
    int epollfd = epoll_create1(1024);

    //将监听的文件描述符相关的检测信息添加到epoll实例当中
    struct epoll_event epev;
    epev.events = EPOLLIN;
    epev.data.fd = lfd;

    epoll_ctl(epollfd,EPOLL_CTL_ADD,lfd,&epev);

    struct epoll_event epevs[1024];

    while(1){
        int ret = epoll_wait(epollfd,epevs,1024,-1);
        if(ret == -1){
            perror("epoll_wait");
            exit(-1);
        }
        cout << "ret = " << ret << endl;

        for(int i=0;i<ret;i++){
            if(epevs[i].data.fd == lfd){
                struct sockaddr_in cliaddr;
                socklen_t len = sizeof(cliaddr);
                int cfd = accept(lfd,(struct sockaddr*)&cliaddr,&len);

                epev.events = EPOLLIN;
                epev.data.fd = cfd;
                epoll_ctl(epollfd,EPOLL_CTL_ADD,cfd,&epev);
            }else{
                //有数据到达,需要通信;
                char buf[1024] = {0};
                int len = read(epevs[i].data.fd,buf,sizeof(buf));
                if(len == -1){
                    perror("read");
                    exit(-1);
                }else if(len==0){
                    cout << "client closed.."<<endl;
                    epoll_ctl(epollfd,EPOLL_CTL_DEL,epevs[i].data.fd,NULL);
                    close(epevs[i].data.fd);
                }else if(len >0){
                    cout << "client data:"<< buf <<endl;
                    write(i,buf,sizeof(buf));
                }
            }
        }

    }
    
    return 0;
}
