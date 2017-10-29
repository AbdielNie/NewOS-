
#include <memory.h>
#include <lib/mem.h>
#include <lib/string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <spinlock.h>
#include <task.h>

/**������ȱ�־*/
static bool schedule_flag = false;

/**��ʱ��ջָ���ű���*/
unsigned long temp_stack;

/**
 * �������ȼ�����
 * ÿ��Ԫ�ض���ָ�����������ָ��
 */
#define num_plan	10
static union task *task_plan[num_plan] =
{
	NULL
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
	task_0.info.counter = 100;
	
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
	
	schedule_flag = true;
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

/**�����ں˼��̺߳���*/
tid new_task(int (*function)(void *), void *argument)
{
	return;
	/**��ֹ�л�����*/
	disable_schedule();
	
	/**Ϊ����������ݽṹ����ռ�*/
	union task *new_task;
	new_task = vmalloc(sizeof(union task));
	if (new_task == NULL) return 0;
	memset(new_task, 0, sizeof(union task));
	
	/**����������뵽����ID˫��������*/
	union task *tsk_ptr;
	for (tsk_ptr = &task_0; tsk_ptr = new_task->info.after; tsk_ptr->info.after != NULL);
	tsk_ptr->info.after = new_task;
	new_task->info.before = tsk_ptr;
	new_task->info.after = NULL;
	new_task->info.id = tsk_ptr->info.father + 1;
	
	/**������������ݽṹ���и�ֵ*/
	new_task->info.father = current->info.id;	/**���ø�����ָ��*/
	new_task->info.counter = 30;				/**ʱ��Ƭ��30��ʼ��ʱ*/
	new_task->info.runtime = 0;					/**�������ܹ�ִ��ʱ���0*/
	new_task->info.state = TASK_HIGH;			/**����������Ϊ�����ȼ�����*/
	new_task->info.pptr = current->info.pptr;	/**������ͬ���Ǳ����̵�һ���߳�*/
	new_task->info.pptr->nthread ++;			/**�����̵��߳�������һ*/
	
	/**��������Ķ�ջ���г�ʼ��*/
	new_task->info.stack = Init_Kernel_Task(((unsigned long)new_task + TASK_SIZE), function, argument);
	
	/**����������Ϊ�������ȼ�*/
	new_task->info.plan = TASK_NORMAL;
	
	/**����������ӵ�ʵʱ���ȼ�������*/
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
	/**�����л�����*/
	enable_schedule();
	
	/**����*/
	return new_task->info.id;
}

/**ִ��Ӧ�ó���������POSIX��׼��*/
int run_exec(void *arg)
{
	return;
	struct GBOS_head *head;
	struct exec_file_para *file_para = arg;
	struct process_struct *new_process;
	/**ֹͣ����*/
	disable_schedule();
	
	/**������̽ṹ��*/
	for (new_process = NULL; new_process == NULL; )
		new_process = kmalloc(sizeof(struct process_struct), 0);
	
	/**��ս��̽ṹ��*/
	memset(new_process, 0, sizeof(struct process_struct));
	
	/**����ǰ�����뵱ǰ�����ѹ�*/
	current->info.pptr->nthread --;
	
	/**�����������ɽ���*/
	current->info.pptr = new_process;
	
	/**���̸�ֵ*/
	new_process->cr3 = new_pdt();
	new_process->msg_list = NULL;
	new_process->nthread ++;

	/**�л�ҳĿ¼��*/
	write_CR3(new_process->cr3);
	
	/**�������*/
	enable_schedule();
	
	/**�����ļ�*/
	file_open(file_para->filename, (void *)0x10000000);
	
	/**���սṹ��*/
	kfree(arg);
	
	/**��׼�ļ�ͷ*/
	head = (void *)0x10000000;
	
	/**����Ƿ�����ЧӦ�ó���*/
	if (head->flag[0] != 'G' |
		head->flag[1] != 'B' |
		head->flag[2] != 'O' |
		head->flag[3] != 'S' )
	{
		/**ʧ�ܷ���*/
		return 0;
	}
	
	/**ִ��Ӧ�ó���*/
	head->entry();
}

/**����������*/
union task* run(char *filename, char *para, int flag)
{
	return;
	struct exec_file_para *new_arg;
	
	/**����filename�ַ���ָ�벻��ΪNULL*/
	if (filename == NULL) return NULL;
	
	/**����һ��exec_file_para�ṹ��װ��ִ��run_exec�̵߳Ĳ���*/
	for (new_arg = NULL; new_arg == NULL; )
		new_arg = kmalloc(sizeof(struct exec_file_para), 0);
	
	/**���exec_file_para�ṹ���ֹ��ɸ���*/
	memset(new_arg, 0, sizeof(struct exec_file_para));
	
	/**�����ļ�����Ϣ����exec_file_para��*/
	strncpy(new_arg->filename, filename, 256);
	
	/**����в�����ͬʱ��������*/
	if (new_arg->para != NULL)
		strncpy(new_arg->para, para, 256);
	
	/**�������߳�����run_exec*/
	new_task(&run_exec, new_arg);
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
	goto finish;
	/**ָ������Ľṹ��*/
	union task *my = current;
	
	/**���Լ������������а���*/
	my->info.next->info.prev = my->info.prev;
	my->info.prev->info.next = my->info.next;
	
	/**�������Ⱥ���*/
	schedule();
	
finish:
	goto finish;
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
void schedule(void)
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
			// print_task(task_plan[plan_ptr]->info.id);
			/**���������ָ��Ϊ������*/
			new_task = task_plan[plan_ptr];
			/**�������ȼ���������ֻ��һ������ʱ*/
			if (task_plan[plan_ptr]->info.prev == task_plan[plan_ptr])
			{
				/**�������������˫��ѭ������*/
				task_plan[plan_ptr] = NULL;
			}else{
				/**�������������˫��ѭ������*/
				task_plan[plan_ptr]->info.prev->info.next = task_plan[plan_ptr]->info.next;
				task_plan[plan_ptr]->info.next->info.prev = task_plan[plan_ptr]->info.prev;
			}
			
			/**�˳�ѭ��*/
			break;
		}
	}
	
	/**����ʱ��Ƭ*/
	new_task->info.counter = 30;
	
	/**�л�ҳĿ¼��*/
	write_CR3(new_task->info.pptr->cr3);
	
	/**�л���������*/
	switch_to(new_task);
}


/**���������*/
void sleep(void)
{
	return;
	/**��������*/
	// current->info.next->info.prev = current->info.prev;
	// current->info.prev->info.next = current->info.next;
	
	/**����˯������*/
	// current->info.state = TASK_SLEEP;
	
	/**��������*/
	// schedule();
}

/**�����Ѻ���*/
void wakeup(tid target)
{
	return;
	/**�жϸ������Ƿ��Ѿ�����*/
	// if (target->info.state == TASK_SLEEP)
	// {
		// /**�ոջ��ѵ����������������*/
		// target->info.state = TASK_HIGH;
		
		// /**���������*/
		// target->info.prev = current;
		// target->info.next = current->info.next;
		// current->info.next = target;
		// target->info.next->info.prev = target;
	// }
	
	/**��������*/
	return;
}