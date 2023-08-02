#include "kernel/types.h"
#include "user/user.h"

int main()
{
    /*p1写端父进程，读端子进程*/
    /*p2写端子进程，读端父进程*/
    int p1[2], p2[2];//p[0]为读，p[1]为写
    pipe(p1);//父进程写，子进程读的pipe
    pipe(p2);//子进程写，父进程读的pipe

    char buffer[4];//来回传输的字符数组

    //fork创建一次返回两次
    if(fork()==0){//表示为子进程调用
        read(p1[0],buffer,4);//子进程从管道中读取数据
        printf("%d: received %s\n", getpid(), buffer);//输出提示
        write(p2[1],"pong", strlen("pong"));//子进程向管道中写入数据
    }
    else{
        write(p1[1],"ping", strlen("ping"));//父进程向管道中写入数据
        wait(0);//等待子进程退出
        read(p2[0],buffer,4);//父进程从管道中读取数据
        printf("%d: received %s\n", getpid(), buffer);//输出提示
    }
    exit(0);//正常退出
}