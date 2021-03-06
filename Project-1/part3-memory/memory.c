#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/moduleparam.h>
#include <linux/init.h>

#include <linux/list.h>
#include <linux/sched.h>
#include <linux/mm.h>
#include <linux/slab.h>
#include <linux/list.h>  /* Needed for linked list implementation */

#include "../arch/x86/include/asm/pgtable.h"
#include <asm/io.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Aurobindo Mondal");


struct vmregion {
	struct list_head list;
	unsigned long start;
	unsigned long end;
	char use[32];
};
struct vmregion vma;

struct kernelVM {
	struct list_head list;
	unsigned long start;
	unsigned long end;
	long size;			//in MB
	char use[32];
	long cntPresent;
	long cntAbsent;
	long cntUnmapped;
};
struct kernelVM vmKernel;

struct output {
 	struct list_head list;
    unsigned long va;
    unsigned long pa;
};
struct output op;

void insertVMA(unsigned long start, unsigned long end, char *use)	{
	struct vmregion *temp;
	temp = (struct vmregion *) kmalloc(sizeof(struct vmregion), GFP_KERNEL);
	temp->start = start;
	temp->end = end;
	strcpy(temp->use, use);
	list_add_tail(&(temp->list), &(vma.list));
}

void insertOutput(unsigned long va, unsigned long pa)	{
	struct output *temp;
	temp = (struct output *) kmalloc(sizeof(struct output), GFP_KERNEL);
	temp->va = va;
	temp->pa = pa;
	list_add_tail(&(temp->list), &(op.list));
}


pte_t *getPTE(struct mm_struct *mm, unsigned long page, unsigned int *level)	{
	pgd_t *pgd;
	pud_t *pud;
	pmd_t *pmd;
	pte_t *pte;

	*level = PG_LEVEL_NONE;
	pgd = pgd_offset(mm, page);
	if(pgd_none(*pgd))	return NULL;
	pud = pud_offset(pgd, page);
	if (pud_none(*pud))	return NULL;

    *level = PG_LEVEL_1G;
    if (pud_large(*pud) || !pud_present(*pud))
    	return (pte_t *)pud;
	pmd = pmd_offset(pud, page);
    if (pmd_none(*pmd))
    	return NULL;

	*level = PG_LEVEL_2M;
    if(pmd_large(*pmd) || !pmd_present(*pmd))
    	return (pte_t *)pmd;
	
	*level = PG_LEVEL_4K;
	pte = pte_offset_kernel(pmd, page);

	return pte;
}

int __init initialise(void)	{

	unsigned long virtAddr, physAddr, pOffset, pa;
	long pres=0, mapp=0, unmapp=0;
	struct task_struct *task = current;
	enum pg_level level;
	pte_t *pte;
	struct vmregion *temp;
	struct kernelVM *kern;
	unsigned long s = (unsigned long)0xffffffff80000000, e =(unsigned long)0xffffffffa0000000;
	unsigned long s1 = (unsigned long)0xffffff0000000000, e1 =(unsigned long)0xffffff7fffffffff;
	/******* initialise vma with virtual adresses ********/

	INIT_LIST_HEAD(&vma.list);
	/*---------- Excluding holes & unused holes ---------*/
	insertVMA(0xffffffff80000000, 0xffffffffa0000000, "Kernel Text Mapping");
	insertVMA(0xffff800000000000, 0xffff87ffffffffff, "Hypervisor");
	insertVMA(0xffff880000000000, 0xffffc7ffffffffff, "Direct mapping");
	insertVMA(0xffffc90000000000, 0xffffe8ffffffffff, "vmalloc");
	insertVMA(0xffffea0000000000, 0xffffeaffffffffff, "Virtual Memory map");
	insertVMA(0xffffec0000000000, 0xfffffc0000000000, "Kasan shadow memory");
	insertVMA(0xffffff0000000000, 0xffffff7fffffffff, "esp fixup stacks");
	insertVMA(0xffffffffa0000000, 0xffffffffff5fffff, "Module mapping space");
	insertVMA(0xffffffffff600000, 0xffffffffffdfffff, "vsyscalls");
	/*---------------------------------------------------*/
	

	INIT_LIST_HEAD(&vmKernel.list);
	INIT_LIST_HEAD(&op.list);
	

	list_for_each_entry(temp, &vma.list, list) {
		pres = 0;
        mapp = 0;
        unmapp = 0;
        for(virtAddr = temp->start; virtAddr < temp->end; virtAddr += PAGE_SIZE)	{
			
			pte = getPTE(task->mm, virtAddr, &level);	// returns the pte and the pgtable level
            if(pte==NULL)	{	//Page not mapped
				unmapp++;
			}
            else    {			//Pages mapped
	        	if(pte && pte_present(*pte)) {			//Pages present in RAM
					pres++;
                    if(virtAddr >= s && virtAddr <= e)	{	//print virtAddr & physAddr for a particular region
						switch(level)	{
						case PG_LEVEL_1G :
							pa = pud_pfn(*(pud_t *)pte)<<PAGE_SHIFT;
							pOffset = virtAddr & ~PUD_PAGE_MASK;
						case PG_LEVEL_2M :
							pa = pmd_pfn(*(pmd_t *)pte)<<PAGE_SHIFT;
							pOffset = virtAddr & ~PMD_PAGE_MASK;
						default :
							pa = pte_pfn(*pte)<<PAGE_SHIFT;
							pOffset = virtAddr & ~PAGE_MASK;
						}
						physAddr = ((phys_addr_t)(pa | pOffset));
						insertOutput(virtAddr, physAddr);	
						//printk(KERN_INFO "%lx || %lx \n", virtAddr, physAddr );
					}
				}
            	else	{								//Pages not present in RAM but mapped
					mapp++;
					if(!(virtAddr >= s1 && virtAddr <= e1))	break;	// specially for stack area
				}
        	}
			/******** Storing VMRegion Information *******/
			
		}
		//schedule();
		kern = (struct kernelVM *) kmalloc(sizeof(struct kernelVM), GFP_ATOMIC);
		kern->start = temp->start;
		kern->end = temp->end;
		strcpy(kern->use, temp->use);
		kern->cntPresent = pres;
		kern->cntAbsent = mapp;
		kern->cntUnmapped = unmapp;
		kern->size = (temp->end - temp->start)/(1024*1024);		
		list_add_tail(&(kern->list), &(vmKernel.list));
	}
	return 0;
}

void __exit terminate(void)	{
	struct list_head *pos, *q;
	struct kernelVM *kern;
	struct vmregion *vmr;
    struct output *out;
	
    list_for_each_entry(out, &op.list, list) {
        printk(KERN_INFO "Virtual Address : %lx  &  Physical Address : %lx\n", out->va, out->pa);
    }

	list_for_each_entry(kern, &vmKernel.list, list) {
		printk(KERN_INFO "Region Name : %s \n",kern->use);
		printk(KERN_INFO "\tStarting Virtual Address : %lx\n",kern->start);
		printk(KERN_INFO "\tEnding Virtual Address : %lx\n",kern->end);
		printk(KERN_INFO "\tTotal Size : %ld MB\n",kern->size);
		printk(KERN_INFO "\tNumber of Pages MAPPED & PRESENT in RAM : %ld\n",kern->cntPresent);
		printk(KERN_INFO "\tNumber of Pages MAPPED & NOT PRESENT in RAM : %ld\n",kern->cntAbsent);		
		printk(KERN_INFO "\tNumber of Pages UNMAPPED : %ld\n",kern->cntUnmapped);
		printk(KERN_INFO "\n");
	}
	
	list_for_each_safe(pos, q, &vma.list) {
		vmr = list_entry(pos, struct vmregion, list);
		list_del(pos);
		kfree(vmr);
	}
	list_for_each_safe(pos, q, &vmKernel.list) {
		kern = list_entry(pos, struct kernelVM, list);
		list_del(pos);
		kfree(kern);
	}
    list_for_each_safe(pos, q, &op.list) {
        out = list_entry(pos, struct output, list);
        list_del(pos);
        kfree(out);
    }

    printk(KERN_INFO "Exiting Module!!! Good Bye!!!\n");
}

module_init(initialise);
module_exit(terminate);
