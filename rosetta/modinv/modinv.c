#include <stdio.h>
 
int mul_inv(int a, int b)
{
        int t, nt, r, nr, q, tmp;
        if (b < 0) b = -b;
        if (a < 0) a = b - (-a % b);
        t = 0;  nt = 1;  r = b;  nr = a % b;
        while (nr != 0) {
          q = r/nr;
          tmp = nt;  nt = t - q*nt;  t = tmp;
          tmp = nr;  nr = r - q*nr;  r = tmp;
        }
        if (r > 1) return -1;
        if (t < 0) t += b;
        return t;
}
int main(void) {
        printf("%d\n", mul_inv(42, 2017));
        return 0;
}
