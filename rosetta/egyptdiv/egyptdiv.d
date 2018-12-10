import std.stdio;
 
int main(string[] args) {
        import std.conv;
 
        ulong remainder;
 
        auto ans = egyptian_division(580, 32, remainder);
        writeln(580, " / ", 32, " = ", ans, " rem ", remainder);
 
        return 0;
}
 
ulong egyptian_division(ulong dividend, ulong divisor, out ulong remainder) {
    enum SIZE = 64;
    ulong[SIZE] powers;
    ulong[SIZE] doublings;
    int i;
 
    for (; i<SIZE; ++i) {
        powers[i] = 1 << i;
        doublings[i] = divisor << i;
        if (doublings[i] > dividend) {
            break;
        }
    }
 
    ulong answer;
    ulong accumulator;
 
    for (i=i-1; i>=0; --i) {
        if (accumulator + doublings[i] <= dividend) {
            accumulator += doublings[i];
            answer += powers[i];
        }
    }
 
    remainder = dividend - accumulator;
    return answer;
}
 
