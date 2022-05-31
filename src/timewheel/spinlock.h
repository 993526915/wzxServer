#ifndef _SPINLOCK_H
#define _SPINLOCK_H

struct spinlock {
	int lock;
};

static void spinlock_init(struct spinlock *lock) {
	lock->lock = 0;
}

static void spinlock_lock(struct spinlock *lock) {
	while (__sync_lock_test_and_set(&lock->lock, 1)) {}
}

static int spinlock_trylock(struct spinlock *lock) {
	return __sync_lock_test_and_set(&lock->lock, 1) == 0;
}

static void spinlock_unlock(struct spinlock *lock) {
	__sync_lock_release(&lock->lock);
}

static void spinlock_destroy(struct spinlock *lock) {
	(void) lock;
}

#endif
