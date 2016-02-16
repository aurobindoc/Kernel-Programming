#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/slab.h>
#include <linux/sched.h>
#include <asm/pgtable.h>

#define mask(k,n) ((k) & ((1<<(n))-1))

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Aurobindo Mondal");

char *var;  // declaring variable whose memory is to be alocated

pte_t *getPTE(struct mm_struct *mm, unsigned long page)	{
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


int __init initialise(void)
{
	unsigned long virtAddr, pOffset, paMacro, paPagewalk;
	pte_t *pte;
    struct task_struct *task = current;

	var = kmalloc(8192, GFP_KERNEL);            // allocating 2 pages
	virtAddr = (unsigned long)var;              // virtual Address of allocated memory
	paMacro = slow_virt_to_phys(var);           // macro to get physical address from virtual address
	
	/***** Get physical address from PageWalk ********/
    pte = getPTE(task->mm, virtAddr);
	if(!pte) {
		printk(KERN_INFO "Address Unmapped!!!\n");
        //try to break the code from here if pte == NULL as it will give rise to Null Pointer Exception
	}
   
    pOffset = mask(virtAddr, 12);

	paPagewalk = (((phys_addr_t)pte_pfn(*pte) << PAGE_SHIFT) | pOffset );

    printk(KERN_INFO "Virtual Address : %lx\n", virtAddr);
	printk(KERN_INFO "Physical Address (using Macro)  : %lx\n", paMacro);
	printk(KERN_INFO "Physical Address (using Pagewalk) : %lx\n", paPagewalk);

	if(paMacro == paPagewalk)
	printk(KERN_INFO "Succes!! Both the physical addresses are same.\n");
	else
	printk(KERN_INFO "Failure!! Both the physical addresses are different.\n");

    return 0;
}
void __exit terminate(void) {
    kfree(var);
}

module_init(initialise);
module_exit(terminate);
