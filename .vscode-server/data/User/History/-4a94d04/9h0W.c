// 首先引入头文件
#include "kernel/types.h"  //声明类型的头文件
#include "user/user.h"  //声明系统调用函数和ulib.c中函数的头文件


//main函数
//argc是命令行总参数个数
//argv是命令行输入的argc个参数，第一个参数为函数的全名
int main(int argc, char *argv[]){
	/*错误处理 必须为函数名+sleep时长 共两个参数*/
	if(argc != 2){
		write(2, "Usage: sleep time\n", strlen("Usage: sleep time\n"));
		exit(1);
	}

	/*把字符串型参数转换为整型*/
	int sleepNum = atoi(argv[1]);
	printf("(nothing happens for a little while)\n");
	sleep(sleepNum);
	exit(0);
}