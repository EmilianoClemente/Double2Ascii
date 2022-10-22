#include <math.h>

typedef long long INT64;
typedef unsigned int UINT;

#define WRITEBUF(buf,data)  do{*buff=data;buff++;}while(0)
#define DECIMAL_ONES(x) ((INT64)(x)%10)
#define DECIMAL_PLACE   4

template<int N>
struct powOf10
{
    enum{value = 10*powOf10<N-1>::value};
};

template<>
struct powOf10<0>
{
    enum{value = 1};
};

template<int Decimal_Place>
size_t Double2Ascii(char* buff,double x,char separator){
    const char* buff_sp=buff;
    char decimal=0;
    char sign=' ';
    int exponent=0;
    int effective=0;
    char* p_write=(char*)0;

    if(0==_finite(x)){
        *buff = 0;
        return 0;
    }
    if(x<0){
        //0�ȉ��̎��A�����ɂ��A������ς���
        x=-x;
        sign='-';
    }
    WRITEBUF(buff,sign);

    //��������1����10�̊ԂɂȂ�悤�ɒ���
    if(x>0){
        while(x>=1E10){
            //�傫����������Ƃ��̍������΍�
            x*=1E-10;
            exponent+=10;
        }
        while(x<=1E-10){
            //��������������Ƃ��̍������΍�
            x*=1E10;
            exponent-=10;
        }
        while (x >= 10) {
            x *= 0.1;
            exponent++;
        }
        while (x < 1) {
            x *= 10;
            exponent--;
        }
    }

    effective =(int)(x * powOf10<Decimal_Place>::value + 0.5);
    if(effective>=powOf10<Decimal_Place+1>::value){
        x *= 0.1;
        exponent ++;
    }

    for(p_write = buff+Decimal_Place+1;p_write>buff;p_write--){
        decimal = effective%10;
        *p_write = decimal+'0';
        effective/=10;
    }
    buff[0] = buff[1];
    buff[1] = separator;
    buff+=(Decimal_Place+2);

    //�w����
    WRITEBUF(buff,'E');
    //����
    if(exponent<0){
        exponent=-exponent;
        WRITEBUF(buff,'-');
    }else{
        WRITEBUF(buff,'+');
    }

    if(exponent>=100){
        //�w������3���̎�
        decimal=DECIMAL_ONES(exponent/100);
        WRITEBUF(buff,decimal+'0');
    }
    decimal=DECIMAL_ONES(exponent/10);
    WRITEBUF(buff,decimal+'0');
    decimal=DECIMAL_ONES(exponent);
    WRITEBUF(buff,decimal+'0');

    *buff = 0;
    //NULL�����̓J�E���g���Ȃ�
    return (buff - buff_sp);
}

