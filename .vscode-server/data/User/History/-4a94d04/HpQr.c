// 首先引入头文件
#include "kernel/types.h"  //声明类型的头文件
#include "user/user.h"  //声明系统调用函数和ulib.c中函数的头文件


//main函数
//argc是命令行总参数个数
//argv是命令行输入的argc个参数，第一个参数为函数的全名
int main(int argc, char *argv[]){
	/*错误处理 必须为函数名+sleep时长 共两个参数*/
	if(argc != 2){
		/*打印错误信息*/
		/*0 表示标准输入，1 表示标准输出，2 表示标准错误*/
		fprintf(2, "must 1 argument for sleep\n");
		exit(1);//错误退出
	}

	/*把字符串型参数转换为整型*/
	int sleepNum = atoi(argv[1]);
	printf("(nothing happens for a little while)\n");
	sleep(sleepNum);//暂停相应时长
	exit(0);//正常退出
}

// stdin：标准输入流，通常用于从用户获取输入。
// stdout：标准输出流，用于常规的标准输出。
// stderr：标准错误流，用于输出错误信息和警告信息
