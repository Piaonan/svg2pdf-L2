#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#define FAIL() {fprintf(stderr,"memory allocation failure in args\n"); exit(EXIT_FAILURE);}
#include "args.h"


option_t* opt_void(option_t* l, const char* kw, void (*f)()){
    option_t* temp=l;
    option_t* prev=NULL;
    while(temp!=NULL){
        prev=temp;
        temp=temp->next;
    }
    if(prev==NULL){
        if((l=(option_t*)malloc(sizeof(option_t)))==NULL) FAIL()
        l->keyword=kw;
        l->spec=OptVoid;
        l->fct.opt_void=f;
        l->next=NULL;
    }else{
        if((prev->next=(option_t*)malloc(sizeof(option_t)))==NULL) FAIL()
        prev->next->keyword=kw;
        prev->next->spec=OptVoid;
        prev->next->fct.opt_void=f;
        prev->next->next=NULL;
    }
    return l;
}

option_t* opt_int(option_t* l, const char* kw, void (*f)(int)){
    option_t* temp=l;
    option_t* prev=NULL;
    while(temp!=NULL){
        prev=temp;
        temp=temp->next;
    }
    if(prev==NULL){
        if((l=(option_t*)malloc(sizeof(option_t)))==NULL) FAIL()
        l->keyword=kw;
        l->spec=OptInt;
        l->fct.opt_int=f;
        l->next=NULL;
    }else{
        if((prev->next=(option_t*)malloc(sizeof(option_t)))==NULL) FAIL()
        prev->next->keyword=kw;
        prev->next->spec=OptInt;
        prev->next->fct.opt_int=f;
        prev->next->next=NULL;
    }
    return l;
}

option_t* opt_float(option_t* l, const char* kw, void (*f)(float)){
    option_t* temp=l;
    option_t* prev=NULL;
    while(temp!=NULL){
        prev=temp;
        temp=temp->next;
    }
    if(prev==NULL){
        if((l=(option_t*)malloc(sizeof(option_t)))==NULL) FAIL()
        l->keyword=kw;
        l->spec=OptFloat;
        l->fct.opt_float=f;
        l->next=NULL;
    }else{
        if((prev->next=(option_t*)malloc(sizeof(option_t)))==NULL) FAIL()
        prev->next->keyword=kw;
        prev->next->spec=OptFloat;
        prev->next->fct.opt_float=f;
        prev->next->next=NULL;
    }
    return l;
}

option_t* opt_string(option_t* l, const char* kw, void (*f)(const char*)){
    option_t* temp=l;
    option_t* prev=NULL;
    while(temp!=NULL){
        prev=temp;
        temp=temp->next;
    }
    if(prev==NULL){
        if((l=(option_t*)malloc(sizeof(option_t)))==NULL) FAIL()
        l->keyword=kw;
        l->spec=OptString;
        l->fct.opt_str=f;
        l->next=NULL;
    }else{
        if((prev->next=(option_t*)malloc(sizeof(option_t)))==NULL) FAIL()
        prev->next->keyword=kw;
        prev->next->spec=OptString;
        prev->next->fct.opt_str=f;
        prev->next->next=NULL;
    }
    return l;
}

option_t* opt_floatfloat(option_t* l, const char* kw, void (*f)(float, float)){
    option_t* temp=l;
    option_t* prev=NULL;
    while(temp!=NULL){
        prev=temp;
        temp=temp->next;
    }
    if(prev==NULL){
        if((l=(option_t*)malloc(sizeof(option_t)))==NULL) FAIL()
        l->keyword=kw;
        l->spec=OptFloatFloat;
        l->fct.opt_floatfloat=f;
        l->next=NULL;
    }else{
        if((prev->next=(option_t*)malloc(sizeof(option_t)))==NULL) FAIL()
        prev->next->keyword=kw;
        prev->next->spec=OptFloatFloat;
        prev->next->fct.opt_floatfloat=f;
        prev->next->next=NULL;
    }
    return l;
}

option_t* opt_intfloat(option_t* l, const char* kw, void (*f)(int, float)){
    option_t* temp=l;
    option_t* prev=NULL;
    while(temp!=NULL){
        prev=temp;
        temp=temp->next;
    }
    if(prev==NULL){
        if((l=(option_t*)malloc(sizeof(option_t)))==NULL) FAIL()
        l->keyword=kw;
        l->spec=OptIntFloat;
        l->fct.opt_intfloat=f;
        l->next=NULL;
    }else{
        if((prev->next=(option_t*)malloc(sizeof(option_t)))==NULL) FAIL()
        prev->next->keyword=kw;
        prev->next->spec=OptIntFloat;
        prev->next->fct.opt_intfloat=f;
        prev->next->next=NULL;
    }
    return l;
}

option_t* opt_intfloatfloat(option_t* l, const char* kw, void (*f)(int, float, float)){
    option_t* temp=l;
    option_t* prev=NULL;
    while(temp!=NULL){
        prev=temp;
        temp=temp->next;
    }
    if(prev==NULL){
        if((l=(option_t*)malloc(sizeof(option_t)))==NULL) FAIL()
        l->keyword=kw;
        l->spec=OptIntFloatFloat;
        l->fct.opt_intfloatfloat=f;
        l->next=NULL;
    }else{
        if((prev->next=(option_t*)malloc(sizeof(option_t)))==NULL) FAIL()
        prev->next->keyword=kw;
        prev->next->spec=OptIntFloatFloat;
        prev->next->fct.opt_intfloatfloat=f;
        prev->next->next=NULL;
    }
    return l;
}

void opt_delete(option_t* l) {
    if(l->next!=NULL){
        opt_delete(l->next);
    }
    free(l);
}

void process_options(option_t* l, int argc, char* *argv){
    int i; option_t* temp=NULL;
    for(i = 1; i < argc; i++){
        temp=l;
        if(argv[i][0]=='-'){
            while(strcmp(temp->keyword,argv[i])){
                temp=temp->next;
                if(temp==NULL) {fprintf(stderr,"Option not found\n"); exit(EXIT_FAILURE);}
            }
            switch(temp->spec){
                case 0: temp->fct.opt_void();
                        break;
                case 1: i++;
                        temp->fct.opt_int(atoi(argv[i]));
                        break;
                case 2: i++;
                        temp->fct.opt_str(argv[i]);
                        break;
                case 3: i++;
                        temp->fct.opt_float(atof(argv[i]));
                        break;
				case 4: i+=2;
						temp->fct.opt_floatfloat(atof(argv[i-1]),atof(argv[i]));
						break;				
				case 5: i+=2;
						temp->fct.opt_intfloat(atoi(argv[i-1]),atof(argv[i]));
						break;
				case 6: i+=3;
						temp->fct.opt_intfloatfloat(atoi(argv[i-2]),atof(argv[i-1]),atof(argv[i]));
						break;
            }
        }
    }
}