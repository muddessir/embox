/**
 * @file
 *
 * @brief Describes thread descriptor structure
 *
 * @date 23.03.11
 * @author Alina Kramar
 */

#ifndef KERNEL_THREAD_TYPES_H_
#define KERNEL_THREAD_TYPES_H_

#include <hal/context.h>
#include <hal/cpu.h>

#include <kernel/spinlock.h>
#include <kernel/sched/sched_strategy.h>
#include <kernel/thread/signal.h>
#include <kernel/thread/thread_stack.h>
#include <kernel/thread/thread_local.h>
#include <kernel/thread/thread_cancel.h>

#include <util/dlist.h>

#ifdef SMP
# define TW_SMP_WAKING    (-1)  /**< In the middle of sched_wakeup. */
#else
# define TW_SMP_WAKING      0   /* Not used in non-SMP kernel. */
#endif

/* Resource mgmt flags. */
#define __THREAD_STATE_EXITED    (0x1 << 3)
#define __THREAD_STATE_DETACHED  (0x1 << 4)

#define THREAD_STATE_INIT 0 // XXX

typedef int __thread_id_t;

struct task;

/**
 * Thread control block.
 *
 * Locking:
 *   t->lock    - used during waking up
 *   t->active  - (SMP) only current is allowed to modify it,
 *                reads are usually paired with t->waiting
 *   t->ready   - any access must be protected with rq lock and interrupts off,
 *                only current can reset it to zero (during 'schedule'),
 *                others can set it to a non-zero during wake up
 *   t->waiting - current can change it from zero to a non-zero with no locks,
 *                others access it with t->lock held and interrupts off
 */
struct thread {
	spinlock_t         lock;

	unsigned int       active;       /**< Running on a CPU. TODO SMP-only. */
	unsigned int       ready;        /**< Managed by the scheduler. */
	unsigned int       waiting;      /**< Waiting for an event. */

	unsigned int       state;        /**< XXX. resource mgmt, needs clean up */

	struct context     context;      /**< Architecture-dependent CPU state. */

	void            *(*run)(void *); /**< Start routine. */
	void              *run_arg;      /**< Argument to pass to start routine. */
	union {
		void          *run_ret;      /**< Return value of the routine. */
		void          *joining;      /**< A joining thread (if any). */
	} /* unnamed */;

	thread_stack_t     stack;        /**< Handler for work with thread stack */

	__thread_id_t      id;           /**< Unique identifier. */

	struct task       *task;         /**< Task belong to. */
	struct dlist_head  thread_link;  /**< list's link holding task threads. */

	struct sigstate    sigstate;     /**< Pending signal(s). */

	struct sched_attr  sched_attr;   /**< Scheduler-private data. */
	int                policy;       /**< Scheduling policy*/

	thread_local_t     local;
	thread_cancel_t    cleanups;
};

#endif /* KERNEL_THREAD_TYPES_H_ */
