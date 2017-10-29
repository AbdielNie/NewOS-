

#ifndef SYSCALLS_H_
#define SYSCALLS_H_

/**ϵͳ�����жϺ�*/
#define SYSCALLS_INTERRUPT	50

/**������(ͨ�üĴ���)��ѹ���ջ��˳��*/
struct context
{
	long edi;		/**esp + 4*/
	long esi;		/**esp + 8*/
	long ebp;		/**esp + 12*/
	long esp;		/**esp + 16*/
	long ebx;		/**esp + 20*/
	long edx;		/**esp + 24*/
	long ecx;		/**esp + 28*/
	long eax;		/**esp + 32*/
};

/**����ϵͳ���ú�����λ��syscalls.asm��*/
void do_syscalls(struct context context);

#endif