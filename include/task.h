
#ifndef TASK_H_
#define TASK_H_

#include <stddef.h>

union task *current;/*当前任务*/

/**进程结构体*/
struct process_struct
{
	unsigned long nthread;				/**线程数量*/
	unsigned long cr3;					/**进程的页目录表地址*/
	struct msg *msg_list;				/**消息链表*/
};
	
/**任务0的联合体定义*/
union task task_0;

/**任务ID定义*/
typedef unsigned long tid;

/**任务信息结构*/
struct task_info{
	union task *before, *after;			/**双向循环链表指向前一个ID的任务和后一个ID的任务*/
	union task *next, *prev;			/**同优先级之间组成双向循环链表*/
	unsigned long stack;				/**堆栈指针*/
	tid id;								/**本任务的ID*/
	tid father;							/**父任务ID*/
	struct process_struct *pptr;		/**进程结构指针*/
	unsigned long counter;				/**时间片计时*/
	unsigned long runtime;				/**运行时间*/
	unsigned long priority;				/**标准优先级，创建任务时确定，后来可以更改*/
	unsigned long plan;					/**当前属于哪一优先级*/
	int state;							/**任务的属性*/
};

/**优先级标准情况*/
#define TASK_HIGH		0			// 实时任务
#define TASK_NORMAL		1			// 普通任务

/**任务联合体的大小*/
#define TASK_SIZE	8192

/**任务联合体*/
union task
{
	struct task_info info;
	unsigned long stack[TASK_SIZE / sizeof(unsigned long)];
};

/**Ghost Bird 应用程序头部结构体*/
#define GBOS_FLAG_MAX	4
struct GBOS_head
{
	char flag[GBOS_FLAG_MAX];
	int (*entry)(void);
};

/**队列管理函数*/
void init_queue(union task *queue);
void add_queue(union task *queue, union task *task);

extern unsigned long init_Kernel_Task(unsigned long stack, int (*function)(void *), void * argument);/*位于\arch\x86\kernel\task\init_stack.asm文件中*/

/**初始化任务函数*/
void init_task(void);
	
/**创建内核任务线程函数*/
tid new_task(int (*function)(void *), void *argument);

/**执行文件名及参数*/
struct exec_file_para
{
	char filename[256];
	char para[256];
};

/**执行应用程序函数（非POSIX标准）*/
int run_exec(void *arg);

/**运行任务函数*/
union task* run(char *filename, char *para, int flag);

/**获得当前任务的id*/
tid get_id(void);

/**获得父任务的id*/
tid get_father(void);
	
/**任务结束函数*/
void exit(int code);

/**允许调度函数*/
void enable_schedule(void);

/**禁止调度函数*/
void disable_schedule(void);

/**任务切换函数，使用算法选择下个任务并加载*/
void schedule(void);

/**任务挂起函数*/
void sleep(void);
	
/**任务唤醒函数*/
void wakeup(tid target);
	
/**exit other task*/
#define	SUCCESS		0
#define	FAIL		-1
long do_exit(union task* id);

#endif
