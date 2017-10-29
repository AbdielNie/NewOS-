
#include <memory.h>
#include <lib/mem.h>
#include <lib/string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <spinlock.h>
#include <task.h>

/**允许调度标志*/
bool schedule_flag = false;

/**临时堆栈指针存放变量*/
unsigned long temp_stack;

/**
 * 任务优先级数组
 * 每个元素都是指向任务链表的指针
 */
#define num_plan	4
static union task *task_plan[num_plan] =
{
	NULL, NULL, NULL, NULL
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
	task_0.info.counter = 1000;
	
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
	
	// schedule_flag = true;
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
			printk("task priority:%d, now running in plan%d\n", task_ptr->info.priority, task_ptr->info.plan);
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
}

/**执行应用程序函数（非POSIX标准）*/
int run_exec(void *arg)
{
}

/**运行任务函数*/
union task* run(char *filename, char *para, int flag)
{
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
	finish:goto finish;
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
volatile int schedule(void)
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
			printk("%#X", &task_plan[plan_ptr]->info);
		}
	}
	
	fin:goto fin;
}


/**任务挂起函数*/
void sleep(void)
{
}

/**任务唤醒函数*/
void wakeup(tid target)
{
}