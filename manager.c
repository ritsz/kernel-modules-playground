#include <stdio.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

/* Define ioctl commands */
#define IOCTL_PATCH_TABLE 0x00000001
#define IOCTL_FIX_TABLE   0x00000004

int main(void)
{
	char * c = "/dev/mem";
      	int device = open(c, O_RDWR);
      	printf("%d\n",device);
      	if ( write(device,"WWWWW",5) < 0) {
		perror("write");
		return -1;
	}
     
      	ioctl(device, IOCTL_FIX_TABLE);
	char x[30];
     	int ret;
	if ((ret = read(device, x, 30)) < 0) {
		perror("Read");
		return -1;
	}
	printf("%s -- %d\n",x,ret);
	close(device);
      	return 0;
}
