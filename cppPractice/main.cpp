#include <stdio.h>
#include <time.h>
#include <string.h>
#include <random>
#include <iostream>
#pragma warning(disable:4996)
typedef const char* FILENAME;
typedef long long INT64;
typedef unsigned int UINT;

#define WRITEBUF(buf,data)  do{*buff=data;buff++;}while(0)
#define DECIMAL_ONES(x) ((INT64)(x)%10)
#define DECIMAL_PLACE   4

template<int plus>
const char* GetExponentFormat(int decimal_place){
    static char format[10];
    format[0]='%';
    sprintf(&format[1],"%d.%dE",decimal_place+7+plus,decimal_place);
    return format; 
}

namespace useTemplate{
    template<int Decimal_Place>
    int Double2Ascii(char* buff,double x,char separator){
#if 0       //最後の部分を落とせば、sprintfと同じ四捨五入ができるようになる
        char*   data=(char*)(&x);
        data[0]=0;
#endif
        static const double ENLARGE_SCALE=pow(10,Decimal_Place);     //xの有効数値を小数点の左に持っていくためのスケール係数
        static const double SHRINK_SCALE=pow(10,-1*Decimal_Place);   //xの有効数値を小数点の右に持っていくためのスケール係数

        double shrink_scale=SHRINK_SCALE;   //staticをそのまま使って計算すると遅くなるので、ローカルにコピーする
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
}

int Double2Ascii(char* buff,double x,UINT fractional_digits,char separator){
    const char* buff_sp=buff;
    const double x_origin=x;
    int decimal=0;
    if(true!=isfinite(x)){
        WRITEBUF(buff,'?'); //TODO 適切な文字を入れる
        WRITEBUF(buff,0);
        //null文字はカウントしない
        return (buff-buff_sp-1);
    }
    char sign=' ';
    if(x<0){
        //0以下の時、整数にし、符号を変える
        x=-x;
        sign='-';
    }
    WRITEBUF(buff,sign);

    //仮数部を1から10の間になるように調整
    int exponent=0;
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
    static const double lookup_multiple[]={1E0,1E1,1E2,1E3,1E4,1E5,1E6,1E7,1E8,1E9};
    static const double lookup_multiple_reciprocal[]={1E0,1E-1,1E-2,1E-3,1E-4,1E-5,1E-6,1E-7,1E-8,1E-9};
    if(fractional_digits>=sizeof(lookup_multiple)/sizeof(lookup_multiple[0])){
        fractional_digits=0;
    }
    double multiple=lookup_multiple[fractional_digits];
    double multiple_reciprocal=lookup_multiple_reciprocal[fractional_digits];

    x=x+0.5*multiple_reciprocal;    //四捨五入

    //四捨五入後、10以上になる場合、10以下にする
    while(x>=10){
        x*=0.1;
        exponent++;
    }

    //整数部
    decimal=(int)x%10;
    WRITEBUF(buff,decimal+'0');

    WRITEBUF(buff,separator);   //小数点を書く

    //小数部
    //TODO 1.0024999999999999*100000が10025.000000000000になることを直す必要ある
    x*=multiple;
    for(int i=0;i<fractional_digits;i++){
        decimal=(INT64)(x*10*multiple_reciprocal)%10;
        WRITEBUF(buff,decimal+'0');
        multiple_reciprocal*=10;
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
        decimal=(exponent/100)%10;
        WRITEBUF(buff,decimal+'0');
    }
    decimal=(exponent/10)%10;
    WRITEBUF(buff,decimal+'0');
    decimal=exponent%10;
    WRITEBUF(buff,decimal+'0');

    WRITEBUF(buff,0);

    //NULL文字はカウントしない
    return (buff - buff_sp-1);
}

int main()
{
    char buff[128];
    char buff2[128];
    const char* format=GetExponentFormat<0>(DECIMAL_PLACE);
    const char* formatPlus1=GetExponentFormat<1>(DECIMAL_PLACE);
    if(0){
        printf("効率確認\n");
        int looptimes=30E7;
        double data=15.9672;
        time_t start=time(NULL);
        for(int i=0;i<looptimes;i++){
            useTemplate::Double2Ascii<DECIMAL_PLACE>(buff,data,'.');
        }
        time_t end=time(NULL);
        printf("looptimes=%d,template version cost %lld s\n",looptimes,end-start);
        start=time(NULL);
        for(int i=0;i<looptimes;i++){
            Double2Ascii(buff,data,DECIMAL_PLACE,'.');
        }
        end=time(NULL);
        printf("looptimes=%d,Double2Ascii cost %lld s\n",looptimes,end-start);
        start=time(NULL);
        for(int i=0;i<looptimes;i++){
            sprintf(buff, format, data);
        }
        end=time(NULL);
        printf("looptimes=%d,sprintf cost %lld s\n",looptimes,end-start);
    }

    if(1){
        printf("個別確認\n");
        double data=(double)(1002450000)/1E-5;
        // Double2Ascii(buff,data,DECIMAL_PLACE,'.');
        useTemplate::Double2Ascii<DECIMAL_PLACE>(buff,data,'.');
        printf("%s\n",buff);
        sprintf(buff2,"%11.4E",data);
        printf("%s\n",buff2);
    }

    if(0){
        printf("ランダム入力 正確性確認\n");
        for(int times=0;times<304;times++){
            long double max=pow(10,times);
            printf("max=%f\n",max);
            std::random_device rd;
            std::mt19937 gen(rd());
            std::uniform_real_distribution<> dist(0,max );
        
            for (int i = 0; i < 1000; i++) {
                double data=dist(gen);
                // int size1=Double2Ascii(buff,data,4,'.');
                int size1=useTemplate::Double2Ascii<DECIMAL_PLACE>(buff,data,'.');
                int size2=sprintf(buff2,format,data);
                if(' '!=buff2[0]){
                    //最初のスペースを揃うために
                    size2=sprintf(buff2,formatPlus1,data);
                }
                double err=(atof(buff)-atof(buff2))/atof(buff);     //誤差
                if(err<0)   err=err*-1;
                if((0!=strcmp(buff,buff2))||(size1!=size2)){
                // if(true){
                    if(true!=isfinite(data)){
                        continue;
                    }
                    printf("err=%f",err);
                    printf("%s,size=%d,",buff,size1);
                    printf("%s,size=%d,",buff2,size2);
                    printf("%.11f,",data);
                    printf("diff!\n");
                    _ASSERT(false);
                }else{
                    printf("%s,size=%d,",buff,size1);
                    printf("%s,size=%d,",buff2,size2);
                    printf("correct\n");
                }
            }
        }
    }

    if(0){
        printf("固定入力 正確性確認");
        //11桁の数字で確認
        int err_cnt=0;
        for(int i=1000000000;i<10000000000;i++){
            double data=(double)i/1E-5;
            // int size1=Double2Ascii(buff,data,DECIMAL_PLACE,'.');
            int size1=useTemplate::Double2Ascii<DECIMAL_PLACE>(buff,data,'.');
            int size2=sprintf(buff2,format,data);
            // int size2=Double2Ascii(buff2,data,DECIMAL_PLACE,'.');
            double err=(atof(buff)-atof(buff2))/atof(buff);     //誤差
            if(err<0)   err=err*-1;
            if((0!=strcmp(buff,buff2))||(size1!=size2)){
            // if(true){
                err_cnt++;
                if(err<0.0001){
                    // continue;
                }
                // printf("err=%f,rate=%.8E,err_cnt=%d,",err,(double)(err_cnt/(i-1000000000+1)),err_cnt);
                printf("%s,size=%d,",buff,size1);
                printf("%s,size=%d,",buff2,size2);
                printf("%.7f,",data);
                INT64* pDATA=(INT64*)(&data);
                printf("%llx,",*pDATA);
                printf("diff!\n");
                printf("bug num=%d\n",i);
                // _ASSERT(false);
            }/*else{
                printf("%s,size=%d,",buff,size1);
                printf("%s,size=%d,",buff2,size2);
                printf("correct\n");
            }*/
        }
    }

    return 0;
}
