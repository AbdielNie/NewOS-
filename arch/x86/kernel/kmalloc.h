
#ifndef KMALLOC_H_
#define KMALLOC_H_

/**�ڴ��*/
struct mem_pool
{
	size_t size;
	unsigned long number;
	struct Memory_Descriptor *next;
};

/**�ڴ�������*/
struct Memory_Descriptor
{
	void *page;
	void *freeptr;
	unsigned short refcnt;
	struct Memory_Descriptor *next;
};
/**����flags�����������֮һ*/
#define MEM_INVALID	0
#define MEM_FREE	1
#define MEM_USED	2


/**�ں�С���ڴ���亯��
 * ����:size - ������ڴ�鳤��
 * ����ֵ:NULL - ����ʧ��; !NULL - ����ɹ��������ڴ��׵�ַ
 */
void *kmalloc(size_t size, int flags);

/**�ں�С���ڴ��ͷź���
 * ����:point - ָ��Ҫ�ͷŵ��ڴ��׵�ַ(֮ǰkmalloc�����ķ���ֵ)
 */
void kfree(void *point);

#endif