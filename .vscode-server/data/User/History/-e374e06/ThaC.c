#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/param.h"

#define MAXN 1024

int main(int argc, char *argv[])
{
    // 如果参数个数小于 2
    if (argc < 2) {
        fprintf(2, "usage: xargs command\n");// 打印参数错误提示
        exit(1);// 异常退出
    }
    
    char * argvs[MAXARG];// 存放子进程 exec 的参数
    int index = 0;// 索引
    for (int i = 1; i < argc; i++)// 略去 xargs ，用来保存命令行参数
        argvs[index++] = argv[i];
    char buf[MAXN] = {"\0"};// 缓冲区存放从管道读出的数据

	//从管道循环读取数据
    while(read(0, buf, MAXN) > 0 ) {
		char temp[MAXN] = {"\0"};// 临时缓冲区 存放追加的参数
        argvs[index] = temp; //xargs命令的参数后面再追加参数

        // 内循环获取追加的参数并创建子进程执行命令
        for(int i = 0; i < strlen(buf); i++) {
            // 为每一行创建一个子进程
            if(buf[i] == '\n') {
                if (fork() == 0)// 创建子线程执行命令
                    exec(argv[1], argvs);
                wait(0);// 等待子线程执行完毕
            } 
            else
                temp[i] = buf[i];// 否则，读取管道的输出作为输入
        }
    }
    // 正常退出
    exit(0);
}
