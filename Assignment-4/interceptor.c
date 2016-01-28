#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/syscalls.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Aurobindo Mondal");

asmlinkage long (* ref_sys_pinformation)(void);
unsigned long **sys_call_table;
unsigned long original_cr0;

unsigned long **get_sys_call_table(void) {
	unsigned long offset = PAGE_OFFSET;
	unsigned long **sct;
	
	while(offset < ULLONG_MAX) {
		sct = (unsigned long **)offset;
		
		if(sct[__NR_close] == (unsigned long *) sys_close) return sct;
		offset += sizeof(void *);
	}
	return NULL;
}

asmlinkage long new_sys_pinformation(void)
{
        struct task_struct *tsk = current;
        printk(KERN_INFO "This is intercepted system call. It returns the parent process ID.\n");
        printk(KERN_INFO "pid = %d and ppid = %d \n",tsk->pid,tsk->parent->pid);
        return tsk->parent->pid;
}

int __init interceptor_init(void) 
{
	if(!(sys_call_table = get_sys_call_table()))	
		return -1;

	original_cr0 = read_cr0();
	write_cr0(original_cr0 & ~0x00010000);
	ref_sys_pinformation = (void *)sys_call_table[__NR_pinformation];
	sys_call_table[__NR_pinformation] = (unsigned long *)new_sys_pinformation;
	write_cr0(original_cr0);

	return 0;
}

void __exit interceptor_end(void) 
{
	if(!sys_call_table)	return;

	write_cr0(original_cr0 & ~0x00010000);
	sys_call_table[__NR_pinformation] = (unsigned long *)ref_sys_pinformation;
	write_cr0(original_cr0);	
}

module_init(interceptor_init);
module_exit(interceptor_end);
