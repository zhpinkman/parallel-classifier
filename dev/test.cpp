// C program to implement one side of FIFO
// This side writes first, then reads
#include <stdio.h>
#include <iostream>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

using namespace std;

int main()
{
    int fd;

    // FIFO file path
    char *myfifo = "/tmp/myfifo";

    // Creating the named file(FIFO)
    // mkfifo(<pathname>, <permission>)
    cout << "111" << endl;
    mkfifo(myfifo, 0666);
    cout << "2" << endl;

    char arr1[80], arr2[80];
    while (1)
    {
        // Open FIFO for write only
        fd = open(myfifo, O_WRONLY);

        // Take an input arr2ing from user.
        // 80 is maximum length
        cout << "kkk" << endl;
        fgets(arr2, 80, stdin);
        cout << "kdkafkjfak" << endl;

        // Write the input arr2ing on FIFO
        // and close it
        write(fd, arr2, strlen(arr2) + 1);
        close(fd);
        cout << "closed" << endl;
        // Open FIFO for Read only
        fd = open(myfifo, O_RDONLY);

        // Read from FIFO
        cout << "opened" << endl;
        read(fd, arr1, sizeof(arr1));
        cout << arr1 << endl;

        // Print the read message
        printf("User2: %s\n", arr1);
        close(fd);
    }
    return 0;
}
