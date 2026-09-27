#include <stdio.h>
#include <stdint.h>
#include <math.h>

int main(){
	long double n, d, t, p;
	for(int i=-1076; i<1024; i++){
		if(i < 0){
			d = powl(2, -1 * i);
			p = roundl(log10(d));
			n = powl(10, p);
			p = p * -1;
		}
		else{
			n = powl(2, i);
			p = roundl(log10(n));
			d = powl(10, p);
		}

		t = n / d;
		printf("{%.16Lf, %d},\n", t, (int)p);
	}

	return 0;
}
