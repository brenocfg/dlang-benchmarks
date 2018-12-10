#include <stdio.h>
#include <math.h>
 
int main() {
    for (int i = 1; i < 5000; i++) {
        int sum = 0;
        for (int number = i; number > 0; number /= 10) {
            int digit = number % 10;
            sum += pow(digit, digit);
        }
        if (sum == i) {
            printf("%i\n", i);
        } 
    }
    return 0;
}

