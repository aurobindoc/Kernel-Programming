#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/slab.h>
#include <linux/sched.h>
#include <asm/pgtable.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Aurobindo Mondal");

struct node {	 // declaring struct var whose memory is to be allocated
	char buffer[256];
};
struct node *var;

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


int __init initialise(void)
{
	unsigned long virtAddr, pOffset, paMacro, paPagewalk, phyAddr;
	pte_t *pte;
	enum pg_level level;
    struct task_struct *task = current;

	var = vmalloc(8192);            // allocating 2 pages
	strcpy(var->buffer,"Hey! this is vmalloc()");
	printk(KERN_ALERT "%s\n", var->buffer);
	virtAddr = (unsigned long)var;              // virtual Address of allocated memory
	paMacro = virt_to_phys(var);           // macro to get physical address from virtual address
	
	/***** Get physical address from PageWalk ********/
    pte = getPTE(task->mm, virtAddr, &level);	// returns the pte and the level of page table till which it traversed
	if(!pte) {
		printk(KERN_INFO "Address Unmapped!!!\n");
		return 0;
	}
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

	if(paMacro == paPagewalk)
	printk(KERN_INFO "Succes!! Both the physical addresses are same.\n");
	else
	printk(KERN_INFO "Failure!! Both the physical addresses are different.\n");

    return 0;
}
void __exit terminate(void) {
    vfree(var);
}

module_init(initialise);
module_exit(terminate);
