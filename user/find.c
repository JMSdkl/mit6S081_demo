#include "kernel/types.h"
#include "kernel/fcntl.h"
#include "kernel/fs.h"
#include "kernel/stat.h"
#include "user/user.h"
char *findFileName(char *pathName)
{
    char *curr = strchr(pathName, '/'); // 找到路径的第一个斜杠  目标是想找最后一个斜杠
    char *prev = 0;
    while (curr != 0)
    { // 找不到为止
        prev = curr;
        curr = strchr(curr + 1, '/'); // 找下一个
    }
    return prev;
}
void find(char *curr_path, char *target)
{
    char buf[512], *p;
    int fd;
    struct dirent de;
    struct stat st;
    if ((fd = open(curr_path, O_RDONLY)) < 0)
    { // 打开这个文件
        fprintf(2, "find: cannot open %s\n", curr_path);
        return;
    }
    if (fstat(fd, &st) < 0)
    { // 看这个路径是不是文件 还是文件夹
        fprintf(2, "find: cannot stat %s\n", curr_path);
        close(fd);
        return;
    }
    // 所以curr_path一定是要一个完整的路径

    switch (st.type)
    {            // 文件夹 和文件 分开处理
    case T_FILE: // 如果是文件 那就要找到路径最后一个东西
        ;
        // root/usr/myfile.s 中的myfile.s 和要找的东西做比较
        char *fillname = findFileName(curr_path);
        int match = 1;
        if (fillname == 0 || strcmp(fillname + 1, target) != 0)
        {
            match = 0; // 不匹配
        }
        if (match) // 匹配就打印出来
            printf("%s\n", curr_path);
        close(fd);
        break;

    case T_DIR:
        // 如果是文件夹 那就要找这个文件夹下面的所有东西
        ;
        memset(buf, 0, sizeof(buf)); // 清空
        uint curr_path_len = strlen(curr_path);
        memcpy(buf, curr_path, curr_path_len); // 先获得原来的路径 后面再继续加
        buf[curr_path_len] = '/';
        p = buf + curr_path_len + 1; // p指向最后一个斜杠的后一位 ，等一下做拼接使用

        /*
        假设你打开了一个包含 3 个文件的目录 /home/user：

        file1.txt
        file2.txt
        subdir/（一个子目录）
        当你使用 read(fd, &de, sizeof(de)) 进行读取时，第一次会读取到第一个文件的目录项，
        将 file1.txt 的信息填充到 struct dirent 中。第二次调用 read() 会读取下一个目录项，
        即 file2.txt，依此类推，直到目录中的所有条目都被读取。
        */
        while (read(fd, &de, sizeof(de)) == sizeof(de))
        {
            if (de.inum == 0 || strcmp(de.name, ".") == 0 ||
                strcmp(de.name, "..") == 0) // 要忽略父目录和上一级目录
                continue;
            memcpy(p, de.name, DIRSIZ);
            p[DIRSIZ] = 0;
            find(buf, target);
        }
        close(fd);
        break;
    }
}

int main(int argc, char **argv)
{
    if (argc != 3)
    {
        fprintf(2, "usage: find [directory] [target filename]\n");
        exit(1);
    }
    find(argv[1], argv[2]);
    exit(0);
}