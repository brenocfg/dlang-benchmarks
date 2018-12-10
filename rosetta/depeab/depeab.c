#include<stdio.h>
#define de 0
#define pe 1
#define ab 2
 
int main(){
	int sum = 0, i, j;
	int try_max = 0;
	int   count_list[3] = {1,0,0};
	for(i=2; i <= 20000; i++){
		try_max = i/2;
		sum = 1;
		for(j=2; j<try_max; j++){
			if (i % j)
				continue;
			try_max = i/j;
			sum += j;
			if (j != try_max)
				sum += try_max;
		}
		if (sum < i){
			count_list[de]++;
			continue;
		}
		if (sum > i){
			count_list[ab]++;
			continue;
		}
		count_list[pe]++;
	}
	printf("\nThere are %d deficient," ,count_list[de]);
	printf(" %d perfect," ,count_list[pe]);
	printf(" %d abundant numbers between 1 and 20000.\n" ,count_list[ab]);
return 0;
}
