/*
 * hello−1.c − My First kernel module.
 * Author - Aurobindo
 */
 
#include <linux/module.h> /* Needed by all modules */
#include <linux/kernel.h> /* Needed for KERN_INFO */
#include <linux/moduleparam.h>
#include <linux/init.h>
#include <linux/stat.h>



int myint = 3;
module_param(myint, int, 0);
MODULE_PARM_DESC(myint, "An integer");

int init_module(void)
{
 printk(KERN_INFO "The integer myint has the value %d. \n", myint);
 return 0;
}
void cleanup_module(void)
{
 printk(KERN_INFO "Goodbye world!!! I think, I need some rest!!!\n");
}
