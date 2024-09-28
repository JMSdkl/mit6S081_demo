#include "kernel/param.h"
#include "kernel/types.h"
#include "user/user.h"

#define buf_size 512
#define MSGSIZE 16
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
    char buf[MSGSIZE];
    read(0, buf, MSGSIZE);
    // printf("标准化输入是：%s\n", buf);
    // 2：如何获得到自己的命令行参数 argv里面
    char *xargv[MSGSIZE];
    int xargc = 0;
    for (int i = 1; i < argc; i++)
    {
        xargv[xargc] = argv[i];
        xargc++;
    }
    // 以换行符进行切割
    char *p = buf; // 前面拿到的标准输出
    for (int i = 0; i < MSGSIZE; ++i)
    {
        if (buf[i] == '\n')
        {
            int pid = fork();
            if (pid > 0)
            {
                p = &buf[i + 1];
                wait(0);
            }
            else
            {
                // 3：如何使用exec去执行命令
                buf[i] = 0;
                xargv[xargc] = p;
                xargc++;
                xargv[xargc] = 0;
                xargc++;
                exec(xargv[0], xargv);
                exit(0);
            }
        }
    }

    // 3：如何使用exec去执行命令
    // exec("echo", argv);
    wait(0);
    exit(0);
}