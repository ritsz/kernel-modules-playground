#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>

int main()
{
        int fd = open("/dev/keylog", O_RDWR);
        ssize_t nread;
        char buf[10];
        if (fd == -1) {
                perror("open");
                return -1;
        }
	printf("/dev/keylog was opened, fd = %d\n", fd);
        nread = read(fd, buf, 10);

        if (nread == -1) {
                perror("read");
                return -1;
        } else {
                printf("%d characters were read\n", nread);
        }
        write(1, buf, nread);
        close(fd);
        return 0;
}

