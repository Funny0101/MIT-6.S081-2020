// #include "kernel/types.h"
// #include "user/user.h"
// #include <unistd.h>

// int main()
// {
//     /*p1写端父进程，读端子进程*/
//     /*p2写端子进程，读端父进程*/
//     int p1[2], p2[2];//p[0]为读，p[1]为写
//     pipe(p1);//父进程写，子进程读的pipe
//     pipe(p2);//子进程写，父进程读的pipe

//     char buffer[] = {'X'};//来回传输的字符数组：一个字节
//     int length = strlen(buffer);

//     pid_t p_id;
//     p_id = fork();//一次创建两次返回
//     if(p_id<0){//fork出现错误（内存不足或进程并发数达到上限）
//         printf("error in fork!");
//         exit(1);//错误退出
//     } 
//     else if(p_id==0){//表示为子进程调用
//         close(p1[1]);//关闭父进程的写
//         close(p2[0]);//关闭父进程的读
//         read(p1[0],buffer,length);//子进程从管道中读取数据
//         printf("%d: received ping", getpid());//输出提示
//         write(p2[1],buffer,length);//子进程向管道中写入数据
//         exit(0);//正常退出
//     }
//     else{
//         close(p1[1]);//关闭子进程的写
//         close(p2[0]);//关闭子进程的读
//         read(p2[0],buffer,length);//父进程从管道中读取数据
//         printf("%d: received pong", getpid());//输出提示
//         write(p1[1],buffer,length);//父进程向管道中写入数据
//         wait(0);//等待子进程退出
//         exit(0);//正常退出
//     }
//     return 0;
// }
