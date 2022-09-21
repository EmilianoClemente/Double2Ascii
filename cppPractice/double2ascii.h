#include <math.h>

typedef long long INT64;
typedef unsigned int UINT;

#define WRITEBUF(buf,data)  do{*buff=data;buff++;}while(0)
#define DECIMAL_ONES(x) ((INT64)(x)%10)
#define DECIMAL_PLACE   4
template<int Decimal_Place>
int Double2Ascii(char* buff,double x,char separator){
    /*xの有効数値を小数点の左に持っていくためのスケール係数*/
    static const double ENLARGE_SCALE=pow(10,Decimal_Place);
    /*xの有効数値を小数点の右に持っていくためのスケール係数*/
    static const double SHRINK_SCALE=pow(10,-1*Decimal_Place);

    /*staticをそのまま使って計算すると遅くなるので、ローカルにコピーする*/
    double shrink_scale=SHRINK_SCALE;
    double enlarge_scale=ENLARGE_SCALE;
    const char* buff_sp=buff;
    char decimal=0;
    char sign=' ';
    int exponent=0;

    if(0==_finite(x)){
        WRITEBUF(buff,'?'); //TODO 適切な文字を入れる
        goto FINALLY;
    }
    if(x<0){
        //0以下の時、整数にし、符号を変える
        x=-x;
        sign='-';
    }
    WRITEBUF(buff,sign);

    //仮数部を1から10の間になるように調整
    if(x>0){
        while(x>=1E10){
            //大きい数字来るときの高速化対策
            x*=1E-10;
            exponent+=10;
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

    //四捨五入処理
    x=x+0.5*shrink_scale;

    //四捨五入後、10以上になる場合、10以下にする
    while(x>=10){
        x*=0.1;
        exponent++;
    }

    //整数部
    decimal=DECIMAL_ONES(x);
    WRITEBUF(buff,decimal+'0');

    WRITEBUF(buff,separator);   //小数点を書く

    //小数部
    //TODO 1.0024999999999999*100000が10025.000000000000になることを直す必要ある
    x*=(enlarge_scale*10);
    for(int i=0;i<Decimal_Place;i++){
        decimal=DECIMAL_ONES(x*shrink_scale);
        WRITEBUF(buff,decimal+'0');
        shrink_scale*=10;
    }

    //指数部
    WRITEBUF(buff,'E');
    //符号
    if(exponent<0){
        exponent=-exponent;
        WRITEBUF(buff,'-');
    }else{
        WRITEBUF(buff,'+');
    }

    if(exponent>=100){
        //指数部が3桁の時
        decimal=DECIMAL_ONES(exponent/100);
        WRITEBUF(buff,decimal+'0');
    }
    decimal=DECIMAL_ONES(exponent/10);
    WRITEBUF(buff,decimal+'0');
    decimal=DECIMAL_ONES(exponent);
    WRITEBUF(buff,decimal+'0');

FINALLY:
    WRITEBUF(buff,0);

    //NULL文字はカウントしない
    return (buff - buff_sp-1);
}

