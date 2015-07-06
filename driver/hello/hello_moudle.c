#include <linux/kernel.h>
#include <linux/module.h>
static int __init mini2440_hello_module_init(void)
{
	printk("hello,Mini2440 module is installed!\n");
	return 0;
	
}

static void __exit mini2440_hello_module_exit(void)
{
	printk("good bye,mini2440 module was removed!\n");
}

module_init(mini2440_hello_module_init);
module_exit(mini2440_hello_module_exit);
MODULE_LICENSE("GPL");
