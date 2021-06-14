#include <limits.h>

int main(void)
{
	unsigned long long a = UINT_MAX / 2 + 1;
	unsigned long long b = a * a / 4 - 1;
	printf("%llX\n", b);
}
