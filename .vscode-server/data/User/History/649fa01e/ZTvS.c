#include <kernel/types.h>
#include <user/user.h>

int main()
{
    /*p1写端父进程，读端子进程*/
    /*p2写端子进程，读端父进程*/
    int p1[2], p2[2];//p[0]为读，p[1]为写
    pipe(p1);//父进程写，子进程读的pipe
    pipe(p2);//子进程写，父进程读的pipe
    pid_t p_id;
    p_id = fork();//一次创建两次返回
    if(p_id<0)
        printf("error in fork!");
    else if(p_id==0){//表示为子进程调用
        close(p1[1]);
        close(p2[0]);
    }

}