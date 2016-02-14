#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/sched.h>
#include <linux/slab.h>
#include <linux/list.h>		/* Needed for linked list implementation */
#include <linux/kfifo.h>	/* Needed for Queue Implementation */	

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Aurobindo Mondal");



//DECLARE_KFIFO(fifo, unsigned int, PAGE_SIZE);	/* Declare a pointer to Queue */
//INIT_KFIFO (fifo) ;


//struct kfifo fifo;

int numberOfProcess(void)	{
	int count=0;
	struct task_struct *task;
	for_each_process(task) {
		count++;
	}
	return count;
}

int producer(void)	{
	struct task_struct *task;
	//if(kfifo_alloc(&fifo, sizeof(int)*numberOfProcess(), GFP_KERNEL)) return 0;
	for_each_process(task)	{
		if(!kfifo_put(&fifo, task->pid)) return 1;
	}
	return 1;
}

void consumer(void)	{
	int val;
	while(kfifo_get(&fifo, val)) {
		printk(KERN_ALERT "pid = %d\n", val);
	}
}

int init_module(void) {
	struct task_struct *task;
	if(producer())	return -1;
	consumer();
	printk(KERN_ALERT "\n-----------------------------\n");
	for_each_process(task)	printk(KERN_ALERT "[test] pid = %d\n", task->pid);
	 	
	return 0;
}
	
void cleanup_module(void) {
	kfifo_free(&fifo);
	printk(KERN_INFO "It's done!!!!");
}
	

