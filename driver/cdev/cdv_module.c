#include <linux/miscdevice.h>
#include <linux/delay.h>
#include <asm/irq.h>
#include <mach/regs-gpio.h>
#include <mach/hardware.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/mm.h>
#include <linux/fs.h>
#include <linux/types.h>
#include <linux/delay.h>
#include <linux/moduleparam.h>
#include <linux/slab.h>
#include <linux/errno.h>
#include <linux/ioctl.h>
#include <linux/cdev.h>
#include <linux/string.h>
#include <linux/list.h>
#include <linux/pci.h>
#include <asm/uaccess.h>
#include <asm/atomic.h>
#include <asm/unistd.h>
#include <linux/gpio.h>
#include <linux/device.h>
#include <linux/cdev.h>

#define CDEV_NAME "cdev_led"
#define LED_MAJOR 241
static int led_major = LED_MAJOR;
struct  cdev cdev;

static int led_drv_open(struct inode * inode,struct file *file)
{
	return 0;
}

static ssize_t led_drv_write(struct file *file,const char __user *buf, size_t count,loff_t *ppos)
{
	return 0;
}

static struct file_operations led_dev_fops ={
	.owner = THIS_MODULE,
	.open = led_drv_open,
	.write = led_drv_write,
};

static int __init mini2440_cdev_module_init(void)
{

	int err;
	dev_t devno;
	int result;
	devno = MKDEV(led_major,0);
	if(led_major)
		result =register_chrdev_region(devno,1,CDEV_NAME);
	else
	{
		result = alloc_chrdev_region(&devno,0,1,CDEV_NAME);
		led_major = MAJOR(devno);
	}

	if(result<0)
	{
		printk(KERN_WARNING "hello:can't get major number");
		return result;
	}

	cdev_init(&cdev,&led_dev_fops);
	cdev.owner = THIS_MODULE;
	cdev.ops = &led_dev_fops;
	err = cdev_add(&cdev,devno,1);
	if(err)
		printk(KERN_NOTICE "error %d adding",err);
	
	printk(CDEV_NAME "\tinitialized\n");
	
	return err;
	
}

static void __exit mini2440_cdev_module_exit(void)
{
	cdev_del(&cdev);
	unregister_chrdev_region(MKDEV(led_major,0),1);
	printk("good bye,mini2440 cdev module was removed!\n");
}

module_init(mini2440_cdev_module_init);
module_exit(mini2440_cdev_module_exit);
MODULE_LICENSE("GPL");
MODULE_AUTHOR("selwyn.wu");
