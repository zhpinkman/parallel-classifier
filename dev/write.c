// C program to implement one side of FIFO
// This side writes first, then reads
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

int main()
{
    int fd;

    // FIFO file path
    char *myfifo = "./myfifo";

    mkfifo(myfifo, 0666);

    char arr1[80], arr2[80];
    // Open FIFO for write only
    fd = open(myfifo, O_WRONLY);

    // Take an input arr2ing from user.
    // 80 is maximum length
    char ss[] = "hello";
    char yy[] = "bye";

    // Write the input arr2ing on FIFO
    // and close it
    write(fd, ss, strlen(ss) + 1);
    write(fd, yy, strlen(yy) + 1);
    close(fd);
    return 0;
}
