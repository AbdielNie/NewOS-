
#include "interrupt.h"

/**中断应答函数*/
void interrupt_handle(struct context context, unsigned char irq, int error_code)
{
	irqaction[irq].handle(error_code);
}

/**简单应答函数*/
void easy_handle(int error_code)
{
	/**无操作*/
	printk("Unknown Interrupt. error:%#x.\n", error_code);
}
