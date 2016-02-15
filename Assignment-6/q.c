#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/sched.h>
#include <linux/slab.h>
#include <linux/list.h>		/* Needed for linked list implementation */
#include <linux/kfifo.h>	/* Needed for Queue Implementation */	

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Aurobindo Mondal");

DECLARE_KFIFO_PTR(fifo, int);	/* Declare a pointer to Queue */

//DECLARE_KFIFO(fifo, unsigned int, PAGE_SIZE);	
//INIT_KFIFO (fifo) ;


//struct kfifo fifo;

DECLARE_KFIFO(test, unsigned char, PAGE_SIZE);

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
	if(kfifo_alloc(&fifo, sizeof(int)*numberOfProcess(), GFP_KERNEL)) return 0;
	for_each_process(task)	{
		printk(KERN_ALERT "%d\n", task->pid);
		if(!kfifo_put(&fifo, task->pid)) return 1;
	}
	return 1;
}

void consumer(void)	{
	int val;
	while(kfifo_get(&fifo, &val)) {
		printk(KERN_ALERT "pid = %d\n", val);
	}
}

int init_module(void) {
	int i;
	unsigned char res;
	printk(KERN_ALERT "Hey How're you???\n");
	struct task_struct *task;
	if(!producer())	return -1;
	consumer();
	printk(KERN_ALERT "\n-----------------------------\n");
	for_each_process(task)	printk(KERN_ALERT "[test] pid = %d\n", task->pid);
	printk(KERN_ALERT "\n-----------------------------\n Putting different types of variable in queue: \n");
	INIT_KFIFO(test);
	
	kfifo_in(&test, "hello", 5);
	
	for(i=0; i!=10; i++)	kfifo_put(&test, i);
	
	kfifo_put(&test, 'a');
	kfifo_put(&test, 'u');
	kfifo_put(&test, 'r');
	kfifo_put(&test, 'o');
	
	while(kfifo_get(&test, &res))	{
		printk(KERN_ALERT "%c\n",res);
	}
	
	 	
	return 0;
}
	
void cleanup_module(void) {
	kfifo_free(&fifo);
	printk(KERN_ALERT "It's done!!!!");
}
	

