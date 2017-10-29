
#ifndef SPIN_LOCK_H_

	typedef unsigned long spinlock_t;

	#define SPIN_LOCK_LOCKED	0
	#define SPIN_LOCK_UNLOCKED	1
	
	void spin_lock_init(spinlock_t *lock);
	
	void spin_lock(spinlock_t *lock);
	void spin_lock_irqsave(spinlock_t *lock, unsigned long flags);
	void spin_lock_irq(spinlock_t *lock);
	void spin_lock_bh(spinlock_t *lock);

	void spin_unlock(spinlock_t *lock);
	void spin_unlock_irqrestore(spinlock_t *lock, unsigned long flags);
	void spin_unlock_irq(spinlock_t *lock);
	void spin_unlock_bh(spinlock_t *lock);

#define SPIN_LOCK_H_
#endif