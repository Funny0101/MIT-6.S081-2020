// 首先引入头文件
#include "kernel/types.h"  //声明类型的头文件
#include "user/user.h"  //声明系统调用函数和ulib.c中函数的头文件


//main函数
//argc是命令行总参数个数
//argv是命令行输入的argc个参数，第一个参数为函数的全名
int main(int argc, char *argv[]){
	if(argc != 2){
		fprintf(2, "must 1 argument for sleep\n");
		exit(1);
	}
	int sleepNum = atoi(argv[1]);
	printf("(nothing happens for a little while)\n");
	sleep(sleepNum);
	exit(0);
}