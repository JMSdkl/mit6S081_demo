#include "kernel/param.h"
#include "kernel/types.h"
#include "user/user.h"

#define buf_size 512
// echo hello too | xargs echo bye
// bye hello too
/*
argc 是 xargs后面的 echo bye  前面的是通过管道| 将前面hello too这个标准输出输入到 xargs的 echo bye后面
*/
int main(int argc, char *argv[])
{
    // 要解决的问题：
    // 1：怎么获取前一个命令的标准化输出（即是 xargs这个命令的标准化输入）？
    // 从文件描述符0 读出来的 前一个的标准输出
    char buf[buf_size];
    read(0, buf, buf_size);
    printf("标准化输入是：%s\n", buf);
    // 2：如何获得到自己的命令行参数 argv里面
    // 3：如何使用exec去执行命令
    exit(0);
}