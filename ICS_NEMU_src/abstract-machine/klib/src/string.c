#include <klib.h>
#include <klib-macros.h>
#include <stdint.h>

#if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)

size_t strlen(const char *s) {
 // panic("Not implemented");
 size_t len=0;
 while(s[len]!='\0'){ 
    len++;
 }
 return len;
}

char *strcpy(char *dst, const char *src) {
  //panic("Not implemented");
  int i=0;
  for(;src[i]!='\0';i++){ 
    dst[i]=src[i];
  }
  dst[i]='\0';
  return dst;

}

char *strncpy(char *dst, const char *src, size_t n) {
  //panic("Not implemented");
  for(int i=0;i<n;i++){ 
    if(src[i]=='\0'){ 
      for(int j=i;j<n;j++){ 
        dst[j]='\0';
      }
      return dst;
    }
   dst[i]=src[i];
  }
  return dst;
}

char *strcat(char *dst, const char *src) {
  //panic("Not implemented");
  int i=0;
  for(;dst[i]!='\0';i++){ 
    continue;    
  }
  int j=i;
  for(;src[j-i]!='\0';j++){ 
    dst[j]=src[j-i];
  }
  dst[j]='\0';
  return dst;
}

int strcmp(const char *s1, const char *s2) {
  //panic("Not implemented");
  int p1=0,p2=0;
  for(;s1[p1]!='\0'&&s2[p2]!='\0';p1++,p2++){ 
    if(s1[p1]==s2[p2]){ 
      continue;
    }else if(s1[p1]>s2[p2]){ 
      return 1;
    }else{ 
      return -1;
    }
  }
  if(s1[p1]!='\0'){ 
    return 1;
  }else if(s2[p2]!='\0'){ 
    return -1;
  }else{ 
    return 0;
  }
}

int strncmp(const char *s1, const char *s2, size_t n) {
 // panic("Not implemented");
 for(int i=0;i<n;i++){ 
    if(s1[i]=='\0'&&s2[i]!='\0'){ 
      return -1;
    }else if(s1[i]!='\0'&&s2[i]=='\0'){ 
      return 1;
    }else if(s1[i]=='\0'&&s2[i]=='\0'){ 
      return 0;
    }
    if(s1[i]>s2[i]){ 
      return 1;
    }else if(s1[i]<s2[i]){ 
      return -1;
    }else{ 
      continue;
    }
  
 }
 return 0;
}

void *memset(void *s, int c, size_t n) {
 //panic("Not implemented");
 unsigned char* cur=s;
 for(int i=0;i<n;i++){ 
    *cur=c;
    cur++;
 }
 return s;
}

void *memmove(void *dst, const void *src, size_t n) {
 // panic("Not implemented");
 const char* cur=(const char*)src;
 char* target=(char*)dst;
 if(target>cur){
    src=src+n-1;
    target=target+n-1;
    for(int i=0;i<n;i++){ 
      *target=*cur;
      cur--;
      target--;
    }
 }else{ 
    for(int i=0;i<n;i++){ 
        *target=*cur;
        cur++;
        target++;
    }
 }
 return dst;
}

void *memcpy(void *out, const void *in, size_t n) {
  //panic("Not implemented");
  const char* cur=(const char*)in;
  char* target=(char*)out;
  for(int i=0;i<n;i++){ 
    *target=*cur;
    cur++;
    target++;
  }
  return out;
}

int memcmp(const void *s1, const void *s2, size_t n) {
  //panic("Not implemented");
  const unsigned char* S1=(const unsigned char*)s1;
  const unsigned char* S2=(const unsigned char*)s2;
  for(int i=0;i<n;i++){ 
    if(*S1>*S2){ 
      return 1;
    }else if(*S1<*S2){ 
      return -1;
    }
  }
  return 0;
}

#endif
