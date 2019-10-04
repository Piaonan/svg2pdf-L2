#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include "menu.h"
#define FAIL() {fprintf(stderr,"memory allocation failure in menu\n"); exit(EXIT_FAILURE);}


menu_t* createMenu(const char* text){
	menu_t* m = NULL;
	m = (menu_t*)malloc(sizeof(menu_t));
	if(m==NULL) FAIL()
	m->papa = NULL;
	m->description=(char*)malloc(sizeof(char)*strlen(text)+1);
	if(m->description==NULL) FAIL()
	strcpy(m->description,text);
	m->nbItem=0;
	return m;
}

void addMenuAction(menu_t* m, const char* text, void(*f)()){
    m->set[m->nbItem].action.description=(char*)malloc(sizeof(char)*strlen(text)+1);
	if(m->set[m->nbItem].action.description==NULL) FAIL()
    strcpy(m->set[m->nbItem].action.description, text);
    m->set[m->nbItem].action.fonction=f;
    m->nbItem++;
}

void addSubMenu(menu_t* m, menu_t* sm){
	if(sm->papa==NULL&&(m->nbItem)<9){
		sm->papa = m;
		m->set[m->nbItem].sous_menu= sm;
		m->nbItem++;
   }else printf("Plus de place pour un sous-menu.\n");
}

void deleteMenu(menu_t* m){
    if(m->papa==NULL){
        int i;
        for(i=0; i<(m->nbItem); i++){
            if(m->set[i].sous_menu->papa!=m) free(m->set[i].action.description);
            else {
				m->set[i].sous_menu->papa=NULL;
				deleteMenu(m->set[i].sous_menu);
			}
        }
    free(m->description);
    free(m);
    } else fprintf(stderr,"Le menu %s possède 1 père.\n",m->description);
}

void launchMenu(menu_t* m){
	printf("\n-----------------------------------------------\n\n");
	printf("%s : \n\n",m->description);
	int i; char choice;
	for(i=0; i< (m->nbItem); i++){
		if(m->set[i].sous_menu->papa==m){
			printf(" %d - %s\n",i+1, m->set[i].sous_menu->description);
		}
		else printf(" %d - %s\n",i+1, m->set[i].action.description);
	}
	printf("\nChoice? : ");
	choice = getchar(); while(choice!='\n'&&getchar()!='\n');
	int numeral = (int)choice-(int)'0';
	while((numeral>(m->nbItem)||numeral<1)&&(choice!='p'||m->papa==NULL)){
		printf("\nNot found, choice? : ");
		choice = getchar(); while(choice!='\n'&&getchar()!='\n');
		numeral = (int)choice-(int)'0';
	}
	if(choice=='p'){
		if(m->papa!=NULL){
			launchMenu(m->papa);
		}
	}
	else {
		if(m->set[numeral-1].sous_menu->papa==m) launchMenu(m->set[numeral-1].sous_menu);
		else (m->set[numeral-1].action.fonction)();
	}
}