#include "kernel/types.h"
#include "user/user.h"

/* n -> pd[n]的映射 */
void mapping(int n, int pd[])
{
  close(n);// 关闭文件描述符 n
  dup(pd[n]);// 将管道的 读或写 端口复制到描述符 n 上
  // 关闭管道中的描述符
//   close(pd[0]);
//   close(pd[1]);
}

/*求素数*/
void primes()
{
  int previous, next;// 定义变量获取管道中的数
  int fd[2];// 定义管道描述符数组

  if (read(0, &previous, sizeof(int))){// 从管道读取数据
    printf("prime %d\n", previous);// 第一个一定是素数，直接打印
    pipe(fd);// 创建管道
    if (fork() == 0){// 创建子进程
      mapping(1, fd);// 子进程将管道的写端口映射到描述符 1 上
      while (read(0, &next, sizeof(int)))// 循环读取管道中的数据
        if (next % previous != 0)// 如果该数不是管道中第一个数的倍数
          write(1, &next, sizeof(int));// 写入管道
    }
    else{
      wait(0);// 等待子进程把数据全部写入管道
      mapping(0, fd);// 父进程将管道的读端口映射到描述符 0 上
      primes();// 递归执行此过程
    }  
  }  
}

int main(int argc, char *argv[])
{
  int fd[2];// 定义描述符
  pipe(fd);// 创建管道
  
  if (fork() == 0){// 创建进程
    mapping(1, fd);// 子进程将管道的写端口映射到描述符 1 上
    for (int i = 2; i <= 35; i++)// 循环获取 2 至 35
      write(1, &i, sizeof(int));// 将其写入管道
  }
  else{
    wait(0);// 等待子进程把数据全部写入管道
    mapping(0, fd);// 父进程将管道的读端口映射到描述符 0 上
    primes();// 调用 primes() 函数求素数
  }
  // 正常退出
  exit(0);
}
