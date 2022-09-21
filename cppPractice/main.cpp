#include <corecrt_math.h>
#include <time.h>
#include <random>
#include <iostream>
#include "double2ascii.h"
#pragma warning(disable:4996)

template<int plus>
const char* GetExponentFormat(int decimal_place){
    static char format[10];
    format[0]='%';
    sprintf(&format[1],"%d.%dE",decimal_place+7+plus,decimal_place);
    return format; 
}

int main()
{
    char buff[128];
    char buff2[128];
    const char* format=GetExponentFormat<0>(DECIMAL_PLACE);
    const char* formatPlus1=GetExponentFormat<1>(DECIMAL_PLACE);
    if(0){
        printf("べき乗が違う入力で効率を確認\n");
        for (double exponent=-100;exponent<100;exponent+=10){
            /* べき乗が違う入力で効率を確認 */
            //int looptimes=(int)30E7;
            //double data=15.9672;
            printf("exponent=%f\n",exponent);
            int looptimes = 1E7;
            double data = pow(10.0,exponent);
            time_t start=time(NULL);
            for(int i=0;i<looptimes;i++){
                Double2Ascii<DECIMAL_PLACE>(buff,data,'.');
            }
            time_t end=time(NULL);
            printf("looptimes=%d,template version cost %lld s\n",looptimes,end-start);
        }
       // start=time(NULL);
       // for(int i=0;i<looptimes;i++){
       //     sprintf(buff, format, data);
       // }
       // end=time(NULL);
       // printf("looptimes=%d,sprintf cost %lld s\n",looptimes,end-start);
    }
    if(1){
        printf("効率確認\n");
        int looptimes=(int)30E7;
        double data=15.9672;
        time_t start=time(NULL);
        for(int i=0;i<looptimes;i++){
            Double2Ascii<DECIMAL_PLACE>(buff,data,'.');
        }
        time_t end=time(NULL);
        printf("looptimes=%d,template version cost %lld s\n",looptimes,end-start);
       // start=time(NULL);
       // for(int i=0;i<looptimes;i++){
       //     sprintf(buff, format, data);
       // }
       // end=time(NULL);
       // printf("looptimes=%d,sprintf cost %lld s\n",looptimes,end-start);
    }

    if(0){
        printf("個別確認\n");
        double data;
        data=100244999999999.984375;    //indivisible sample
        // data=173725700000000.000000;     //example data for testing banker's round
        // Double2Ascii(buff,data,DECIMAL_PLACE,'.');
        Double2Ascii<DECIMAL_PLACE>(buff,data,'.');
        printf("%s\n",buff);
        sprintf(buff2,"%11.4E",data);
        printf("%s\n",buff2);
        INT64* pDATA=(INT64*)(&data);
        printf("%f,%llx,",data,*pDATA);
    }

    if(0){
        printf("ランダム入力 正確性確認\n");
        for(int times=0;times<304;times++){
            long double max=pow(10,times);
            //printf("max=%Lf\n",max);
            std::random_device rd;
            std::mt19937 gen(rd());
            std::uniform_real_distribution<> dist(0,max );
        
            for (int i = 0; i < 1000; i++) {
                double data=dist(gen);
                // int size1=Double2Ascii(buff,data,4,'.');
                size_t size1=Double2Ascii<DECIMAL_PLACE>(buff,data,'.');
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
                    printf("%s,size=%lld,",buff,size1);
                    printf("%s,size=%d,",buff2,size2);
                    printf("%.11f,",data);
                    printf("diff!\n");
                    _ASSERT(false);
                }else{
//                    printf("%s,size=%d,",buff,size1);
//                    printf("%s,size=%d,",buff2,size2);
//                    printf("correct\n");
                }
            }
        }
    }

    if(0){
        printf("固定入力 正確性確認");
        //11桁の数字で確認
        int err_cnt=0;
        for(INT64 i=1000000000;i<10000000000;i++){
            double data=(double)i/1E-5;
            // int size1=Double2Ascii(buff,data,DECIMAL_PLACE,'.');
            size_t size1=Double2Ascii<DECIMAL_PLACE>(buff,data,'.');
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
                printf("%s,size=%lld,",buff,size1);
                printf("%s,size=%d,",buff2,size2);
                printf("%.7f,",data);
                INT64* pDATA=(INT64*)(&data);
                printf("%llx,",*pDATA);
                printf("diff!\n");
                printf("bug num=%lld\n",i);
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
