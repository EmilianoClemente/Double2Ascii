#include <stdio.h>
#include <time.h>
#include <string.h>
#include <random>
#include <iostream>
#pragma warning(disable:4996)
typedef const char* FILENAME;
typedef long long int64;

#define WRITEBUF(buf,data)  do{*buff=data;buff++;}while(0)

void combineFile(FILENAME main,FILENAME add){
    FILE* fmain=fopen(main,"a+");
    FILE* fadd=fopen(add,"r");
    FILE* fdes=fopen("des.csv","a+");
    char buffer=0;
    FILE* cur=fmain;
    if((fmain==NULL)||(fadd==NULL)) goto FINALLY;

    while(true){
        while(true){
            if(feof(cur)) goto FINALLY;
            fread(&buffer,1,1,cur);
            if('\n'==buffer)break;
            fwrite(&buffer,1,1,fdes);
        }
        if(fmain==cur)  cur=fadd;
        else{
            buffer='\n';
            fwrite(&buffer,1,1,fdes);
            cur=fmain;
        }
    }



FINALLY:
    if(fmain!=NULL) fclose(fmain);
    if(fadd!=NULL)  fclose(fadd);
}

int Double2Ascii(char* buff,double x,int fractional_digits,char separator){
    const char* startAdr=buff;
    char sign=' ';
    if(x<0){
        //0以下の時、整数にし、符号を変える
        x=-x;
        sign='-';
    }
    WRITEBUF(buff,sign);

    //仮数部を1から10の間になるように調整
    int exponent=0;
    if(x>=10){
        while (x >= 10) {
            x /= 10;
            exponent++;
        }
    }
    else if(x<1&&x>0){
        while (x < 1) {
            x *= 10;
            exponent--;
        }
    }

    //四捨五入処理
    double round_num=0.5;
    for(int i=0;i<fractional_digits;i++){
        round_num/=10;
    }
    x=x+round_num;

    //四捨五入後、10以上になる場合、10以下にする
    if(x>=10){
        x/=10;
        exponent++;
    }

#if 0   //整数部が1桁以上のときの処理
    int start=1;
    if(x>=100.0){
        start=100;
    }
    else if(x>=10.0){
        start=10;
    }
    else{
        start=1;
    }

    for(int i=start;i>=1;i/=10){
        //整数部
        int digit=(int)(x/i)%10;
        WRITEBUF(buff,digit+'0');
    }
#else
    int digit=(int)x%10;
    WRITEBUF(buff,digit+'0');
#endif

    WRITEBUF(buff,separator);   //小数点を書く
    int times=10;
    for(int i=0;i<fractional_digits;i++){
        //小数部
        int digit=(int64)(x*times)%10;
        WRITEBUF(buff,digit+'0');
        times*=10;
    }

    if(/*exponent!=0*/true){
        //指数部
        WRITEBUF(buff,'E');
        if(exponent<0){
            exponent=-exponent;
            WRITEBUF(buff,'-');
        }else{
            WRITEBUF(buff,'+');
        }
        if(exponent>99);
        else{
            int digit=(exponent/10)%10;
            WRITEBUF(buff,digit+'0');
            digit=exponent%10;
            WRITEBUF(buff,digit+'0');
        }
    }

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
        int looptimes=1E7;
        double data=1.12348888;
        time_t start=time(NULL);
        for(int i=0;i<looptimes;i++){
            Double2Ascii(buff,data,4,'.');
        }
        time_t end=time(NULL);
        printf("%lld",end-start);
        start=time(NULL);
        for(int i=0;i<looptimes;i++){
            sprintf(buff, "%11.4E", data);
        }
        end=time(NULL);
        printf("%lld",end-start);
    }

    if(1){
        printf("個別確認\n");
        double data=1E74;
        Double2Ascii(buff,data,4,'.');
        printf("%s\n",buff);
        sprintf(buff2,"%11.4E",data);
        printf("%s\n",buff2);
    }

    if(1){
        printf("正確性確認\n");
        for(int times=0;times<100;times++){
            long double max=pow(10,times);
            printf("max=%f\n",max);
            std::random_device rd;
            std::mt19937 gen(rd());
            std::uniform_real_distribution<> dist(0,max );
        
            for (int i = 0; i < 1000; i++) {
                double data=dist(gen);
                int size1=Double2Ascii(buff,data,5,'.');
                int size2=sprintf(buff2,"%12.5E",data);
                double err=(atof(buff)-atof(buff2))/atof(buff);
                if(err<0)   err=err*-1;
                if((0!=strcmp(buff,buff2))||(size1!=size2)/*&&(err>0.0001)*/){
                // if(true){
                    printf("err=%f",err);
                    printf("%s,size=%d,",buff,size1);
                    printf("%s,size=%d,",buff2,size2);
                    printf("diff!\n");
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
