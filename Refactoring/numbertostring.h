#pragma once
#include <stdio.h>
#include <math.h>
#include <stdint.h>

template<int N>
const int64_t PowOfN = pow(10, N);

template<int N>
const double DPowOfN = pow(10, N);

#define OneE(n) (PowOfN<n>)
#define DOneE(n) (DPowOfN<n>)

#define OPTIMIZE_WHEN_LARGE_NUM

template<int Decimal_Place>
size_t Double2Ascii(char* buff,double x,char separator){
    char* const buff_sp=buff;
    char* p_write=(char*)0;
    int exponent=Decimal_Place;
    int64_t effective=0;
    size_t size = 0;

    if(!isfinite(x)){
        *buff = 0;
        return 0;
    }

    if(x<0){
        x=-x;
        *buff = '-';
        buff++;
    }

    if(x == 0){
        exponent = 0;
    }
    else{
        if(x >= (DOneE(Decimal_Place+1))){
#ifdef OPTIMIZE_WHEN_LARGE_NUM
            while( x >= DOneE(100+Decimal_Place+1)){
                x *= DOneE(-100);
                exponent+=100;
            }
            while(x >= DOneE(20+Decimal_Place+1)){
                x *= DOneE(-20);
                exponent+=20;
            }
#endif
            while (x >= (DOneE(Decimal_Place+1))) {
                x *= 0.1;
                exponent++;
            }
        }
        else{
#ifdef OPTIMIZE_WHEN_LARGE_NUM
            while( x <= DOneE(-100+Decimal_Place)){
                x *= DOneE(100);
                exponent-=100;
            }
            while(x <= DOneE(-20+Decimal_Place)){
                x *= DOneE(20);
                exponent-=20;
            }
#endif
            while (x < (DOneE(Decimal_Place))) {
                x *= 10;
                exponent--;
            }
        }
    }

    effective = (int64_t)(x+0.5);
    if(effective>=OneE(Decimal_Place+1)){
        x *= 0.1;
        exponent ++;
    }

    for(p_write = buff+Decimal_Place+1;p_write>buff;p_write--){
        *p_write = effective%10 + '0';
        effective/=10;
    }
    buff[0] = buff[1];
    buff[1] = separator;
    buff+=(Decimal_Place+2);

    *buff = 'E';
    buff++;

    if(exponent<0){
        exponent=-exponent;
        *buff = '-';
    }else{
        *buff = '+';
    }

    p_write = buff+3;
    if(exponent>99){
        p_write++;
    }
    size = p_write - buff_sp;
    *p_write = 0;
    p_write--;

    for(; p_write>buff; p_write--){
        *p_write = exponent%10 + '0';
        exponent/=10;
    }

    return size;
}
