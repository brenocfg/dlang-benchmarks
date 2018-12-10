import std.stdio;
 
void main() {
    for (int i=1; i<5000; i++) {
        int sum = 0;
        for (int number=i; number>0; number/=10) {
            int digit = number % 10;
            sum += digit ^^ digit;
        }
        if (sum == i) {
            writeln(i);
        } 
    }
}
