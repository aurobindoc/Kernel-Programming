#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/moduleparam.h>
#include <linux/init.h>
#include <linux/stat.h>
#include <linux/slab.h>
#include <linux/sched.h>
#include <asm/pgtable.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Aurobindo Mondal");

static unsigned long va;
module_param(va, ulong, 0);
MODULE_PARM_DESC(va, "Virtual Address");


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
  	if(pte && pte_present(*pte))	return pte;

	return NULL;
}

int __init initialise(void)	{
	unsigned long virtAddr, pOffset, paMacro, paPagewalk, phyAddr;
	pte_t *pte;
	enum pg_level level;
    struct task_struct *task = current;
	virtAddr = (unsigned long)va;
	char *var = (char *) virtAddr;
	printk(KERN_INFO "%s\n",var);
	
	paMacro = virt_to_phys(virtAddr);  
	
	pte = getPTE(task->mm, virtAddr, &level);	// returns the pte and the level of page table till which it traversed
	if(pte) {
		switch(level)	{
		case PG_LEVEL_1G :
			phyAddr = pud_pfn(*(pud_t *)pte)<<PAGE_SHIFT;
			pOffset = virtAddr & ~PUD_PAGE_MASK;
		case PG_LEVEL_2M :
			phyAddr = pmd_pfn(*(pmd_t *)pte)<<PAGE_SHIFT;
			pOffset = virtAddr & ~PMD_PAGE_MASK;
    	default :
			phyAddr = pte_pfn(*pte)<<PAGE_SHIFT;
			pOffset = virtAddr & ~PAGE_MASK;
		}
		paPagewalk = ((phys_addr_t)(phyAddr | pOffset ));
		printk(KERN_INFO "Virtual Address : %lx\n", virtAddr);
		printk(KERN_INFO "Physical Address (using Macro)  : %lx\n", paMacro);
		printk(KERN_INFO "Physical Address (using Pagewalk) : %lx\n", paPagewalk);
	}
	else {
		printk(KERN_INFO "Page Unmapped\n");
	}
	return 0;
}

void __exit terminate(void) {
    printk(KERN_INFO "Checking done!!!\n");
}

module_init(initialise);
module_exit(terminate);
