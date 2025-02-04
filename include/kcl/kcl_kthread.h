/* SPDX-License-Identifier: MIT */
#ifndef AMDKCL_KTHREAD_H
#define AMDKCL_KTHREAD_H

#include <linux/sched.h>
#include <linux/kthread.h>
#ifndef HAVE_KTHREAD_USE_MM
#include <linux/mmu_context.h>
#endif

#if !defined(HAVE___KTHREAD_SHOULD_PATK)
extern bool __kcl_kthread_should_park(struct task_struct *k);
#endif

#if !defined(HAVE_KTHREAD_PARK_XX)
extern void (*_kcl_kthread_parkme)(void);
extern void (*_kcl_kthread_unpark)(struct task_struct *k);
extern int (*_kcl_kthread_park)(struct task_struct *k);
extern bool (*_kcl_kthread_should_park)(void);
#endif

#ifndef HAVE_KTHREAD_USE_MM
static inline
void kthread_use_mm(struct mm_struct *mm)
{
	use_mm(mm);
}
static inline
void kthread_unuse_mm(struct mm_struct *mm)
{
	unuse_mm(mm);
}
#endif

#endif /* AMDKCL_KTHREAD_H */
