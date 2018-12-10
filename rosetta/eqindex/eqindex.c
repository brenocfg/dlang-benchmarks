#include <stdio.h>
#include <stdlib.h>
 
int list[] = {-7, 1, 5, 2, -4, 3, 0};
 
int eq_idx(int *a, int len, int **ret)
{
	int i, sum, s, cnt;
        cnt = s = sum = 0;
	*ret = malloc(sizeof(int) * len);
 
	for (i = 0; i < len; i++)
                sum += a[i];
 
	for (i = 0; i < len; i++) {
		if (s * 2 + a[i] == sum) {
			(*ret)[cnt] = i;
                        cnt++;
                }
		s += a[i];
	}
 
	*ret = realloc(*ret, cnt * sizeof(int));
 
	return cnt;
}
 
int main()
{
	int i, cnt, *idx;
	cnt = eq_idx(list, sizeof(list) / sizeof(int), &idx);
 
	printf("Found:");
	for (i = 0; i < cnt; i++)
                printf(" %d", idx[i]);
	printf("\n");
 
	return 0;
}
