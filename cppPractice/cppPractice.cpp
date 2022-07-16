#include <stdio.h>
#include <time.h>
#include <string.h>
#include <random>
#include <iostream>
#pragma warning(disable:4996)
typedef const char* FILENAME;
typedef long long int64;

#define WRITEBUF(buf,data)  do{*buff=data;buff++;}while(0)
#define NORMAL_ROUND

int Double2Ascii(char* buff,double x,int fractional_digits,char separator){
    const char* startAdr=buff;
    int digit=0;
    if(true!=isfinite(x)){
        WRITEBUF(buff,'?'); //TODO 適切な文字を入れる
        WRITEBUF(buff,0);
        //null文字はカウントしない
        return (buff-startAdr-1);
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
#ifdef  NORMAL_ROUND
    double adjust=0.5;  //四捨五入調整用
    for(int i=0;i<fractional_digits;i++){
        adjust*=0.1;
    }
    x=x+adjust;
#endif
#ifdef  ROUND_TO_EVEN
    int adjust_multipy=1;
    for(int i=0;i<fractional_digits;i++){
        adjust_multipy*=10;
    }
    int second_last_eff_digit=(int64)(x*adjust_multipy/10+0.5)%10;
    int last_eff_digit=(int64)(x*adjust_multipy)%10;
    int throw_digit=(int64)(x*adjust_multipy*10)%10;
    if(5==throw_digit){
        if(0==second_last_eff_digit%2){
            //偶数なら捨てるので、何もしない
        }else{
            //奇数
            x=x+(0.5/adjust_multipy);
        }
    }else{
        x=x+(0.5/adjust_multipy);
    }
#endif

    //四捨五入後、10以上になる場合、10以下にする
    while(x>=10){
        x*=0.1;
        exponent++;
    }

    //整数部
    digit=(int)x%10;
    WRITEBUF(buff,digit+'0');

    WRITEBUF(buff,separator);   //小数点を書く

    //小数部
#if 0
    int multiple=10;    //倍数
    for(int i=0;i<fractional_digits;i++){
        digit=(int64)(x*multiple)%10;
        WRITEBUF(buff,digit+'0');
        multiple*=10;
    }
#else
    double multiple=10;
    for(int i=0;i<fractional_digits;i++){
        multiple*=10;
    }
    x*=multiple;
    for(int i=0;i<fractional_digits;i++){
        digit=(int64)(x*10/multiple)%10;
        WRITEBUF(buff,digit+'0');
        multiple*=0.1;
    }
#endif

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
        digit=(exponent/100)%10;
        WRITEBUF(buff,digit+'0');
    }
    digit=(exponent/10)%10;
    WRITEBUF(buff,digit+'0');
    digit=exponent%10;
    WRITEBUF(buff,digit+'0');

    WRITEBUF(buff,0);

    //NULL文字はカウントしない
    return (buff - startAdr-1);
}

int main()
{
    char buff[128];
    char buff2[128];
    if(0){
        printf("効率確認\n");
        int looptimes=15E7;
        double data=15.9672E19;
        time_t start=time(NULL);
        for(int i=0;i<looptimes;i++){
            Double2Ascii(buff,data,4,'.');
        }
        time_t end=time(NULL);
        printf("looptimes=%d,Double2Ascii cost %lld s\n",looptimes,end-start);
        start=time(NULL);
        for(int i=0;i<looptimes;i++){
            sprintf(buff, "%11.4E", data);
        }
        end=time(NULL);
        printf("looptimes=%d,sprintf cost %lld s\n",looptimes,end-start);
    }

    if(1){
        printf("個別確認\n");
        double data=(double)(9779950)/1E-5;
        Double2Ascii(buff,data,4,'.');
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
                int size1=Double2Ascii(buff,data,4,'.');
                int size2=sprintf(buff2,"%11.4E",data);
                if(' '!=buff2[0]){
                    //最初のスペースを揃うために
                    size2=sprintf(buff2,"%12.4E",data);
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

    if(1){
        printf("固定入力 正確性確認");
        //11桁の数字で確認
        for(int i=0;i<10000000000;i++){
            double data=(double)i/1E-5;
            int size1=Double2Ascii(buff,data,4,'.');
            int size2=sprintf(buff2,"%11.4E",data);
            double err=(atof(buff)-atof(buff2))/atof(buff);     //誤差
            if(err<0)   err=err*-1;
            if((0!=strcmp(buff,buff2))||(size1!=size2)){
            // if(true){
                if(err<0.00001){
                    continue;
                }
                printf("err=%f",err);
                printf("%s,size=%d,",buff,size1);
                printf("%s,size=%d,",buff2,size2);
                printf("diff!\n");
                if(0.0!=err){
                    printf("bug num=%d\n",i);
                    // _ASSERT(false);
                }
            }/*else{
                printf("%s,size=%d,",buff,size1);
                printf("%s,size=%d,",buff2,size2);
                printf("correct\n");
            }*/
        }
    }

    return 0;
}
