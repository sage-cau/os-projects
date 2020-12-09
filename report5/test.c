#include <stdio.h>
#include <string.h> // strlen()
#include <unistd.h> // open, close, read, write
#include <fcntl.h>  // O_RDWR, O_NONBLOCK
#define BUFF_SIZE 1024
int main()
{
    int fd;
    char *str_hello = "Hello, Virtual Buffer!!";
    char buff[BUFF_SIZE];

    printf("Device driver test.\n");
    if ((fd = open("/dev/virtual_buffer", O_RDWR)) > 0)
    {
        write(fd, str_hello, strlen(str_hello) + 1);
        printf("write to device: %s\n", buff);
        read(fd, buff, BUFF_SIZE);
        printf("read from device: %s\n", buff);
        close(fd);
    }
    return 0;
}
