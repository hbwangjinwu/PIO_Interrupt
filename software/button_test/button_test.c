#include <stdio.h>  
#include <sys/types.h>  
#include <sys/stat.h>  
#include <fcntl.h>  
#include <unistd.h>  
  
  
int main(int argc ,char *argv[])  
{  
    int fd;  
    unsigned char button_val;  
    int i;
	unsigned char mask=0x01;
    fd = open("/dev/pio_button",O_RDWR);  
    if (fd < 0)  
    {  
        printf("open file error\n");  
    }  
    while(1)  
    {  
		mask=0x01;
        read(fd,&button_val,1); 
		for(i=0;i<4;i++)
		{
			if(button_val&mask)
				 printf("button[%d] is pressed\n",i);
		    mask<<=1;
		}               
    }  
    return 0;  
}  

