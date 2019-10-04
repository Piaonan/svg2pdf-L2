#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include "xml.h"
#define FAIL() {fprintf(stderr,"memory allocation failure in xml\n"); exit(EXIT_FAILURE);}
 

xelement_t* create_xelement(const char* name){
    xelement_t* elem = (xelement_t*)malloc(sizeof(xelement_t));
    if (elem==NULL) FAIL()
    elem->listAttrib=NULL;
    elem->name=(char*)malloc(sizeof(char)*strlen(name)+1);
    if (elem->name == NULL) FAIL()
    strcpy(elem->name,name);
    elem->father=NULL;
    elem->brother=NULL;
    elem->contenu.raw=NULL;
    elem->contenu.listSon=NULL;
    return elem;
}

xattribute_t* add_xattribute(xelement_t* e, const char* name, const char* value){
    xattribute_t* temp = e->listAttrib;
    xattribute_t* prev=NULL;
    while(temp!=NULL){
        if(!strcmp(temp->name,name)) {
            free(temp->value);
            temp->value=(char*)malloc(sizeof(char)*strlen(value)+1);
            if(temp->value==NULL) FAIL()
            strcpy(temp->value,value);
            return temp;
        }
        prev=temp;
        temp=temp->next;
    }
    if(prev!=NULL){ //ajout à la fin de la liste des attributs
		prev->next=(xattribute_t*)malloc(sizeof(xattribute_t));
		prev->next->next=NULL;
		if((prev->next->name=(char*)malloc(sizeof(char)*strlen(name)+1))==NULL) FAIL()
		strcpy(prev->next->name,name);
		if((prev->next->value=(char*)malloc(sizeof(char)*strlen(value)+1))==NULL) FAIL()
		strcpy(prev->next->value,value);
		return prev->next;
    }else {
		e->listAttrib=(xattribute_t*)malloc(sizeof(xattribute_t));
		e->listAttrib->next=NULL;
		if((e->listAttrib->name=(char*)malloc(sizeof(char)*strlen(name)+1))==NULL) FAIL()
		strcpy(e->listAttrib->name,name);
		if((e->listAttrib->value=(char*)malloc(sizeof(char)*strlen(value)+1))==NULL) FAIL()
		strcpy(e->listAttrib->value,value);
		return e->listAttrib;
    }
}

void add_sub_xelement(xelement_t* e, xelement_t* s){
    if(s->father!=NULL||(e->contenu.raw!=NULL&&e->contenu.listSon->father!=e)){
        fprintf(stderr,"Erreur, l'élément %s a 1 parent ou l'élement %s contient 1 donnée brute.\n",s->name,e->name);
        exit(EXIT_FAILURE);
    }
    xelement_t* temp=e->contenu.listSon;
    xelement_t* prev=NULL;
    while(temp!=NULL){
        prev=temp;
        temp=temp->brother;
    }
    if(prev!=NULL){ //ajout à la fin de la liste des éléments fils
        prev->brother=s;
        prev->brother->father=e;
    }else {
        e->contenu.listSon=s;
        e->contenu.listSon->father=e;
    }
}

void add_raw(xelement_t* e, const char* r){
    if(e->contenu.listSon!=NULL&&e->contenu.listSon->father==e){
        fprintf(stderr,"Erreur, l'élément %s contient des sous-attributs.\n",e->name);
        exit(EXIT_FAILURE);
    }
    if(e->contenu.raw!=NULL){
        free(e->contenu.raw);
    }
    e->contenu.raw=(char*)malloc(sizeof(char)*strlen(r)+1);
    if(e->contenu.raw==NULL) FAIL()
    strcpy(e->contenu.raw,r);
}

void delete_xelement(xelement_t* e) {
    if(e->father==NULL){
        xattribute_t* temp= e->listAttrib;
        xattribute_t* prev=NULL;
        while(temp!=NULL){
            prev = temp;
            temp = temp->next;
            free(prev->name); free(prev->value);
            free(prev);
        }
        if(e->contenu.listSon!=NULL&&e->contenu.listSon->father==e) {
            e->contenu.listSon->father=NULL;
            delete_xelement(e->contenu.listSon);
        }
        else if(e->contenu.raw!=NULL){
            free(e->contenu.raw);
        }
        if(e->brother!=NULL){
            e->brother->father=NULL;
            delete_xelement(e->brother);
        }
        free(e->name);
        free(e);
    }
    else fprintf(stderr,"Erreur, l'élément %s a 1 parent.\n",e->name);

}

void space(FILE* fd, xelement_t* e){
	xelement_t* temp = e->father; int i = 0; int cpt = 0;
	while (temp!=NULL){
		temp=temp->father; 
		cpt++;
	}
	for(i=0; i<cpt; i++) fprintf(fd,"  ");
}

void save_xelement(FILE* fd, xelement_t* e){
    xattribute_t* temp=e->listAttrib; 
	space(fd,e);
    fprintf(fd,"<%s",e->name);
    while(temp!=NULL){ //printf les attributs
        fprintf(fd," %s=\"%s\"",temp->name, temp->value);
        temp=temp->next;
    }
    if(e->contenu.raw==NULL)fprintf(fd," />\n");
    else{
        if(e->contenu.listSon!=NULL&&e->contenu.listSon->father==e){ //printf les éléments fils
            fprintf(fd,">\n");
            save_xelement(fd,e->contenu.listSon);
			space(fd,e);
            fprintf(fd,"</%s>\n",e->name);
        }
        else fprintf(fd,">%s</%s>\n",e->contenu.raw,e->name); //sinon print la donnée brute
    }
    if(e->brother!=NULL)save_xelement(fd,e->brother);

}

void save_xml(const char* fname, xelement_t* e){
    FILE* fd=fopen(fname,"w+");
    if (fd==NULL) {
        fprintf(stderr,"Erreur d'accès sur le fichier (%s).\n",fname);
        exit(EXIT_FAILURE);
    }
    save_xelement(fd,e);
    fclose(fd);
}

void print_xelement(xelement_t* e) {
  save_xelement(stdout, e);
}

char next_char(FILE* fd){
	char c = fgetc(fd);
	while(c==' '||c=='\n'||c=='\t'||c=='\r'||c==EOF){
		if(c==EOF) {
			fprintf(stderr,"Erreur, fin du fichier atteinte.\n");
			exit(EXIT_FAILURE);
		}
		c=fgetc(fd);
	}
	return c;
}

void check_next_char(FILE* fd, char c) {
    if(next_char(fd)!=c){
        fprintf(stderr,"Erreur, le prochain caractère n'est pas celui attendu.\n");
        exit(EXIT_FAILURE);
    }
}

bool is_next_char(FILE* fd, char c, bool cons) {
    char next = next_char(fd);
    if(next!=c) {
        ungetc(next,fd);
        return false;
    }
    if(!cons) ungetc(next,fd);
    return true;
}

char* next_word(FILE* fd){
    char *word=(char*)calloc(1,sizeof(char));
    if(word==NULL) FAIL()
    int i=0; //compteur pour incrémenter la mémoire et pour savoir si un identifiant suivant existe
    char temp[2]={0};
    temp[0]= next_char(fd);
    while(temp[0]!=' '&&temp[0]!='='&&temp[0]!='/'&&temp[0]!='>'&&temp[0]!='<'&&temp[0]!='\n'&&temp[0]!='\t'&&temp[0]!='\r'){
        if(temp[0]==EOF) {
            fprintf(stderr,"Erreur, fin du fichier atteinte.\n");
            exit(EXIT_FAILURE);
        }
        i++;
        word=(char*)realloc(word,i*sizeof(char)+1);
        if(word==NULL) FAIL()
        strcat(word,temp);
        temp[0]=fgetc(fd);
    }
    ungetc(temp[0],fd);
    if(i==0) return NULL;
    return word;
}

void check_next_word(FILE* fd, const char* w){
    if(strcmp(next_word(fd),w)){
        fprintf(fd,"Erreur, le mot comparé est différent de celui attendu.\n");
        exit(EXIT_FAILURE);
    }
}

char* next_string(FILE* fd){
    char *string=(char*)calloc(1,sizeof(char));
    if(string==NULL) FAIL()
    int i=0; char temp[2]={0}; temp[0]= next_char(fd);
    while((temp[0]= fgetc(fd))!='"'){
        if(temp[0]==EOF) {
            fprintf(stderr,"Erreur, fin du fichier atteinte.\n");
            exit(EXIT_FAILURE);
        }
        i++;
        string=(char*)realloc(string,i*sizeof(char)+1);
        if(string==NULL) FAIL()
        strcat(string,temp);
    }
    return string;
}

char* next_raw(FILE* fd){
    char *raw=(char*)calloc(1,sizeof(char));
    if(raw==NULL) FAIL()
    int i=0; char temp[2]={0};
    while((temp[0]= fgetc(fd))!='<'){
        if(temp[0]==EOF) {
            fprintf(stderr,"Erreur, fin du fichier atteinte.\n");
            exit(EXIT_FAILURE);
        }
        i++;
        raw=(char*)realloc(raw,i*sizeof(char)+1);
        if(raw==NULL) FAIL()
        strcat(raw,temp);
    }
    ungetc(temp[0],fd);
    return raw;
}

void load_xelement_raw(FILE* fd, xelement_t* e) {
  char* w = next_raw(fd);
  check_next_char(fd, '<');
  check_next_char(fd, '/');
  check_next_word(fd, e->name);
  check_next_char(fd, '>');
  add_raw(e,w);
  free(w);
}

void load_xelement_sub(FILE* fd, xelement_t* e) {
  xelement_t* f = load_xelement(fd, e->name);
  if (f != NULL) {
    add_sub_xelement(e,f);
    load_xelement_sub(fd, e);
  }
}

void load_xelement_content(FILE* fd, xelement_t* e) {
  if (is_next_char(fd,'<',false))
    load_xelement_sub(fd, e);
  else
    load_xelement_raw(fd, e);
}

xelement_t* load_xelement(FILE* fd, const char* end_tag) {
  xelement_t* e = NULL;
  char c;

  check_next_char(fd,'<');

  if ((end_tag) && (is_next_char(fd,'/',true))) {
      check_next_word(fd,end_tag);
      check_next_char(fd,'>');
      return NULL;
  }

  char* name = next_word(fd);
  if (name == NULL) {
      fprintf(stderr, "load_xelement: tag name expected\n");
      exit(EXIT_FAILURE);
  }
  e = create_xelement(name);
  free(name);

  while((name = next_word(fd)) != NULL) {
    check_next_char(fd,'=');
    char* value = next_string(fd);
    add_xattribute(e,name,value);
  }

  c = next_char(fd);

  if (c == '/') {
    check_next_char(fd,'>');
    return e;
  }

  if (c == '>') {
    load_xelement_content(fd, e);
    return e;
  }
  
  fprintf(stderr, "load_xelement: end of markup expected ('>' or '/>'), but got %c\n", c);
  exit(EXIT_FAILURE);
}

xelement_t* load_xml(const char* fname) {
  FILE* fd = NULL;
  if ( (fd = fopen(fname, "r"))==NULL ){
    fprintf(stderr,"Erreur, fichier (%s) inexistant.\n",fname);
    exit(EXIT_FAILURE);
  }
  xelement_t* e = load_xelement(fd,NULL);
  fclose(fd);
  return e;
}