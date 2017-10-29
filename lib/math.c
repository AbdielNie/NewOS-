
#include <lib/math.h>

/**取一个有符号数的绝对值函数*/
long abs(long number)
{
	if (number < 0) return number * (-1);
	return number;
}