#include <stdio.h>
#include <time.h>
#include <string.h>
#include <random>
#include <iostream>
#pragma warning(disable:4996)
typedef const char* FILENAME;
typedef long long int64;

#define WRITEBUF(buf,data)  do{*buff=data;buff++;}while(0)

int Double2Ascii(char* buff,double x,int fractional_digits,char separator){
    const char* startAdr=buff;
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
    while(x>=1E10){
        //大きい数字来るときの高速化対策
        x/=1E10;
        exponent+=10;
    }
    while (x >= 10) {
        x /= 10;
        exponent++;
    }
    while (x < 1) {
        x *= 10;
        exponent--;
    }

    //四捨五入処理
    double adjust=0.5;  //四捨五入調整用
    for(int i=0;i<fractional_digits;i++){
        adjust/=10;
    }
    x=x+adjust;

    //四捨五入後、10以上になる場合、10以下にする
    while(x>=10){
        x/=10;
        exponent++;
    }

    //整数部
    int digit=(int)x%10;
    WRITEBUF(buff,digit+'0');

    WRITEBUF(buff,separator);   //小数点を書く
    int multiple=10;    //倍数
    for(int i=0;i<fractional_digits;i++){
        //小数部
        int digit=(int64)(x*multiple)%10;
        WRITEBUF(buff,digit+'0');
        multiple*=10;
    }

    //指数部
    WRITEBUF(buff,'E');
    if(exponent<0){
        exponent=-exponent;
        WRITEBUF(buff,'-');
    }else{
        WRITEBUF(buff,'+');
    }
    if(exponent>99);    //指数が二桁以上はかけない
    else{
        int digit=(exponent/10)%10;
        WRITEBUF(buff,digit+'0');
        digit=exponent%10;
        WRITEBUF(buff,digit+'0');
    }

    WRITEBUF(buff,0);

    //NULL文字はカウントしない
    return (buff - startAdr-1);
}

int main()
{
    char buff[128];
    char buff2[128];
    if(1){
        printf("効率確認\n");
        int looptimes=1E7;
        double data=1.2324543E11;
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

    if(0){
        printf("個別確認\n");
        double data=1E74;
        Double2Ascii(buff,data,4,'.');
        printf("%s\n",buff);
        sprintf(buff2,"%11.4E",data);
        printf("%s\n",buff2);
    }

    if(0){
        printf("正確性確認\n");
        for(int times=0;times<100;times++){
            long double max=pow(10,times);
            printf("max=%f\n",max);
            std::random_device rd;
            std::mt19937 gen(rd());
            std::uniform_real_distribution<> dist(0,max );
        
            for (int i = 0; i < 1000; i++) {
                double data=dist(gen);
                int size1=Double2Ascii(buff,data,4,'.');
                int size2=sprintf(buff2,"%11.4E",data);
                double err=(atof(buff)-atof(buff2))/atof(buff);     //誤差
                if(err<0)   err=err*-1;
                if((0!=strcmp(buff,buff2))||(size1!=size2)){
                // if(true){
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

    return 0;
}
