#include "numbertostring.h"
#include <stdint.h>
#include <string.h>

static uint64_t div10_64bit(uint64_t x) {
	unsigned __int128 magic = 0xCCCCCCCCCCCCCCCDULL;

	return (uint64_t)((x * magic) >> 67);
}	

static uint64_t div100_64bit(uint64_t x) {
    unsigned __int128 magic = 0x28F5C28F5C28F5DULL;
    
    unsigned __int128 product = (unsigned __int128)x * magic;
    
    return (uint64_t)(product >> 64);
}

static size_t handle_7ff(char* buf, uint64_t mantissa){
	if(mantissa == 0){
		memcpy(buf, "Inf", 4);
	}
	else{
		memcpy(buf, "NaN", 4);
	}

	return 4;
}

static inline uint64_t fast_mod10_u64(uint64_t x) {
    unsigned __int128 magic = 14757395258967641293ULL; 
    
    uint64_t q = (uint64_t)((x * magic) >> 67);
    
    return x - q * 10;
}

static inline uint64_t fast_mod100_u64(uint64_t x) {
	return x - div100_64bit(x) * 100;
}

static char lookup_table[] = {
	'0','0','0','1','0','2','0','3','0','4','0','5','0','6','0','7','0', '8','0', '9',
	'1','0','1','1','1','2','1','3','1','4','1','5','1','6','1','7','1', '8','1', '9',
	'2','0','2','1','2','2','2','3','2','4','2','5','2','6','2','7','2', '8','2', '9',
	'3','0','3','1','3','2','3','3','3','4','3','5','3','6','3','7','3', '8','3', '9',
	'4','0','4','1','4','2','4','3','4','4','4','5','4','6','4','7','4', '8','4', '9',
	'5','0','5','1','5','2','5','3','5','4','5','5','5','6','5','7','5', '8','5', '9',
	'6','0','6','1','6','2','6','3','6','4','6','5','6','6','6','7','6', '8','6', '9',
	'7','0','7','1','7','2','7','3','7','4','7','5','7','6','7','7','7', '8','7', '9',
	'8','0','8','1','8','2','8','3','8','4','8','5','8','6','8','7','8', '8','8', '9',
	'9','0','9','1','9','2','9','3','9','4','9','5','9','6','9','7','9', '8','9', '9',
};

static void write_effective(char* buf, uint64_t effective, int n){
	char* w = buf + (n - 1);
	char* p;
	while(w > buf){
		p = &(lookup_table[fast_mod100_u64(effective) * 2 + 1]);
		effective = div100_64bit(effective);
		*(w--) = *(p--);
		*(w--) = *p;
	}

	if(w == buf){
		*w = fast_mod10_u64(effective) + '0';
	}
}

static int ndigit(uint64_t n){
	if(n >= 10000000000000000000UL)return 20;
	if(n >= 1000000000000000000UL)return 19;
	if(n >= 100000000000000000)return 18;
	if(n >= 10000000000000000)return 17;
	if(n >= 1000000000000000)return 16;
	if(n >= 100000000000000)return 15;
	if(n >= 10000000000000)return 14;
	if(n >= 1000000000000)return 13;
	if(n >= 100000000000)return 12;
	if(n >= 10000000000)return 11;
	if(n >= 1000000000)return 10;
	if(n >= 100000000)return 9;
	if(n >= 10000000)return 8;
	if(n >= 1000000)return 7;
	if(n >= 100000)return 6;
	if(n >= 10000)return 5;
	if(n >= 1000)return 4;
	if(n >= 100)	return 3;
	if(n >= 10)return 2;
	if(n >= 1)return 1;

	return 0;
}

size_t ToChars(char* buf, double x, char separator, int n){
	uint64_t bits = *((uint64_t*)(&x));
	uint64_t mantissa = bits & 0xfffffffffffffUL;
	uint64_t f = mantissa | (0x1UL << 52);
	int exponent = (bits >> 52) & 0x7ff;
	int e;
	int edigit = 2;

	n++;
	if(exponent == 0x7ff){
		return handle_7ff(buf, mantissa);
	}

	if(bits >> 63){
		// minus
		*(buf++) = '-';
	}

	if(exponent == 0){
		if(mantissa == 0){
			// +- 0
			*(buf++) = '0';
			*buf = 0;
			return 2;
		}
		else{
			// subnormal number
			exponent = 1;
			f = mantissa;
		}
	}

	exponent = exponent - 1023 - 52;
	if(exponent < (-52)){
		int digits;
		e = 0;
		while(exponent != 0){
			while((0xffc0000000000000UL & f) == 0){
				f = f * 10;
				e = e - 1;
			}
			if(exponent < -10){
				f = f >> 10;
				exponent = exponent + 10;
			}
			else{
				f = f >> 1;
				exponent = exponent + 1;
			}
		}
		digits = ndigit(f);
		e = digits + e - 1;
		while(digits > n){
			digits--;
			f = div10_64bit(f);
		}

		if(e <= -100){
			edigit = 3;
		}
	}
	else if(exponent < 0){
		int p = -1 * exponent;
		uint64_t integer = f >> p;
		uint64_t mask = (0x1UL << p) - 1;
		uint64_t remainder = f & mask;
		int digits = ndigit(integer);
		e = digits - 1;
		if(digits < n){
			while(digits < n){
				digits ++;
				remainder = (remainder & mask) * 10;
				integer = integer * 10 + (fast_mod10_u64(remainder >> p));
			}
		}
		else{
			while(digits > n){
				digits --;
				// integer/=10;
				integer = div10_64bit(integer);
			}
		}
		f = integer;
	}else if(exponent < 11){
		// big number
		int digits;
		f = f << exponent;
		digits = ndigit(f);
		e = digits - 1;

		while(digits > n){
			digits --;
			// f/=10;
			f = div10_64bit(f);
		}
	}
	else{
		// really big number
		int digits;
		e = 0;
		while(exponent > 10){
			f = f << 10;
			// f = f / 1000;
			f = div10_64bit(f);
			f = div10_64bit(f);
			f = div10_64bit(f);
			exponent = exponent - 10;
			e = e + 3;
			while(0xffc0000000000000UL & f){
				// f = f / 10;
				f = div10_64bit(f);
				e = e + 1;
			}
		}
		while(exponent > 0){
			f = f << 1;
			exponent--;
			while(f & (1UL << 63)){
				// f = f / 10;
				f = div10_64bit(f);
				e = e + 1;
			}
		}

		digits = ndigit(f);
		e = e + digits - 1;
		while(digits > n){
			digits--;
			// f = f / 10;
			f = div10_64bit(f);
		}

		if(e >= 100){
			edigit = 3;
		}
	}

	if(f % 10 > 5){
		f++;
		if(ndigit(f) != n){
			// f = f / 10;
			f = div10_64bit(f);
		}
	}
	// f = f / 10;
	f = div10_64bit(f);
	n --;

	write_effective(buf + 1, f, n);
	*buf = buf[1];
	buf[1] = separator;

	buf[n + 1] = 'E';
	if(e < 0){
		buf[n+2] = '-';
		e = -1 * e;
	}
	else{
		buf[n+2] = '+';
	}

	write_effective(buf+n+3, e, edigit);

	buf[n + 3 + edigit] = 0;
	
	return n + 4 + edigit;
}

#ifdef TEST_NUMBERTOSTRING
#include <exception>
static void check_write_effective(){
	char buffer[32];

	write_effective(buffer, 987654321, 9);
	buffer[9] = 0;

	if(strcmp(buffer, "987654321") != 0){
		printf("%s\n", buffer);
		throw std::exception();
	}

	write_effective(buffer, 1234567890, 10);
	buffer[10] = 0;

	if(strcmp(buffer, "1234567890") != 0){
		printf("%s\n", buffer);
		throw std::exception();
	}
	printf("%s\n", buffer);

	write_effective(buffer, 2, 2);
	buffer[2] = 0;

	if(strcmp(buffer, "02") != 0){
		printf("%s\n", buffer);
		throw std::exception();
	}
	printf("%s\n", buffer);
}

static void check_ToChars_sub(double x, int digits){
	char buffer[64], answer[64];
	memset(buffer, ' ', sizeof(buffer));
	ToChars(answer, x, '.', digits);

	printf("%s", answer);

	if(x > 0){
		snprintf(buffer, sizeof(buffer), " %.*E", digits - 1, x);
		printf("%s\n", buffer);
	}
	else{
		snprintf(buffer, sizeof(buffer), "%.*E", digits - 1, x);
		printf("%s\n", buffer);
	}

	if(strcmp(answer, buffer)!= 0){
		// throw std::exception();
	}
}

static void check_ToChars(){
	check_ToChars_sub(1.2345, 8);
	check_ToChars_sub(234567, 8);
	check_ToChars_sub(34567, 8);
	check_ToChars_sub(4567, 8);
	check_ToChars_sub(567, 8);
	check_ToChars_sub(1.23456E-33, 8);
	check_ToChars_sub(1.23456E+16, 8);
	check_ToChars_sub(1.23456E+17, 8);
	check_ToChars_sub(1.23456E+18, 8);
	check_ToChars_sub(9.998888E+18, 8);
	check_ToChars_sub(3.456e+24, 8);
	check_ToChars_sub(4.9136813-101, 8);
	check_ToChars_sub(4.9136813+101, 8);
	check_ToChars_sub(4.9136813e-101, 8);
	check_ToChars_sub(4.9136813e+101, 8);
	check_ToChars_sub(4.9136813e-309, 8);
}

#include <chrono>
static void benchmark(){
	char buffer[32];
	const double x = 1.234567E10;
	enum{
		LOOP = 1000000,
		N = 9,
	};
	{
		memset(buffer, 0, sizeof(buffer));
		auto t1 = std::chrono::steady_clock::now();
		for(int i=0; i<LOOP; i++){
			Double2Ascii<N>(buffer, x, '.');
		}

		auto t2 = std::chrono::steady_clock::now();
		printf("benchmark t=%lld, %s\n", (t2 - t1).count(), buffer);
	}
	{
		memset(buffer, 0, sizeof(buffer));
		auto t1 = std::chrono::steady_clock::now();
		for(int i=0; i<LOOP; i++){
			ToChars(buffer, x, '.', N+1);
		}

		auto t2 = std::chrono::steady_clock::now();
		printf("benchmark t=%lld, %s\n", (t2 - t1).count(), buffer);
	}
}

#include <string.h>

int main(){
	try{
		check_ToChars();
		check_write_effective();
		benchmark();
	}catch(...){
		printf("test failed\n");
	}

	return 0;
}

#endif
