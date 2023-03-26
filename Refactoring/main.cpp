#include <string.h>
#include <time.h>
#if 1
#include "numbertostring.h"
#else
#include <stdio.h>
#include "../cppPractice/double2ascii.h"
#endif

template<int> struct CompileTimeError;
template<> struct CompileTimeError<true> {};
#define STATIC_CHECK(expr, msg) {CompileTimeError<(expr)!=0> Error_##msg; (void)Error_##msg;}

#define arraylen 2
#define ARRAYLEN(array) (sizeof(array)/sizeof(array[0]))

template<int DECIMALPLACE>
bool Test_Correctness(){
    double test_cases[] = {
        1.1, 234.7899, 333, -3.675, 1E122, 0
    };

    char want[32], got[32];

    for(int i=0; i<ARRAYLEN(test_cases); i++){
        double test_num = test_cases[i];
        int want_len = sprintf(want, "%.*E", DECIMALPLACE, test_num);
        int got_len = Double2Ascii<DECIMALPLACE>(got, test_num, '.');
        if(want_len != got_len){
            printf("want=\"%s\", got=\"%s\"\n", want, got);
            printf("Correctness test failed,test number=%*E, want len = %d, got len = %d\n", 
                    DECIMALPLACE, test_num, want_len, got_len);
            return false;
        }
        if(0 != strcmp(want, got)){
            printf("outputs are not the same,want:\"%s\",got:\"%s\"\n", want, got);
            return false;
        }
    }

    printf("Correctness test ok\n");
    return true;
}

template<int DECIMALPLACE>
bool Test_Benchmark(){
    /*
     * before:1.71
     * after :0.54
     * improvement:3~3.2
     * */
    int times = 1E7;
    clock_t start, end;
    double test_num = 32683921.1323;
    // double test_num = 1E-200;
    // double test_num = 1E199;
    double before, after;
    char buffer[32];

    start = clock();
    for(int i=0; i<times; i++){
        sprintf(buffer, "%.*E", DECIMALPLACE, test_num);
    }
    end = clock();
    before = (end-start)/(double)CLOCKS_PER_SEC;

    start = clock();
    for(int i=0; i<times; i++){
        Double2Ascii<DECIMALPLACE>(buffer, test_num, '.');
    }
    end = clock();
    after = (end-start)/(double)CLOCKS_PER_SEC;


    printf("before:%f, after:%f\n", before, after);
    printf("improvement = %f\n", before/after);

    return true;
}

int main(int argc, const char* argv[]){
#if 1
    Test_Correctness<4>();
    Test_Benchmark<4>();
    Test_Correctness<9>();
    Test_Benchmark<9>();
    // Test_Correctness<17>();
    // Test_Benchmark<17>();
#else
    printf("%lld\n", (OneE(1)));
    printf("%lld\n", (OneE(2)));
    printf("%lld\n", (OneE(3)));
    printf("%lld\n", (OneE(4)));
    printf("%lld\n", (OneE(5)));
    printf("%lld\n", (OneE(6)));
    printf("%lld\n", (OneE(7)));
    printf("%lld\n", (OneE(8)));
    printf("%lld\n", (OneE(9)));
    printf("%lld\n", (OneE(10)));
    printf("%lld\n", (OneE(11)));
    printf("%lld\n", (OneE(12)));
    printf("%lld\n", (OneE(13)));
    printf("%lld\n", (OneE(14)));
    printf("%lld\n", (OneE(15)));
    printf("%lld\n", (OneE(16)));
    printf("%lld\n", (OneE(17)));
    printf("%lld\n", (OneE(18)));
#endif
    return 0;
}
