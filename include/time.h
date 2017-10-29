
#ifndef TIME_H_
#define TIME_H_

#include <types.h>

/**时钟相关的变量*/
extern ulong century;					/**世纪*/
extern ulong year, mon, day;			/**年、月、日*/
extern ulong hour, min, sec;			/**时、分、秒*/
extern ulong week_day;					/**一周中当前天*/
extern ulong ms;							/**微秒*/
extern ulong system_runtime;				/**系统运行时间，一开始归0*/
	
#endif