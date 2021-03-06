#include <linux/mm.h>
#include <linux/hugetlb.h>
#include <linux/shm.h>
#include <linux/mman.h>
#include <linux/fs.h>
#include <linux/highmem.h>
#include <linux/security.h>
#include <linux/mempolicy.h>
#include <linux/personality.h>
#include <linux/syscalls.h>
#include <linux/swap.h>
#include <linux/swapops.h>
#include <linux/mmu_notifier.h>
#include <linux/migrate.h>
#include <linux/perf_event.h>
#include <asm/uaccess.h>
#include <asm/pgtable.h>
#include <asm/cacheflush.h>
#include <asm/tlbflush.h>

#ifndef pgprot_modify
static inline pgprot_t pgprot_modify(pgprot_t oldprot, pgprot_t newprot)
{
	printk( KERN_DEBUG "%s, %d\n", __FUNCTION__, __LINE__ );
	return newprot;
}
#endif

static void change_pte_range(struct mm_struct *mm, pmd_t *pmd,
		unsigned long addr, unsigned long end, pgprot_t newprot)
{
	pte_t *pte, oldpte;
	spinlock_t *ptl;

	pte = pte_offset_map_lock(mm, pmd, addr, &ptl);
	arch_enter_lazy_mmu_mode();
	do {
		printk( KERN_DEBUG "%s, %d\n", __FUNCTION__, __LINE__ );
		oldpte = *pte;
		if (pte_present(oldpte)) {
			pte_t ptent;

			printk( KERN_DEBUG "%s, %d\n", __FUNCTION__, __LINE__ );

			ptent = ptep_modify_prot_start(mm, addr, pte);
			ptent = pte_modify(ptent, newprot);
			ptep_modify_prot_commit(mm, addr, pte, ptent);
		}
	} while (pte++, addr += PAGE_SIZE, addr != end);
	arch_leave_lazy_mmu_mode();
	pte_unmap_unlock(pte - 1, ptl);
}

static inline void change_pmd_range(struct vm_area_struct *vma, pud_t *pud,
		unsigned long addr, unsigned long end, pgprot_t newprot)
{
	pmd_t *pmd;
	unsigned long next;

	pmd = pmd_offset(pud, addr);
	do {
		printk( KERN_DEBUG "%s, %d\n", __FUNCTION__, __LINE__ );
		next = pmd_addr_end(addr, end);
		if (pmd_none_or_clear_bad(pmd))
			continue;
		change_pte_range(vma->vm_mm, pmd, addr, next, newprot);
	} while (pmd++, addr = next, addr != end);
}

static inline void change_pud_range(struct vm_area_struct *vma, pgd_t *pgd,
		unsigned long addr, unsigned long end, pgprot_t newprot)
{
	pud_t *pud;
	unsigned long next;

	pud = pud_offset(pgd, addr);
	do {
		printk( KERN_DEBUG "%s, %d\n", __FUNCTION__, __LINE__ );
		next = pud_addr_end(addr, end);
		if (pud_none_or_clear_bad(pud))
			continue;
		change_pmd_range(vma, pud, addr, next, newprot);
	} while (pud++, addr = next, addr != end);
}

static void change_protection(struct vm_area_struct *vma,
		unsigned long addr, unsigned long end, pgprot_t newprot)
{
	struct mm_struct *mm = vma->vm_mm;
	pgd_t *pgd;
	unsigned long next;
	unsigned long start = addr;

	pgd = pgd_offset(mm, addr);
	flush_cache_range(vma, addr, end);
	do {
		printk( KERN_DEBUG "%s, %d\n", __FUNCTION__, __LINE__ );
		next = pgd_addr_end(addr, end);
		if (pgd_none_or_clear_bad(pgd))
			continue;
		change_pud_range(vma, pgd, addr, next, newprot);
	} while (pgd++, addr = next, addr != end);
	flush_tlb_range(vma, start, end);
}

static int
fixup(struct vm_area_struct *vma, struct vm_area_struct **pprev,
	unsigned long start, unsigned long end, unsigned long newflags)
{
	struct mm_struct *mm = vma->vm_mm;
	unsigned long oldflags = vma->vm_flags;
	unsigned long charged = 0;
	pgoff_t pgoff;
	int error;

	if (newflags == oldflags) {
		*pprev = vma;
		return 0;
	}

	/*
	 * First try to merge with previous and/or next vma.
	 */
	pgoff = vma->vm_pgoff + ((start - vma->vm_start) >> PAGE_SHIFT);
	*pprev = vma_merge(mm, *pprev, start, end, newflags,
			vma->anon_vma, vma->vm_file, pgoff, vma_policy(vma));
	if (*pprev) {
		vma = *pprev;
		goto success;
	}

	*pprev = vma;

	if (start != vma->vm_start) {
		error = split_vma(mm, vma, start, 1);
		if (error)
			goto fail;
	}

	if (end != vma->vm_end) {
		error = split_vma(mm, vma, end, 0);
		if (error)
			goto fail;
	}

success:
	/*
	 * vm_flags and vm_page_prot are protected by the mmap_sem
	 * held in write mode.
	 */
	vma->vm_flags = newflags;
	vma->vm_page_prot = pgprot_modify(vma->vm_page_prot,
					  vm_get_page_prot(newflags));

	mmu_notifier_invalidate_range_start(mm, start, end);
	change_protection(vma, start, end, vma->vm_page_prot);
	mmu_notifier_invalidate_range_end(mm, start, end);
	return 0;

fail:
	vm_unacct_memory(charged);
	return error;
}

asmlinkage long sys_csie_ncu_change_access( unsigned long start_address, unsigned long end_address, int operation ) {
	unsigned long vm_flags, nstart, tmp, reqprot;
	struct vm_area_struct *vma, *prev;
	int error = -EINVAL;

	start_address &= PAGE_MASK;
	end_address = PAGE_ALIGN( end_address );

	reqprot = ( operation ) ? ( PROT_READ | PROT_WRITE ) : PROT_READ;
	vm_flags = ( operation ) ? ( VM_READ | VM_WRITE ) : VM_READ;

	down_write(&current->mm->mmap_sem);

	vma = find_vma_prev(current->mm, start_address, &prev);
	error = -ENOMEM;
	if (!vma){
		printk( KERN_DEBUG "%s, %d\n", __FUNCTION__, __LINE__ );
		goto out;
	}

	if (vma->vm_start >= end_address){
		printk( KERN_DEBUG "%s, %d\n", __FUNCTION__, __LINE__ );
		goto out;
	}
	start_address = vma->vm_start;
	error = -EINVAL;
	if (!(vma->vm_flags & VM_GROWSDOWN)){
		printk( KERN_DEBUG "%s, %d\n", __FUNCTION__, __LINE__ );
		goto out;
	}

	if (start_address > vma->vm_start)
		prev = vma;

	for (nstart = start_address ; ; ) {
		unsigned long newflags;

		/* Here we know that  vma->vm_start <= nstart < vma->vm_end. */

		newflags = vm_flags | (vma->vm_flags & ~(VM_READ | VM_WRITE | VM_EXEC));

		tmp = vma->vm_end;
		if (tmp > end_address)
			tmp = end_address;
		error = fixup(vma, &prev, nstart, tmp, newflags);
		if (error){
			printk( KERN_DEBUG "%s, %d\n", __FUNCTION__, __LINE__ );
			goto out;
		}
		nstart = tmp;

		if (nstart < prev->vm_end)
			nstart = prev->vm_end;
		if (nstart >= end_address){
			printk( KERN_DEBUG "%s, %d\n", __FUNCTION__, __LINE__ );
			goto out;
		}

		vma = prev->vm_next;
		if (!vma || vma->vm_start != nstart) {
			error = -ENOMEM;
			printk( KERN_DEBUG "%s, %d\n", __FUNCTION__, __LINE__ );
			goto out;
		}
	}
out:
	up_write(&current->mm->mmap_sem);
	printk( KERN_DEBUG "%s, %d\n", __FUNCTION__, __LINE__ );
	return error;
}
