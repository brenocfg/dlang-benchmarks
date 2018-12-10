#include <stdio.h>
 
void comb(int m, int n, unsigned char *c)
{
	int i;
	for (i = 0; i < n; i++) c[i] = n - i;
 
	while (1) {
		for (i = n; i--;)
			printf("%d%c", c[i], i ? ' ': '\n');
 
		i = 0;
		if (c[i]++ < m) continue;
 
		for (; c[i] >= m - i;) if (++i >= n) return;
		for (c[i]++; i; i--) c[i-1] = c[i] + 1;
	}
}
 
int main()
{
	unsigned char buf[100];
	comb(5, 3, buf);
	return 0;
}
