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
	char * c = "/dev/memory";
      int device = open(c, O_RDWR);
      printf("%d\n",device);
      write(device,"WWWWW",5);
     
      	//ioctl(device, IOCTL_FIX_TABLE);
	void* x = "";
     	int ret = read(device, x, 1);
	char * ch = (char*)x;
	printf("%s -- %d\n",ch,ret);
	close(device);
      return 0;
   }
