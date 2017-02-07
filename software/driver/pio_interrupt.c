//platform driver
#include <linux/module.h>
#include <linux/types.h>
#include <linux/miscdevice.h>
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/platform_device.h>
#include <linux/interrupt.h>
#include <linux/clk.h>
#include <linux/uaccess.h>
#include <linux/io.h>
#include <linux/poll.h>
#include <linux/irq.h>
#include <asm/unistd.h>
#include <linux/device.h>
#include <linux/of.h>
#include <linux/slab.h>
#include <linux/sched.h>


// define the interrupt registers,  refer to embedded ip user guide for infomation
#define  PIO_REGISTER_INTERRUPTMSK 2
#define  PIO_REGISTER_EDGECAPTUE   3

struct irq_des
{
	int  button_irq;
	void __iomem *pio_base;
};


static volatile int key_values;
static volatile int ev_press = 0;

//define the wait quene head
static DECLARE_WAIT_QUEUE_HEAD(button_waitq);


struct irq_des pio_irq;



static irqreturn_t buttons_interrupt(int irq, void *dev_id)
{
	if(irq ==pio_irq.button_irq){
		key_values = readl(pio_irq.pio_base+PIO_REGISTER_EDGECAPTUE*4);
		ev_press = 1;
		wake_up_interruptible(&button_waitq);	
	}
	//clear all  edge caputre registers
	writel(0x0f,pio_irq.pio_base+PIO_REGISTER_EDGECAPTUE*4); 
    return IRQ_RETVAL(IRQ_HANDLED);
}


static int pio_buttons_open(struct inode *inode, struct file *file)
{
    int err = 0; 
    err = request_irq(pio_irq.button_irq, buttons_interrupt, 0, 
                          "PIO_BUTTON", NULL);
    if (err) {
	    disable_irq(pio_irq.button_irq);
        free_irq(pio_irq.button_irq, NULL);
		return -EBUSY;
    }  
    ev_press = 0;
    //enable pio interrupt 
    writel(0x0f,pio_irq.pio_base+PIO_REGISTER_INTERRUPTMSK*4); 
    return 0;
}


static int pio_buttons_close(struct inode *inode, struct file *file)
{
    
	free_irq(pio_irq.button_irq, NULL);
        return 0;
}


static int pio_buttons_read(struct file *filp, char __user *buff, size_t count, loff_t *offp)
{
    unsigned long err;
    wait_event_interruptible(button_waitq, ev_press);
    ev_press = 0;

    err = copy_to_user(buff, (const void *)&key_values, min(sizeof(key_values), count));

    return err ? -EFAULT : min(sizeof(key_values), count);
}

static unsigned int pio_buttons_poll( struct file *file, struct poll_table_struct *wait)
{
    unsigned int mask = 0;
    poll_wait(file, &button_waitq, wait);
    if (ev_press)
        mask |= POLLIN | POLLRDNORM;
    return mask;
}


static struct file_operations dev_fops = {
    .owner   =   THIS_MODULE,
    .open    =   pio_buttons_open,
    .release =   pio_buttons_close, 
    .read    =   pio_buttons_read,
    .poll    =   pio_buttons_poll,
};

static struct miscdevice misc = {
	.minor = MISC_DYNAMIC_MINOR,
	.name = "pio_button",
	.fops = &dev_fops,
};

static int my_plat_probe(struct platform_device *pdev)
{
	int ret;
	struct resource *res;
	int  irq;
	int  error;
	res = platform_get_resource(pdev,IORESOURCE_MEM,0);
	if(res == NULL)
		return -ENOENT;	
	if(!request_mem_region(res->start,resource_size(res),pdev->name)){
			error=-EBUSY;
			return error;
	}
	pio_irq.pio_base=ioremap(res->start,resource_size(res));
	irq=platform_get_irq(pdev,0);
	if(irq<0)
	{
		return irq;
	}
	pio_irq.button_irq=irq;
	ret = misc_register(&misc);
	if(ret)
		return ret;
		
	return 0;
}

static int my_plat_remove(struct platform_device *pdev)
{
	struct resource *res;
	printk("my platfrom device has removed.\n");
	res=platform_get_resource(pdev,IORESOURCE_MEM,0);
	release_mem_region(res->start,resource_size(res));
	misc_deregister(&misc);
	return 0;
}

static struct of_device_id pio_button_match[]={
	{.compatible="terasic,pio-button",.data=NULL},
	{}
};
MODULE_DEVICE_TABLE(of,pio_button_match);

struct platform_driver my_buttons_drv = { 
	.probe = my_plat_probe,
	.remove = my_plat_remove,
	.driver = { 
		.owner = THIS_MODULE,
		.name = "my_buttons",
		.of_match_table=pio_button_match,
	},
};

static int __init platform_drv_init(void)
{
	int ret;

	ret = platform_driver_register(&my_buttons_drv);
	
	return ret;
}

static void __exit platform_drv_exit(void)
{
	platform_driver_unregister(&my_buttons_drv);
}

module_init(platform_drv_init);
module_exit(platform_drv_exit);

MODULE_AUTHOR("Terasic");
MODULE_LICENSE("GPL");
