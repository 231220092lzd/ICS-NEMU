#include <stdint.h>
#include <stdbool.h>
#define MAX UINT64_MAX
uint64_t overflow_mod_m=0;
uint64_t mod(uint64_t a, uint64_t m);
uint64_t plusmod(uint64_t current,uint64_t final,uint64_t m);
uint64_t mul(uint64_t b,uint64_t i,uint64_t m);
uint64_t multimod(uint64_t a, uint64_t b, uint64_t m){
    a=mod(a,m);
    b=mod(b,m);
    overflow_mod_m=mod(MAX,m)+1;
    uint64_t final=0;
    uint64_t i=0;
    while(a!=0){
        if((a&1)==1){
            uint64_t current=mul(b,i,m);
            final=plusmod(current,final,m);
        }
        a=a>>1;
        i++;
    }
    return final;
}

uint64_t mul(uint64_t b,uint64_t i,uint64_t m){
    for(uint64_t shift=0;shift<i;shift++){
        b=plusmod(b,b,m);
    }
    return b;
}

uint64_t mod(uint64_t a,uint64_t m){
    while(a>=m){
        uint64_t max=0;
        uint64_t middle=m;
        if(middle>=(MAX>>2)){ 
          max=1;
        }else{ 
           while(middle<=a&&middle<(MAX>>2)){
              max++;
              middle=middle<<1;
          }
        }
        uint64_t max_m=m<<(max-1);
        while(a>=max_m){
            a-=max_m;
        }
    }
    return a;
}

uint64_t plusmod(uint64_t current,uint64_t final,uint64_t m){  //(a+b)mod m
    uint64_t result=current+final;
    if(result<current){
       result=mod(result,m);
       if(result<=(MAX)-overflow_mod_m){
          result=mod((result+overflow_mod_m),m);//still overflow?
        }else{ 
          while(result>(MAX)-overflow_mod_m){ 
              result=mod((result+overflow_mod_m),m);
          }
          result=mod((result+overflow_mod_m),m);
        }
    }else{
        result=mod(result,m);
    }
    return result;
}
