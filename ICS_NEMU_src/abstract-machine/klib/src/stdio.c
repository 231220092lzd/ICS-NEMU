#include <am.h>
#include <klib.h>
#include <klib-macros.h>
#include <stdarg.h>

#if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)

int printf(const char *fmt, ...) {
  //panic("Not implemented");
  va_list input;
  va_start(input,fmt);
  char output[2024]={'\0'};
  int final=vsprintf(output,fmt,input);
  va_end(input);
  for(int i=0;output[i]!='\0';i++){ 
    putch(output[i]);
  }
  return final;
}

int vsprintf(char *out, const char *fmt, va_list ap) {
  //panic("Not implemented");
  int final=0;
  for(int i=0;fmt[i]!='\0';i++){ 
    if(fmt[i]=='%'){
      bool fillzero=false;
      int filllength=0;
      char middle[24]={'\0'};
      i++;
      if(fmt[i]=='0'){ 
        i++;
        fillzero=true;
      }
      for(int j=0;fmt[i]<='9'&&fmt[i]>='0';i++,j++){ 
        middle[j]=fmt[i];
        middle[j+1]='\0';
      }
      for(int k=strlen(middle)-1,m=1;k>=0;k--,m*=10){ 
        filllength+=(middle[k]-'0')*m;
      }
      switch(fmt[i]){ 
        case 's':{
                char* target=va_arg(ap,char*);
                strcpy(out,target);
                out+=strlen(target);
                final+=strlen(target);
                break;
                 }
        case 'd':{
                int target=va_arg(ap,int);
                int length=0;
                int reallength;
                bool underzero=false;
                if(target==0){
                  length=1;//如果是0就长度为1避免下面不进循环，如果是负数就要多一个-号位
                }else if(target<0){
                  length=1;
                  target=-target;
                  underzero=true;
                }
                for(int middle=target;middle>0;length++){
                        middle/=10;
                }
                if(filllength>length){ 
                  reallength=filllength;
                }else{ 
                  reallength=length;    
                }
                for(int j=reallength-1,k=(int)underzero;k<length;j--,k++){
                  out[j]='0'+target%10;
                  target/=10;
                  if(k==length-1&&underzero){
                    out[j-1]='-';
                  }
                }
                if(filllength>length){
                  char cur;
                  if(fillzero==true){
                    cur='0';
                  }else{
                    cur=' ';
                  }
                  if(!underzero){
                    for(int j=0;j<reallength-length;j++){
                      out[j]=cur;
                    }
                  }else{
                    for(int j=0;j<=reallength-length;j++){
                      out[j]=cur;
                    }
                    if(fillzero){
                      out[0]='-';
                    }else{
                      out[reallength-length]='-';
                    }
                    
                  } 
                }
                final+=reallength;
                out[reallength]='\0';
                out+=reallength;
                break;}
          }
        }else{ 
          *out++=fmt[i];
          *out='\0';
          final++;
        }
      }
  return final;
}

int sprintf(char *out, const char *fmt, ...) {//负数没处理
  //panic("Not implemented");
  va_list input;
  va_start(input,fmt);
  int final=0;
  final=vsprintf(out,fmt,input);
  va_end(input);
  return final;
}

int snprintf(char *out, size_t n, const char *fmt, ...) {
  panic("Not implemented");
}

int vsnprintf(char *out, size_t n, const char *fmt, va_list ap) {
  panic("Not implemented");
}

#endif
