
#ifndef PAGE_H_
#define PAGE_H_

#define PAGE_SIZE	4096				// 页的大小：4096Byte

/**页故障处理程序*/
void do_page_fault(int error_code);

#endif