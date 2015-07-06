#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/delay.h>
#include <linux/poll.h>
#include <linux/irq.h>
#include <asm/irq.h>
#include <linux/interrupt.h>
#include <asm/uaccess.h>
#include <mach/regs-gpio.h>
#include <mach/hardware.h>
#include <linux/platform_device.h>
#include <linux/cdev.h>
#include <linux/miscdevice.h>
#include <linux/gpio.h>
#include <linux/sched.h>

#define BUTTON_DEVICE_NAME "button_dev"
struct button_irq_desc{
	int irq;
	int pin;
	int pin_setting;
	int number;
	char *name;
};

static struct button_irq_desc button_irqs[]={
	{IRQ_EINT8,S3C2410_GPG(0),S3C2410_GPG0_EINT8,0,"KEY0"},
	{IRQ_EINT11,S3C2410_GPG(3),S3C2410_GPG3_EINT11,1,"KEY1"},
	{IRQ_EINT13,S3C2410_GPG(5),S3C2410_GPG5_EINT13,2,"KEY2"},
	{IRQ_EINT15,S3C2410_GPG(7),S3C2410_GPG7_EINT15,3,"KEY3"},
	{IRQ_EINT14,S3C2410_GPG(6),S3C2410_GPG6_EINT14,4,"KEY4"},
	{IRQ_EINT19,S3C2410_GPG(11),S3C2410_GPG11_EINT19,5,"KEY5"},
};

static volatile char key_values[]={'0','0','0','0','0','0'};
static volatile int ev_press=0;

static DECLARE_WAIT_QUEUE_HEAD(button_waitq);
static irqreturn_t button_interrupt(int irq,void *dev_id)
{
	struct button_irq_desc *button_irqs =(struct button_irq_desc*)(dev_id);
	int down;

	down = !s3c2410_gpio_getpin(button_irqs->pin);
	if(down!=(key_values[button_irqs->number]&1)){
		key_values[button_irqs->number]='0'+down;
		ev_press=1;
		wake_up_interruptible(&button_waitq);
	}
	printk("button_interrup =%d\n",button_irqs->number);
	return IRQ_RETVAL(IRQ_HANDLED);
}

static int s3c2440_button_open(struct inode * inode,struct file *file)
{
	int err;
	int i=0;
	for( i=0;i<sizeof(button_irqs)/sizeof(button_irqs[0]);i++){
		err = request_irq(button_irqs[i].irq,button_interrupt,IRQ_TYPE_EDGE_BOTH,button_irqs[i].name,(void*)&button_irqs[i]);
		if(err)
			break;
	}

	if(err){
		i--;
		for(;i>=0;i--){
			disable_irq(button_irqs[i].irq);
			free_irq(button_irqs[i].irq,(void*)&button_irqs);
		}
		return -EBUSY;
	}
}

static int s3c2440_button_close(struct inode *inode,struct file *file)
{
	int i;
	for (i=0;i<sizeof(button_irqs)/sizeof(button_irqs[0]);i++){
		free_irq(button_irqs[i].irq,(void*)&button_irqs);
	}

	return 0;
}

static int s3c2440_button_read(struct file *filp,char __user *buff,size_t count,loff_t *offp)
{
	unsigned long err;
	if(!ev_press){
		if(filp->f_flags&O_NONBLOCK)
			return -EAGAIN;
		else
			wait_event_interruptible(button_waitq,ev_press);
	}

	ev_press =0 ;
	err = copy_to_user(buff,(const void *)key_values,min(sizeof(key_values),count));
	return err;
}

static unsigned int s3c2440_button_poll( struct file *file, struct poll_table_struct *wait)
{
	unsigned int mask = 0;
	poll_wait(file, &button_waitq, wait);
	
	if (ev_press)
		mask |= POLLIN | POLLRDNORM;
	return mask;
}

static struct file_operations button_dev_fops ={
	.owner = THIS_MODULE,
	.open = s3c2440_button_open,
	.release = s3c2440_button_close,
	.read = s3c2440_button_read,
	.poll = s3c2440_button_poll,
};

static struct miscdevice misc={
	.minor = MISC_DYNAMIC_MINOR,
	.name = BUTTON_DEVICE_NAME,
	.fops = &button_dev_fops,
};

static int  __init mini2440_button_module_init(void){
	int ret ;
	ret = misc_register(&misc);
	printk("button module init ok!");
	return ret;
}

static void __exit mini2240_button_module_exit(void){
	misc_deregister(&misc);
	printk("button module exit ok!");
}

module_init(mini2440_button_module_init);
module_exit(mini2240_button_module_exit);
MODULE_LICENSE("GPL");
MODULE_AUTHOR("selwyn.wu");
