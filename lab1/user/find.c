#include "kernel/types.h"
#include "user/user.h"
#include "kernel/stat.h"
#include "kernel/fs.h"

// find 函数
void find(char *dir, char *file)
{   
    char buf[512], *p;//文件名缓冲区 和 指针
    int fd;//文件描述符 fd
    struct dirent de;//与文件相关的结构体
    struct stat st;

    // open() 函数打开路径，返回一个文件描述符，如果错误返回 -1
    if ((fd = open(dir, 0)) < 0)
    {
        fprintf(2, "find: cannot open %s\n", dir);// 报错，提示无法打开此路径
        return;
    }

    // int fstat(int fd, struct stat *);
    // st用于接收一个目录节点的所有信息
    // 如果出错，则返回 -1
    if (fstat(fd, &st) < 0)
    {
        fprintf(2, "find: cannot stat %s\n", dir);// 出错则报错
        close(fd);// 关闭文件描述符 fd
        return;
    }

    /*错误处理*/
    if (st.type != T_DIR) //如果不是目录类型
    {
        fprintf(2, "error: %s is not a directory in find\n", dir);
        close(fd);// 关闭文件描述符 fd
        return;
    }
    if(strlen(dir) + 1 + DIRSIZ + 1 > sizeof(buf))// 文件路径＋/＋文件名的最大值＋尾零
    {
        fprintf(2, "error: directory too long in find\n");//无法放入缓冲区
        close(fd);// 关闭文件描述符 fd
        return;
    }
    
    strcpy(buf, dir);// 将 dir 指向的字符串即绝对路径复制到 buf
    p = buf + strlen(buf);//p指向文件名
    *p++ = '/';//路径后面加前缀/

    // 读取fd，无法读取一个完整的目录项时停止循环
    while (read(fd, &de, sizeof(de)) == sizeof(de))
    {
        //根据inum字段判断目录项是否有效
        if(de.inum == 0)
            continue;

        // 防止递归"." 和 ".." 根据目录项的名称字段判断
        if (strcmp(de.name, ".")==0 || strcmp(de.name, "..")==0)
            continue;
        
        memmove(p, de.name, DIRSIZ);//把目录项名称复制到p，长度为DIRSIZ
        p[DIRSIZ] = 0; //设置文件名结束符

        // int stat(char *, struct stat *);
        if(stat(buf, &st) < 0)
        {
            fprintf(2, "error: cannot stat %s in find\n", buf);
            continue;
        }
        
        if (st.type == T_DIR)// 如果是目录类型，递归查找
            find(buf, file);
        else if (st.type == T_FILE && strcmp(de.name, file)==0)// 如果是文件类型 并且 名称与要查找的文件名相同 
            printf("%s\n", buf);// 打印缓冲区存放的路径
    }
}

int main(int argc, char *argv[])
{
    if (argc != 3)// 如果参数个数不为 3 则报错
    {
        fprintf(2, "usage: find dirName fileName\n");// 输出提示
        exit(1);// 异常退出
    }
    find(argv[1], argv[2]);// 调用 find 函数查找指定目录下的文件
    exit(0);// 正常退出
}
