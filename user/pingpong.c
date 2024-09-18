#include "kernel/types.h"
#include "user/user.h"

int main(int argc, char **argv)
{
    int pid;
    int pipes1[2], pipes2[2];
    pipe(pipes1);
    pipe(pipes2);
    int ret = fork();
    char buf[] = {'a'};
    if (ret == 0)
    { // 进入了子进程
        pid = getpid();
        close(pipes1[1]);        // 关闭写通道
        close(pipes2[0]);        // 关闭读通道
        read(pipes1[0], buf, 1); // 没有数据就卡住在这里先
        printf("%d: received ping\n", pid);
        write(pipes2[1], buf, 1); // 读取到了父进程的数据 返回消息给父进程
        exit(0);
    }
    else
    { // 进入父进程
        pid = getpid();
        close(pipes2[1]);         // 关闭写通道
        close(pipes1[0]);         // 关闭读通道
        write(pipes1[1], buf, 1); // 父进程先写一个数据给子进程
        read(pipes2[0], buf, 1);  // 读取子进程返回的一个数据
        printf("%d: received pong\n", pid);
        exit(0);
    }
}
