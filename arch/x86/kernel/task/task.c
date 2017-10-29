
#include <memory.h>
#include <lib/mem.h>
#include <lib/string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <spinlock.h>
#include <task.h>

/**允许调度标志*/
static bool schedule_flag = false;

/**临时堆栈指针存放变量*/
unsigned long temp_stack;

/**
 * 任务优先级数组
 * 每个元素都是指向任务链表的指针
 */
#define num_plan	10
static union task *task_plan[num_plan] =
{
	NULL
};

/**任务0的联合体定义，同时也是ID双向链表*/
union task task_0;

/**进程0*/
struct process_struct *process_0;

/**初始化任务管理*/
void init_task(void)
{
	/**对0任务的数据结构进行赋值*/
	task_0.info.runtime = 0;		/**0任务总共运行时间归0*/
	task_0.info.priority = TASK_NORMAL;	/**设置0任务为正常优先级任务*/
	
	/**将0任务做成双向循环链表*/
	task_0.info.before = NULL;
	task_0.info.after = NULL;
	task_0.info.id = 0;
	
	/**指定当前运行任务是0任务*/
	current = &task_0;
	
	/**分配时间片*/
	task_0.info.counter = 100;
	
	/**分配进程0结构体*/
	for (process_0 = NULL; process_0 == NULL; )
		process_0 = kmalloc(sizeof(struct process_struct), 0);
	
	/**清空进程0结构体*/
	memset(process_0, 0, sizeof(struct process_struct));
	
	/**将0任务升级成0进程*/
	task_0.info.pptr = process_0;
	
	/**0进程赋值*/
	process_0->cr3 = read_CR3();
	process_0->msg_list = NULL;
	process_0->nthread ++;
	
	schedule_flag = true;
}

/**任务测试——显示任务信息*/
int print_task(tid id)
{
	unsigned long n;
	union task *task_ptr;
	
	for (task_ptr = &task_0; ; task_ptr = task_ptr->info.after)
	{
		if (task_ptr->info.id == id)
		{
			printk("found this task:\n");
			printk("task ID:%d, father:%d\n", task_ptr->info.id, task_ptr->info.father);
			printk("task stack:%#x\n", task_ptr->info.stack);
			printk("task counter:%d, running time in total:%dms\n", task_ptr->info.counter, task_ptr->info.runtime);
			printk("task priority:%d, now running in plan(%d)\n", task_ptr->info.priority, task_ptr->info.plan);
			return 0;
		}
		
		if (task_ptr->info.after == NULL)
		{
			printk("not found.\n");
			return -1;
		}
	}
}

/**创建内核级线程函数*/
tid new_task(int (*function)(void *), void *argument)
{
	return;
	/**禁止切换任务*/
	disable_schedule();
	
	/**为新任务的数据结构分配空间*/
	union task *new_task;
	new_task = vmalloc(sizeof(union task));
	if (new_task == NULL) return 0;
	memset(new_task, 0, sizeof(union task));
	
	/**将新任务插入到任务ID双向链表中*/
	union task *tsk_ptr;
	for (tsk_ptr = &task_0; tsk_ptr = new_task->info.after; tsk_ptr->info.after != NULL);
	tsk_ptr->info.after = new_task;
	new_task->info.before = tsk_ptr;
	new_task->info.after = NULL;
	new_task->info.id = tsk_ptr->info.father + 1;
	
	/**对新任务的数据结构进行赋值*/
	new_task->info.father = current->info.id;	/**设置父任务指针*/
	new_task->info.counter = 30;				/**时间片从30开始计时*/
	new_task->info.runtime = 0;					/**新任务总共执行时间归0*/
	new_task->info.state = TASK_HIGH;			/**设置新任务为高优先级任务*/
	new_task->info.pptr = current->info.pptr;	/**新任务同样是本进程的一个线程*/
	new_task->info.pptr->nthread ++;			/**本进程的线程数量加一*/
	
	/**对新任务的堆栈进行初始化*/
	new_task->info.stack = Init_Kernel_Task(((unsigned long)new_task + TASK_SIZE), function, argument);
	
	/**设立新任务为正常优先级*/
	new_task->info.plan = TASK_NORMAL;
	
	/**将新任务添加到实时优先级数组中*/
	if (task_plan[TASK_HIGH] != NULL)
	{
		new_task->info.next = task_plan[TASK_HIGH];
		new_task->info.prev = task_plan[TASK_HIGH]->info.prev;
		task_plan[TASK_HIGH]->info.prev->info.next = new_task;
		task_plan[TASK_HIGH]->info.prev = new_task;
	}else{
		task_plan[TASK_HIGH] = new_task;
		new_task->info.next = new_task;
		new_task->info.prev = new_task;
	}
	
finish:
	/**允许切换任务*/
	enable_schedule();
	
	/**返回*/
	return new_task->info.id;
}

/**执行应用程序函数（非POSIX标准）*/
int run_exec(void *arg)
{
	return;
	struct GBOS_head *head;
	struct exec_file_para *file_para = arg;
	struct process_struct *new_process;
	/**停止调度*/
	disable_schedule();
	
	/**分配进程结构体*/
	for (new_process = NULL; new_process == NULL; )
		new_process = kmalloc(sizeof(struct process_struct), 0);
	
	/**清空进程结构体*/
	memset(new_process, 0, sizeof(struct process_struct));
	
	/**将当前任务与当前进程脱钩*/
	current->info.pptr->nthread --;
	
	/**将任务升级成进程*/
	current->info.pptr = new_process;
	
	/**进程赋值*/
	new_process->cr3 = new_pdt();
	new_process->msg_list = NULL;
	new_process->nthread ++;

	/**切换页目录表*/
	write_CR3(new_process->cr3);
	
	/**允许调度*/
	enable_schedule();
	
	/**加载文件*/
	file_open(file_para->filename, (void *)0x10000000);
	
	/**回收结构体*/
	kfree(arg);
	
	/**对准文件头*/
	head = (void *)0x10000000;
	
	/**检查是否是有效应用程序*/
	if (head->flag[0] != 'G' |
		head->flag[1] != 'B' |
		head->flag[2] != 'O' |
		head->flag[3] != 'S' )
	{
		/**失败返回*/
		return 0;
	}
	
	/**执行应用程序*/
	head->entry();
}

/**运行任务函数*/
union task* run(char *filename, char *para, int flag)
{
	return;
	struct exec_file_para *new_arg;
	
	/**至少filename字符串指针不能为NULL*/
	if (filename == NULL) return NULL;
	
	/**分配一个exec_file_para结构体装载执行run_exec线程的参数*/
	for (new_arg = NULL; new_arg == NULL; )
		new_arg = kmalloc(sizeof(struct exec_file_para), 0);
	
	/**清空exec_file_para结构体防止造成干扰*/
	memset(new_arg, 0, sizeof(struct exec_file_para));
	
	/**拷贝文件名信息进入exec_file_para中*/
	strncpy(new_arg->filename, filename, 256);
	
	/**如果有参数则同时拷贝参数*/
	if (new_arg->para != NULL)
		strncpy(new_arg->para, para, 256);
	
	/**创建新线程运行run_exec*/
	new_task(&run_exec, new_arg);
}

/**获得任务的id*/
tid get_id(void)
{
	return current->info.id;
}

/**获得父任务的id*/
tid get_father(void)
{
	return current->info.father;
}


/**退出任务*/
void exit(int code)
{
	goto finish;
	/**指向自身的结构体*/
	union task *my = current;
	
	/**将自己从任务链表中剥离*/
	my->info.next->info.prev = my->info.prev;
	my->info.prev->info.next = my->info.next;
	
	/**启动调度函数*/
	schedule();
	
finish:
	goto finish;
}

/**允许调度函数*/
void enable_schedule(void)
{
	schedule_flag = true;
}

/**禁止调度函数*/
void disable_schedule(void)
{
	schedule_flag = false;
}

/**切换到目标任务*/
extern void switch_to(union task* id);

/**任务切换函数，使用算法选择下个任务并加载*/
void schedule(void)
{
	union task *new_task;
	unsigned long plan_ptr,a;
	
	/**判断是否允许调度*/
	if (schedule_flag == false) return;
	
	/**重新确定当前任务实时优先级*/
	current->info.plan ++;
	if (current->info.plan >= num_plan)
	{
		current->info.plan = num_plan - 1;
	}
	
	/**如果当前任务所属的实时优先级已经包含任务*/
	if (task_plan[current->info.plan] != NULL)
	{
		current->info.next = task_plan[current->info.plan];
		current->info.prev = task_plan[current->info.plan]->info.prev;
		task_plan[current->info.plan]->info.prev->info.next = current;
		task_plan[current->info.plan]->info.prev = current;
	
	/**如果当前任务所属的实时优先级没有包含任务*/
	}else{
		/**组成双向循环链表*/
		current->info.next = current;
		current->info.prev = current;
		
		/**加入优先级表中*/
		task_plan[current->info.plan] = current;
	}
	
	/**选择下一个要运行的进程*/
	for (plan_ptr = 0; plan_ptr < num_plan; plan_ptr ++)
	{
		if (task_plan[plan_ptr] != NULL)
		{
			// print_task(task_plan[plan_ptr]->info.id);
			/**将这个任务指定为新任务*/
			new_task = task_plan[plan_ptr];
			/**当该优先级的链表中只有一个任务时*/
			if (task_plan[plan_ptr]->info.prev == task_plan[plan_ptr])
			{
				/**将这个任务脱离双向循环链表*/
				task_plan[plan_ptr] = NULL;
			}else{
				/**将这个任务脱离双向循环链表*/
				task_plan[plan_ptr]->info.prev->info.next = task_plan[plan_ptr]->info.next;
				task_plan[plan_ptr]->info.next->info.prev = task_plan[plan_ptr]->info.prev;
			}
			
			/**退出循环*/
			break;
		}
	}
	
	/**分配时间片*/
	new_task->info.counter = 30;
	
	/**切换页目录表*/
	write_CR3(new_task->info.pptr->cr3);
	
	/**切换至新任务*/
	switch_to(new_task);
}


/**任务挂起函数*/
void sleep(void)
{
	return;
	/**脱离链表*/
	// current->info.next->info.prev = current->info.prev;
	// current->info.prev->info.next = current->info.next;
	
	/**设置睡眠属性*/
	// current->info.state = TASK_SLEEP;
	
	/**调度任务*/
	// schedule();
}

/**任务唤醒函数*/
void wakeup(tid target)
{
	return;
	/**判断该任务是否已经挂起*/
	// if (target->info.state == TASK_SLEEP)
	// {
		// /**刚刚唤醒的任务加入正常队列*/
		// target->info.state = TASK_HIGH;
		
		// /**加入队列中*/
		// target->info.prev = current;
		// target->info.next = current->info.next;
		// current->info.next = target;
		// target->info.next->info.prev = target;
	// }
	
	/**正常返回*/
	return;
}