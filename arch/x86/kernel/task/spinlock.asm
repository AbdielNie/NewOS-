spin_lock:
	mov		eax,1			; Set the EAX register to 1.
	
	xchg	eax,[lock]		; Atomically swap the EAX register with
							; the lock variable.
							; This will always store 1 to the lock, leaving
							; previous value in the EAX register.

	test	eax,eax			; Test EAX with itself. Among other things, this will
							;  set the processor's Zero Flag if EAX is 0.
							; If EAX is 0, then the lock was unlocked and
							;  we just locked it.
							; Otherwise, EAX is 1 and we didn't acquire the lock.

	jnz		spin_lock		; Jump back to the MOV instruction if the Zero Flag is
							;  not set; the lock was previously locked, and so
							; we need to spin until it becomes unlocked.

	ret						; The lock has been acquired, return to the calling
							; function.