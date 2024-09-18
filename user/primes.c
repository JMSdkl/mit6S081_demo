#include "kernel/types.h"
#include "user/user.h"

void runprocess(int listenFd)
{

    int my_num = 0;
    int forked = 0;
    int passed_num = 0;
    int pipes[2];
    while (1)
    {
        int read_bytes = read(listenFd, &passed_num, 4); // 读出来第一个数字
                                                         // left neighbor has no more number to provide
        if (read_bytes == 0)
        {
            close(listenFd);
            if (forked)
            {
                // tell my children I have no more number to offer
                close(pipes[1]);
                // wait my child termination
                int child_pid;
                wait(&child_pid);
            }
            exit(0);
        }
        if (my_num == 0)
        {
            my_num = passed_num;
            printf("prime %d\n", my_num);
        }
        if (passed_num % my_num != 0)
        {
            if (!forked)
            {
                pipe(pipes);
                forked = 1;
                int ret = fork();
                if (ret == 0)
                {
                    close(pipes[1]);
                    close(listenFd);
                    runprocess(pipes[0]);
                }
                else
                {
                    close(pipes[0]);
                }
            }
            write(pipes[1], &passed_num, 4);
        }
    }
}

int main(int argc, char **argv)
{
    int pipes[2];
    pipe(pipes);
    for (int i = 2; i <= 35; i++)
    {
        write(pipes[1], &i, 4); // 4字节大小
    }
    close(pipes[1]);
    runprocess(pipes[0]);
    exit(0);
}