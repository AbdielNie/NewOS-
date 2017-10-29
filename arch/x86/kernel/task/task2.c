
#include <memory.h>
#include <lib/mem.h>
#include <lib/string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <spinlock.h>
#include <task.h>

/**������ȱ�־*/
bool schedule_flag = false;

/**��ʱ��ջָ���ű���*/
unsigned long temp_stack;

/**
 * �������ȼ�����
 * ÿ��Ԫ�ض���ָ�����������ָ��
 */
#define num_plan	4
static union task *task_plan[num_plan] =
{
	NULL, NULL, NULL, NULL
};

/**����0�������嶨�壬ͬʱҲ��ID˫������*/
union task task_0;

/**����0*/
struct process_struct *process_0;

/**��ʼ���������*/
void init_task(void)
{
	/**��0��������ݽṹ���и�ֵ*/
	task_0.info.runtime = 0;		/**0�����ܹ�����ʱ���0*/
	task_0.info.priority = TASK_NORMAL;	/**����0����Ϊ�������ȼ�����*/
	
	/**��0��������˫��ѭ������*/
	task_0.info.before = NULL;
	task_0.info.after = NULL;
	task_0.info.id = 0;
	
	/**ָ����ǰ����������0����*/
	current = &task_0;
	
	/**����ʱ��Ƭ*/
	task_0.info.counter = 1000;
	
	/**�������0�ṹ��*/
	for (process_0 = NULL; process_0 == NULL; )
		process_0 = kmalloc(sizeof(struct process_struct), 0);
	
	/**��ս���0�ṹ��*/
	memset(process_0, 0, sizeof(struct process_struct));
	
	/**��0����������0����*/
	task_0.info.pptr = process_0;
	
	/**0���̸�ֵ*/
	process_0->cr3 = read_CR3();
	process_0->msg_list = NULL;
	process_0->nthread ++;
	
	// schedule_flag = true;
}

/**������ԡ�����ʾ������Ϣ*/
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

/**�����ں˼��̺߳���*/
tid new_task(int (*function)(void *), void *argument)
{
}

/**ִ��Ӧ�ó���������POSIX��׼��*/
int run_exec(void *arg)
{
}

/**����������*/
union task* run(char *filename, char *para, int flag)
{
}

/**��������id*/
tid get_id(void)
{
	return current->info.id;
}

/**��ø������id*/
tid get_father(void)
{
	return current->info.father;
}


/**�˳�����*/
void exit(int code)
{
	finish:goto finish;
}

/**������Ⱥ���*/
void enable_schedule(void)
{
	schedule_flag = true;
}

/**��ֹ���Ⱥ���*/
void disable_schedule(void)
{
	schedule_flag = false;
}

/**�л���Ŀ������*/
extern void switch_to(union task* id);

/**�����л�������ʹ���㷨ѡ���¸����񲢼���*/
volatile int schedule(void)
{
	union task *new_task;
	unsigned long plan_ptr,a;
	
	/**�ж��Ƿ��������*/
	if (schedule_flag == false) return;
	
	/**����ȷ����ǰ����ʵʱ���ȼ�*/
	current->info.plan ++;
	if (current->info.plan >= num_plan)
	{
		current->info.plan = num_plan - 1;
	}
	
	/**�����ǰ����������ʵʱ���ȼ��Ѿ���������*/
	if (task_plan[current->info.plan] != NULL)
	{
		current->info.next = task_plan[current->info.plan];
		current->info.prev = task_plan[current->info.plan]->info.prev;
		task_plan[current->info.plan]->info.prev->info.next = current;
		task_plan[current->info.plan]->info.prev = current;
	
	/**�����ǰ����������ʵʱ���ȼ�û�а�������*/
	}else{
		/**���˫��ѭ������*/
		current->info.next = current;
		current->info.prev = current;
		
		/**�������ȼ�����*/
		task_plan[current->info.plan] = current;
	}
	
	/**ѡ����һ��Ҫ���еĽ���*/
	for (plan_ptr = 0; plan_ptr < num_plan; plan_ptr ++)
	{
		if (task_plan[plan_ptr] != NULL)
		{
			printk("%#X", &task_plan[plan_ptr]->info);
		}
	}
	
	fin:goto fin;
}


/**���������*/
void sleep(void)
{
}

/**�����Ѻ���*/
void wakeup(tid target)
{
}