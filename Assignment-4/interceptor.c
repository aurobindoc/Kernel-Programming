#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/syscalls.h>

unsigned long **sys_call_table;

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Aurobindo Mondal");

unsigned long **acquire_sys_call_table(void) {
	unsigned long offset = PAGE_OFFSET;
	unsigned long **sct;
	
	while(offset < ULLONG_MAX) {
		sct = (unsigned long **)offset;
		
		if(sct[__NR_close] == (unsigned long *) sys_close) return sct;
		offset += sizeof(void *);
	}
	return NULL;
}

int __init interceptor_init(void) {
	if(!(sys_call_table = get_sys_call_table()))	return -1;

	
	return 0;
}

void __exit interceptor_end(void) {

}

module_init(interceptor_start);
module_exit(interceptor_end);
