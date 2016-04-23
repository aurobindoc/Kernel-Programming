/*
 * hello−1.c − My First kernel module.
 * Author - Aurobindo
 */
 
#include <linux/module.h> /* Needed by all modules */
#include <linux/kernel.h> /* Needed for KERN_INFO */
#include <linux/moduleparam.h>
#include <linux/init.h>
#include <linux/stat.h>


int arr_argc = 0;
int myintArray[3];
module_param_array(myintArray, int, &arr_argc, 0000);
MODULE_PARM_DESC(myintArray, "An integer Array");

int init_module(void)
{
	int i;
	for(i=0;  i < (sizeof myintArray / sizeof (int)); i++) {
		printk(KERN_INFO "myintArray[%d] = %d \n", i, myintArray[i]);
	}
 	return 0;
}
void cleanup_module(void)
{
 printk(KERN_INFO "Goodbye world!!! I think, I need some rest!!!\n");
}
