/*
 * hello−1.c − My First kernel module.
 * Author - Aurobindo
 */
 
#include <linux/module.h> /* Needed by all modules */
#include <linux/kernel.h> /* Needed for KERN_INFO */


int init_module(void)
{
	printk(KERN_INFO "Hello world!!! I am ready to ROCK!!!\n");
	return 0;
}
void cleanup_module(void)
{
	printk(KERN_INFO "Goodbye world!!! I think, I need some rest!!!\n");
}
