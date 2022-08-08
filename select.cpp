#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <time.h>
#include <sys/select.h>
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

    // 创建一个fd_set的集合,存放的是需要检测的文件的文件描述符
    fd_set rdset,tmp;
    FD_ZERO(&rdset);
    FD_SET(lfd,&rdset);
    int maxfd = lfd;

    while(1){
        
        tmp = rdset;

        //调用select系统函数 让内核检测哪些文件描述符有数据
        int ret = select(maxfd+1,&tmp,NULL,NULL,NULL);
        if(ret == -1){
            perror("select");
            exit(-1);
        }else if(ret == 0){
            continue; // 设置时间后 时间结束没有文件描述符发生改变
        }else{
            //说明有文件描述符发生了变化
            if(FD_ISSET(lfd,&tmp)){
                //表示有新的客户端连接进来了
                struct sockaddr_in clientaddr;
                socklen_t len = sizeof(clientaddr);
                int cfd = accept(lfd,(struct sockaddr*)&clientaddr,(struct socklen_t*)&len);
                //将新的文件描述符加入到fd_set中
                FD_SET(cfd,&tmp); 
                //更新最大的文件描述符
                maxfd = maxfd > cfd ? maxfd : cfd;
            }

            for(int i = lfd + 1; i<=maxfd;i++){
                if(FD_ISSET(i,&tmp)){
                    //说明客户端发来了数据
                    char buf[1024] = {0};
                    int len = read(i,buf,sizeof(buf));
                    if(len == -1){
                        perror("read");
                        exit(-1);
                    }else if(len==0){
                        cout << "client closed.."<<endl;
                        close(i);
                        FD_CLR(i,&rdset);
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