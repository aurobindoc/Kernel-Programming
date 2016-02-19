#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/moduleparam.h>
#include <linux/init.h>
#include <linux/sched.h>
#include <linux/mm_types.h>
#include <linux/fs.h>
#include <linux/mm.h>
#include <linux/slab.h>
#include <linux/stat.h>
#include <uapi/asm/mman.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Aurobindo Mondal");


int pID = -1;
module_param(pID, int, 0);
MODULE_PARM_DESC(pID, "An integer");

struct task_struct * trackProcessInfo(void) {
	struct task_struct *tsk = NULL;
	for_each_process(tsk) {
		if(pID == tsk->pid) {
			return tsk;
		}
	}
	return NULL;
}

void showVMArea(struct task_struct *task) {
	struct vm_area_struct *vm = task->mm->mmap;
	char *fileName;
	while(vm != NULL)	{
		vm_flags_t flags = vm->vm_flags;
		unsigned long start = vm->vm_start, end = vm->vm_end;
		if(vm->vm_file != NULL) fileName = vm->vm_file->f_path.dentry->d_iname;
		else fileName = "[ anon ]";	// may be [ anon ] or [ stack ]
		
		/* Tried "vm_is_stack_for_task(task, vm)" in mm/util.c but its doesnot work, 
		I don't find it in any library*/
		
				
		if (stack_guard_page_start(vm, start)) {	// remove 1 guard page from vma start
			start += PAGE_SIZE;
			fileName = "[ stack ]";
		}
		if (stack_guard_page_end(vm, end))	{		// remove 1 guard page from vma end
			end -= PAGE_SIZE;
			fileName = "[ stack ]";
		}
		
		unsigned long size = (end - start)/1024;		

		char r = flags & VM_READ ? 'r' : '-';
		char w = flags & VM_WRITE ? 'w' : '-';
		char x = flags & VM_EXEC ? 'x' : '-';
		char s = flags & VM_MAYSHARE ? 's' : '-';
		char R = MAP_NORESERVE ? '-' : 'R';

		printk(KERN_INFO "%12lx\t%lu KB\t%c%c%c%c%c\t%s\n",start, size,r,w,x,s,R,fileName);
		vm = vm->vm_next;
	}
}

int __init initiate(void) {
	printk(KERN_INFO "Started tracking Information about process %d\n", pID);
	struct task_struct *task = trackProcessInfo();
	printk(KERN_INFO "\nShowing VM Area of process %d : \n", pID);
	if(task != NULL)	showVMArea(task);
	else	printk(KERN_INFO "Error : Sorry !!! process not found. Enter correct pid. \n");
	return 0;
}

void __exit terminate(void) {
	printk(KERN_INFO "It's Done !!! \n");
}

module_init(initiate);
module_exit(terminate);
