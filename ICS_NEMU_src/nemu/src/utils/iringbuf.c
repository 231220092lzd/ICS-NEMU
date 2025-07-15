#include <common.h>
#ifdef IRINGBUF
struct{ 
  int address;
  char* instruct;
  listnode* next;
  listnode* back;
}listnode;
typedef struct listnode listnode;
int maxlength=20;
int curlength=0;
listnode* head=(listnode*)malloc(sizeof(listnode*));
listnode* tail=(listnode*)malloc(sizeof(listnode*));
void iringbuf_store(int ad,char* ins){ 
    if(curlength==maxlength){
      listnode* cur=tail->back;
      tail->back=cur->back;
      tail->back->next=tail;
      free(cur);
    }
    listnode* newnode=(listnode*)malloc(sizeof(listnode*));
    newnode->address=ad;
    newnode->instruct=(char*)malloc((strlen(ins)+1)*sizeof(char*));
    strcpy(newnode->instruct,ins);
    newnode->next=head->next;
    newnode->back=head;
    newnode->next->back=newnode;
    head->next=newnode;
  }
void iringbuf_show(){
  listnode* current=head->next;
  while(current!=tail){
    printf("0x%x: %s\n",current->address,current->instruct);
    current=current->next;
  }
}
#endif
