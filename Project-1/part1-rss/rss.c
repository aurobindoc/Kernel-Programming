#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/moduleparam.h>
#include <linux/init.h>

#include <linux/list.h>
#include <linux/sched.h>
#include <linux/mm.h>
#include <linux/slab.h>
#include "../arch/x86/include/asm/pgtable.h"
#include <asm/io.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Aurobindo Mondal");

int pID = -1;
module_param(pID, int, 0);
MODULE_PARM_DESC(pID,"Process ID");

struct node {
	struct list_head list;
	int rss;
	int vmpages;
};
struct node output;


long vmpages = 0, rss = 0;

pte_t *isRSS(struct mm_struct *mm, unsigned long page)	{
	pgd_t *pgd;
	pud_t *pud;
	pmd_t *pmd;
	pte_t *pte;

	pgd = pgd_offset(mm, page);
	if(pgd_none(*pgd) || pgd_bad(*pgd)) {
		return NULL;
	}
	
	pud = pud_offset(pgd, page);
	if(pud_none(*pud) || pud_bad(*pud)) {
		return NULL;
	}

	pmd = pmd_offset(pud, page);
	if(pmd_none(*pmd) || pmd_bad(*pmd)) {
		return NULL;
	}

	pte = pte_offset_kernel(pmd, page);
  	return pte;
}

int __init initialise(void)	{
	struct task_struct *task = NULL;
	struct vm_area_struct *vma = NULL;
	struct node *temp;
	unsigned long page;
	pte_t *pte;
	int r, v;
	INIT_LIST_HEAD(&output.list);
	
	if((task = pid_task(find_vpid(pID), PIDTYPE_PID)) == NULL)	{	// Assigns task_struct of process pID to *task
		printk(KERN_INFO "Enter Correct pID.\n");
		return 0;
	}
	
	vma = task->mm->mmap;
	
	while(vma != NULL)	{
		page = vma->vm_start;
		r=0;
		v=0;
		while(page < vma->vm_end)	{
			pte=isRSS(task->mm, page);
            if(pte==NULL)    printk(KERN_INFO "Page Unmapped\n"); 
            else    {
	        	if(pte && pte_present(*pte)) {
		        	printk(KERN_INFO "Physical Address of Page : %lx || Page present in RAM\n",(unsigned long)pte_pfn(*pte)<<PAGE_SHIFT);
                	rss++;
					r++;
				}
            	else 	printk(KERN_INFO "Page not present in RAM\n");		
        	}
			v++;
			vmpages++;
			page += PAGE_SIZE;
		}
		temp = (struct node *)kmalloc(sizeof(struct node),GFP_KERNEL);
		temp->rss = r*PAGE_SIZE/1024;
		temp->vmpages = v*PAGE_SIZE/1024;
		list_add_tail(&(temp->list), &(output.list));
		vma = vma->vm_next;
	}
	
	list_for_each_entry(temp, &output.list, list) {
		printk(KERN_INFO "Rss = %d || Vmpages = %d\n", temp->rss, temp->vmpages);
	}
	printk(KERN_INFO "VmSize = %ld || Number of VmPages = %ld\n", vmpages*PAGE_SIZE/1024, vmpages);
	printk(KERN_INFO "RSS = %ld ||  Number of RSS = %ld\n", rss*PAGE_SIZE/1024, rss);
	return 0;
}

void __exit terminate(void)	{
	struct list_head *pos, *q;
	struct node *temp;
	list_for_each_safe(pos, q, &output.list) {
		temp = list_entry(pos, struct node, list);
		list_del(pos);
		kfree(temp);
	}
	printk(KERN_INFO "It's done !!!\n");
}

module_init(initialise);
module_exit(terminate);

