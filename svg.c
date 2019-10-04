#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <stdbool.h>
#include "pdf.h"
#include "xml.h"
#include "svg.h"
#define FAIL() {fprintf(stderr,"memory allocation failure in svg\n"); exit(EXIT_FAILURE);}
#define MAXI 200
#define CHECK() {if(e==NULL){fprintf(stderr,"Vous n'avez chargé aucun fichier.\n"); return;}}


void print_coord(coord_t c) {
	 printf("(%f,%f)", c.x, c.y);
}

coord_t coordinate(double x, double y){
	coord_t point;
	point.x = x;
	point.y = y;
	return point;
}

coord_t inv_translate_point(coord_t p, coord_t v){
	p.x = p.x - v.x;
	p.y = p.y - v.y;
	return p;
}

 coord_t inv_rotate_point(coord_t p, double t){
	 coord_t point;
	 point.x =p.x*cos(-t)-p.y*sin(-t);
	 point.y =p.x*sin(-t) + p.y*cos(-t);
	 return point;
 }

void freeOfList(figure_t* temp){
    if(temp==NULL) return;
    freeOfList(temp->next);
    free(temp);
    temp=NULL;
}

figure_t* circle(double radius){
	figure_t* c=NULL;
	if( (c=(figure_t*)malloc(sizeof(figure_t)))==NULL ) FAIL()
	c->couleur = '+';
	c->centre = coordinate(0,0);
	c->type = Cercle;
	c->cercle.carreRayon =  pow(radius,2);
	c->next=NULL;
	return c;
}

figure_t* rectangle(double width, double height){
	figure_t* r=NULL;
	if( (r=(figure_t*)malloc(sizeof(figure_t)))==NULL ) FAIL()
	r->couleur = '+';
	r->centre = coordinate(0,0);
	r->type = Rectangle;
	r->rectangle.demiLargeur=width/2;
	r->rectangle.demiHauteur=height/2;
	r->next=NULL;
	return r;
}

figure_t* line(double length){
	figure_t* l=NULL;
	if( (l=(figure_t*)malloc(sizeof(figure_t)))==NULL ) FAIL()
	l->couleur = '+';
	l->centre = coordinate(0,0);
	l->type = Ligne;
	l->ligne.demiLongueur=length/2;
	l->next=NULL;
	return l;
}

figure_t* color(figure_t* f, color_t c){
	f->couleur = c;
	return f;
}

figure_t* translate(figure_t* f, double dx, double dy){
	f->centre=coordinate(dx,dy);
	return f;
}

figure_t* rotate(figure_t* f, double dt){
	f->rotation = dt;
	return f;
}

color_t intersect(coord_t p, figure_t* f, double grain){
	p = inv_translate_point(p,f->centre);
	p = inv_rotate_point(p,f->rotation);
	switch(f->type) {
		case 0: if((pow(p.x,2) + pow(p.y,2))<= f->cercle.carreRayon) return f->couleur; break;
		case 1: if((fabs(p.x) <= f->rectangle.demiLargeur)&& (fabs(p.y) <= f->rectangle.demiHauteur)) return f->couleur; break;
		case 2: if((fabs(p.x) <= f->ligne.demiLongueur) && (fabs(p.y) <= grain))  return f->couleur; break;
	}
	return 0;
}

image_t image(unsigned int width, unsigned int height, double grain) {
    image_t i;
	i.width = width;
	i.height = height;
	i.grain = grain;
	i.first = NULL;
	i.nbFigure = 0;
	return i;
 }

image_t* append(image_t* img, figure_t* f){
    img->nbFigure++;
    f->next=img->first; 
	img->first=f;
	return img;
}

void paint(image_t img){
	bool espace; unsigned int a, b;
	for(a = 0; a < img.height; a++){
		for(b = 0; b < img.width; b++){
			espace = true;
			figure_t* temp=img.first;
			while(temp!=NULL) {
				if(intersect(coordinate(b*img.grain*0.5,a*img.grain),temp,img.grain)){
					printf("%c",temp->couleur);
					espace = false;
					break;
				}
                temp=temp->next;
            }
			if(espace) printf(" ");
		}
		printf("\n");
	}
}




extern bool cont;
xelement_t* e=NULL;
char* nom=NULL;




void correct(char *chaine){
    char *p = strchr(chaine, '\n');
    if (p) *p = '\0';
	else while(getchar()!='\n');
}

int seek_format(const char* chaine){
	int i=strlen(chaine);
	if(i<5)return 0;
	else if(chaine[i-4]=='.'&&chaine[i-3]=='s'&&chaine[i-2]=='v'&&chaine[i-1]=='g') return 1;
	else if(chaine[i-4]=='.'&&chaine[i-3]=='p'&&chaine[i-2]=='d'&&chaine[i-1]=='f') return 2;
	return 0;
}

char* question(const char* ask, bool b){
	if(b) printf("\n");
	printf("%s",ask);
	char* choice = (char*)calloc(MAXI,sizeof(char));
	if (choice==NULL) FAIL()
	fgets(choice,MAXI,stdin); correct(choice);
	return choice;
}

int* tabVal (char* answer, int* ptr){
	 int nb=0; bool b=true; int j=0; unsigned int efface, tri=0; 
    //cherchons combien il y a d'éléments et mettons ces derniers dans un tableau trié...
    while(answer[j]!='\0'){
        if(answer[j]!=' '&&b){
            nb++;
			(*ptr)++;
            b=false;
        }
        else if(answer[j]==' ')b=true;
        j++;
    }
	
    int* tab=(int*)calloc(nb,sizeof(int));
	if(tab==NULL) FAIL()
		
    for(j=0;j<nb;j++){
        sscanf(answer,"%d",&tab[j]);
		tri = j; efface = tab[j];
		while(tri!=0){
			if(tab[tri]<tab[tri-1]){
				efface=tab[tri-1];
				tab[tri-1]=tab[tri];
				tab[tri]=efface;
			}
			else break;
			tri--;
		}
		efface=0;
		
        while(answer[efface]==' '){
            efface++;
        }
        while(answer[efface]!=' '&&efface<strlen(answer)){
            answer[efface]=' ';
            efface++;
        }
    }
	return tab;
}

const char* value (xelement_t* x, const char* ch){
	xattribute_t* temp = x->listAttrib;
    while(temp!=NULL){
        if(!strcmp(temp->name,ch)) {
            return temp->value;
		}   
        temp=temp->next;
    }
	return "0";
}

int listNum (xelement_t* x, int i, bool b){
	xelement_t* temp = x->contenu.listSon; 
	while(temp!=NULL){
		if(strcmp(temp->name,"desc")&&strcmp(temp->name,"title")){
			i++;
			if(b) printf("\n %d - %s dans %s",i, temp->name, temp->father->name);
		}
		if((temp->contenu.listSon!=NULL&&temp->contenu.listSon->father==temp)) i = listNum(temp,i,b);
		temp=temp->brother;
	}
	return i;
}

//prev->brother est le précédent frère de temp, si prev->brother==NULL alors temp est le 1er fils, j'ai ensuite prev->father qui est le père de temp...
//cette fonction renvoie la j-ème figure dans le svg en parcours récursif...
xelement_t* which_one (xelement_t* x, int* i, int j, xelement_t* prev){
	xelement_t* temp=x->contenu.listSon;
	if(prev!=NULL){prev->father=x; prev->brother=NULL;}
	while((*i)<j&&temp!=NULL){
		if(strcmp(temp->name,"desc")&&strcmp(temp->name,"title"))(*i)++;
		if(((*i)<j&&temp->contenu.listSon!=NULL&&temp->contenu.listSon->father==temp)) temp = which_one(temp,i,j,prev);
		if((*i)<j&&temp!=NULL) {
			if(prev!=NULL) prev->brother=temp;
			temp=temp->brother;
		}
	}
	if((*i)<j) return x;
	else return temp;
}

double copy(const char* transform, int debut, int fin){ //récupère les flottants dans translate, rotate et scale...
	char* answer=(char*)calloc(fin-debut+2,sizeof(char));
	if(answer==NULL) FAIL()
	int i;
	for(i=0;i<(fin-debut+1);i++){
		answer[i]=transform[debut+i];
	}
	double f = atof(answer);
	free(answer);
	return f;
}

double data(const char* transform, char code){ //situe les positions de début et fin des flottants dans translate, rotate et scale...
	int debut=0; int fin=0; int i=0;
	if(code=='a'){
		while(transform[i]!='\0'){
			if(i>8&&transform[i-9]=='t'&&transform[i-8]=='r'){
				fin=debut;
			}
			if(fin==0) debut++;
			else fin++;
			if(transform[fin]==' '){
				return copy(transform, debut+1, fin-1);
			}
			i++;
		}
	}
	else if(code=='b'){
		while(transform[i]!='\0'){
			if(i>8&&transform[i]==' '){
				fin=debut;
			}
			if(fin==0) debut++;
			else fin++;
			if(transform[fin]==')'){
				return copy(transform, debut+1, fin-1);
			}
			i++;
		}
	}
	else if(code=='c'){
		while(transform[i]!='\0'){
			if(i>5&&transform[i-6]=='r'&&transform[i-5]=='o'){
				fin=debut;
			}
			if(fin==0) debut++;
			else fin++;
			if(transform[fin]==')'){
				return copy(transform, debut+1, fin-1);
			}
			i++;
		}
	}
	else if(code=='d'){
		while(transform[i]!='\0'){
			if(i>4&&transform[i-5]=='s'&&transform[i-4]=='c'){
				fin=debut;
			}
			if(fin==0) debut++;
			else fin++;
			if(transform[fin]==')'){
				return copy(transform, debut+1, fin-1);
			}
			i++;
		}
	}
	return 0;
}

char* true_color(const char* fill){
	if(!strcmp(fill,"crimson")) return "0.86 0.08 0.23";
	else if(!strcmp(fill,"red")) return "1 0 0";
	else if (!strcmp(fill,"green")) return "0 0.5 0"; 
	else if (!strcmp(fill,"lime")) return "0 1 0"; 
	else if (!strcmp(fill,"blue")) return "0 0 1";
	else if (!strcmp(fill,"maroon")) return "0.5 0 0";
	else if (!strcmp(fill,"yellow")) return "1 1 0";
	else if (!strcmp(fill,"teal")) return "0 0.5 0.5";
	else if (!strcmp(fill,"navy")) return "0 0 0.5";
	else if (!strcmp(fill,"olive")) return "0.5 0.5 0";
	else if (!strcmp(fill,"purple")) return "0.5 0 0.5";
	else if(!strcmp(fill,"orange")) return "1 0.65 0";
	else if (!strcmp(fill,"aqua")||!strcmp(fill,"cyan")) return "0 1 1";
	else if (!strcmp(fill,"magenta")||!strcmp(fill,"fuchsia")) return "1 0 1";
	else if(!strcmp(fill,"gray")||!strcmp(fill,"grey")) return "0.5 0.5 0.5";
	else return "0 0 0";
}

char* my_realloc(char* cnt, const char* src){
	if(cnt!=NULL) cnt = (char*)realloc(cnt,strlen(cnt)+strlen(src)+1);
	else cnt = (char*)calloc(strlen(src)+1,sizeof(char));
	if(cnt==NULL) FAIL()
	strcat(cnt,src);
	return cnt;
}

char* trans_pdf (char* cnt, const char* transform){
	double a=data(transform, 'a');
	double b=data(transform, 'b');
	double c=data(transform, 'c');
	double d=data(transform, 'd');
	char* buff=NULL;
	if(a!=0||b!=0){
		asprintf(&buff,"1 0 0 1 %.3g %.3g cm ",a,b);
		cnt=my_realloc(cnt,buff);
	}
	if(c!=0){
		asprintf(&buff,"%.3g %.3g %.3g %.3g 0 0 cm ", cos(c * M_PI / 180), sin(c * M_PI / 180), sin(-c * M_PI / 180), cos(c * M_PI / 180));
		cnt=my_realloc(cnt,buff);
	}
	if(d!=0){
		asprintf(&buff,"%.3g 0 0 %.3g 0 0 cm ",d,d);
		cnt=my_realloc(cnt,buff);
	}
	if(buff!=NULL) free(buff);
	return cnt;
}

char* police_pdf(const char* font){
	if(!strcmp(font,"Times-Roman")) return "FT";
	else if(!strcmp(font,"Times-Bold")) return "FTB";
	else if(!strcmp(font,"Times-Italic")) return "FTI";
	else if(!strcmp(font,"Times-BoldItalic")) return "FTBI";
	else if(!strcmp(font,"Helvetica-Bold")) return "FVB";
	else if(!strcmp(font,"Helvetica-Oblique")) return "FVI";
	else if(!strcmp(font,"Helvetica-BoldOblique")) return "FVBI";
	else return "FV";
}

char* data_pdf (char* cnt, xelement_t* x, pdf_t* p){
	xelement_t* temp = x->contenu.listSon; char* buff = NULL; const char* im=NULL;
	while(temp!=NULL){
		buff=NULL; //Important
		if(strcmp(temp->name,"desc")&&strcmp(temp->name,"title")){
			cnt=my_realloc(cnt,"q ");
			cnt=trans_pdf(cnt,value(temp,"transform"));
		}
		if(!strcmp(temp->name,"g"))	cnt=data_pdf(cnt,temp,p);
		else if(!strcmp(temp->name,"circle")){
			asprintf(&buff,"%s rg %s 0 0 %s %s %s cm ",true_color(value(temp,"fill")),value(temp,"r"),value(temp,"r"),value(temp,"cx"),value(temp,"cy"));
			cnt=my_realloc(cnt,buff);
			cnt=my_realloc(cnt,"-1 0 m -1 0.553 -0.553 1 0 1 c 0.553 1 1 0.553 1 0 c 1 -0.553 0.553 -1 0 -1 c -0.553 -1 -1 -0.553 -1 0 c F ");
		}
		else if(!strcmp(temp->name,"rect")){		
			asprintf(&buff,"%s rg %s %s %s %s re F ",true_color(value(temp,"fill")),value(temp,"x"), value(temp,"y"), value(temp,"width"), value(temp,"height"));
			cnt=my_realloc(cnt,buff);
		}
		else if(!strcmp(temp->name,"line")){
			asprintf(&buff,"%s w %s RG %s %s m %s %s l S ",value(temp,"stroke-width"),true_color(value(temp,"stroke")),value(temp,"x1"), value(temp,"y1"), value(temp,"x2"), value(temp,"y2"));
			cnt=my_realloc(cnt,buff);
		}
		else if(!strcmp(temp->name,"text")&&temp->contenu.raw!=NULL&&temp->contenu.listSon->father!=temp){
			asprintf(&buff,"%s rg BT /%s %s. Tf 1 0 0 -1 %s %s Tm (%s) Tj ET ",true_color(value(temp,"fill")),police_pdf(value(temp,"font-family")),value(temp,"font-size"),value(temp,"x"),value(temp,"y"),temp->contenu.raw);
			cnt=my_realloc(cnt,buff);
		}
		else if(!strcmp(temp->name,"image")){
			asprintf(&buff,"%s 0 0 -%s %s %g cm ",value(temp,"width"),value(temp,"height"),value(temp,"x"),atof(value(temp,"height"))+atof(value(temp,"y")));
			cnt=my_realloc(cnt,buff);
			free(buff);
			if(strcmp(value(temp,"xlink:href"),"0")) im = pdf_load_image(p, value(temp,"xlink:href"));
			else im = pdf_load_image(p, value(temp,"href"));
			asprintf(&buff,"/%s Do ",im);
			cnt=my_realloc(cnt,buff);
		}
		if(buff!=NULL) free(buff);  //Important
		if(strcmp(temp->name,"desc")&&strcmp(temp->name,"title"))cnt=my_realloc(cnt,"Q ");
		temp = temp->brother;
	}
	return cnt;
}

float dimension(const char* val){
	int i = strlen(val);
	if(i>2&&val[i-2]=='c'&&val[i-1]=='m') return atof(val)*100;
	return atof(val);
}

void enregistrerPDF(const char* fnameSa){
	pdf_t* p = pdf_create(1, dimension(value(e,"width")), dimension(value(e,"height")));
    
	char* cnt = NULL;
	asprintf(&cnt,"q 1 0 0 -1 0 %g cm ",dimension(value(e,"height")));
	cnt = data_pdf (cnt, e, p);
	cnt = my_realloc(cnt, "Q ");
	pdf_set_content(p, 0, cnt); free(cnt);
	pdf_save(fnameSa,p);
	pdf_delete(p);	
}

void enregistrerSVG(const char* fnameSa){
	save_xml(fnameSa,e);
}

void enregistrer(){
	CHECK()
	
	printf("\n(Mettez aussi l'extension s'il vous plaît, sinon, par défaut, ce sera sous le nom du fichier d'origine.)");
	char* fnameSa = question("Sous quel nom voulez-vous enregistrer l'image courante ? : ", true);
	int a = seek_format(fnameSa);
	if(a==2) enregistrerPDF(fnameSa);
	else if(a==1) enregistrerSVG(fnameSa);
	else enregistrerSVG(nom);
	free(fnameSa);
}

void sauvegarder(){
	if(e!=NULL){
		printf("\nVoulez-vous enregistrer l'image courante avant tout ?\n 1 - Oui\n 2 - Non\n\nChoice? : ");
		char choice = getchar(); while(choice!='\n'&&getchar()!='\n');
		if( choice=='1') enregistrer();
		
	
		delete_xelement(e);
		free(nom);
	}
}

void ouvrir(){
	sauvegarder();
	
	char* fnameOp = question("Quel fichier voulez-vous ouvrir ? : ",true);
	e = load_xml(fnameOp);
	
	if(strcmp(e->name,"svg")||(e->contenu.listSon!=NULL&&e->contenu.listSon->father!=e)){
		fprintf(stderr,"Erreur, votre fichier (%s) n'est pas en svg.\n", fnameOp);
		delete_xelement(e);
		e = NULL;
		return;
	}
	
	nom = fnameOp;
}

xelement_t* title_desc (const char* desc_title){
	xelement_t* temp = e->contenu.listSon;
	while(temp!=NULL){
		if(!strcmp(temp->name,desc_title)&&temp->contenu.raw!=NULL&&temp->contenu.listSon!=temp) return temp;
		temp=temp->brother;
	}
	return NULL;
}

void information(){
	CHECK()
	
	printf("\nLe nom du fichier en cours est : %s\n",nom);
	if(title_desc("title")!=NULL){
		printf("Titre de l'image : %s\n",title_desc("title")->contenu.raw);
	}
	if(title_desc("desc")!=NULL){
		printf("Description de l'image : %s\n",title_desc("desc")->contenu.raw);
	}
	printf("L'image chargée est de taille : %s x %s\n",value(e,"width"),value(e,"height"));
	printf("\nIl y a %d figure(s).\n",listNum(e,0,true));
}

figure_t* tr_ro_sc (figure_t* f, const char* transform, image_t* img){
	double a=data(transform, 'a');
	double b=data(transform, 'b');
	double c=data(transform, 'c');
	double d=data(transform, 'd');
	if(a!=0||b!=0){
		f = translate(f, a/10, b/10);
	}
	if(c!=0){
		f = rotate(f, c * M_PI / 180);
	}
	if(0){
		img->grain/=d/2;
	}
	return f;
}

void calcul(xelement_t* x, const char* transform, image_t* img){
	
	xelement_t* temp=x->contenu.listSon;
	figure_t* f = NULL;
	
	while(temp!=NULL){
		if(!strcmp(temp->name,"line")){
			f = line((atof(value(temp,"x2")) - atof(value(temp,"x1"))) / 10);
			f = tr_ro_sc(f, value(temp,"transform"), img);
			f = color(f, '*');
			if(transform!=NULL&&strcmp(transform,"0")){
				f = tr_ro_sc(f, transform, img);
			}
			img = append(img,f);
		}
		else if(!strcmp(temp->name,"rect")){
			f = rectangle(atof(value(temp,"width"))/10,atof(value(temp,"height"))/10);
			f = tr_ro_sc(f, value(temp,"transform"), img);
			if(transform!=NULL&&strcmp(transform,"0")){
				f = tr_ro_sc(f, transform, img);
			}
			img = append(img,f);
		}
		else if(!strcmp(temp->name,"circle")){
			f = circle(atof(value(temp,"r"))/10);
			f = tr_ro_sc(f, value(temp,"transform"), img);
			f = color(f, 'O');
			if(transform!=NULL&&strcmp(transform,"0")){
				f = tr_ro_sc(f, transform, img);
			}
			img = append(img,f);
		}
		else if(!strcmp(temp->name,"g")&&temp->contenu.listSon!=NULL&&temp->contenu.listSon->father==temp){
			if(transform==NULL){
				calcul(temp,value(temp,"transform"),img);
			}
			else {
				char* buff=NULL;
				asprintf(&buff,"%s,%s",transform,value(temp,"transform"));
				calcul(temp,buff,img);
				free(buff);
			}
		}
		temp=temp->brother;
	}
}

void afficher(){
	CHECK()
	
	image_t img = image( ((int)dimension(value(e,"width")))/10, ((int)dimension(value(e,"height")))/10, 0.5);

	calcul(e,NULL,&img);

	paint(img);
	freeOfList(img.first);
}

void quit() {
	sauvegarder();
	cont = false;
}









void add_c() {
	CHECK()

	xelement_t* se = create_xelement("circle");
	add_xattribute(se, "cx", "0");
	add_xattribute(se, "cy", "0");
	
	char* answer = question("Ajouter quel rayon ? : ", false); 
	add_xattribute(se, "r", answer); free(answer);
	answer = question("Ajouter quelle couleur (En anglais...) ? : ", false); 
	add_xattribute(se, "fill", answer); free(answer);
	add_sub_xelement(e,se);
}

void add_r() {
	CHECK()
	
	xelement_t* se = create_xelement("rect");
	add_xattribute(se, "x", "0");
	add_xattribute(se, "y", "0");
	
	char* answer = question("Ajouter quelle largeur ? : ", false); 
	add_xattribute(se, "width", answer); free(answer);
	answer = question("Ajouter quelle hauteur ? : ", false); 
	add_xattribute(se, "height", answer); free(answer);
	answer = question("Ajouter quelle couleur (En anglais...) ? : ", false); 
	add_xattribute(se, "fill", answer); free(answer);
	add_sub_xelement(e,se);
}

void add_l() {
	CHECK()
	
	xelement_t* se = create_xelement("line");
	char* answer = question("Ajouter quelle longueur ? : ", false); 
	float x2 = atof(answer)/2; float x1 = -x2;
	
	sprintf(answer,"%g",x1); add_xattribute(se, "x1", answer); 
	add_xattribute(se, "y1", "0");
	sprintf(answer,"%g",x2); add_xattribute(se, "x2", answer); 
	add_xattribute(se, "y2", "0");
	add_xattribute(se, "stroke-width", "15");
	
	free(answer);
	answer = question("Ajouter quelle couleur (En anglais...) ? : ", false); 
	add_xattribute(se, "stroke", answer); free(answer);
	add_sub_xelement(e,se);
}

void add_t() {
	CHECK()
	
	xelement_t* se = create_xelement("text");
	add_xattribute(se, "x", "0");
	add_xattribute(se, "y", "0");
	add_xattribute(se, "font-family", "Times-Roman");
	add_xattribute(se, "font-size", "25");
	
	char* answer = question("Ajouter quel texte ? : ", false);
	add_raw(se, answer); free(answer);
	answer = question("Ajouter quelle couleur (En anglais...) ? : ", false); 
	add_xattribute(se, "fill", answer); free(answer);
	add_sub_xelement(e,se);
}

void add_i() {
	CHECK()
	
	xelement_t* se = create_xelement("image");
		
	char* answer = question("Ajouter quelle image ? : ", false);
	add_xattribute(se, "href", answer); free(answer);
	
	add_xattribute(se, "x", "0");
	add_xattribute(se, "y", "0");
	
	answer = question("Ajouter quelle largeur ? : ", false); 
	add_xattribute(se, "width", answer); free(answer);
	answer = question("Ajouter quelle hauteur ? : ", false); 
	add_xattribute(se, "height", answer); free(answer);
	
	add_xattribute(se, "preserveAspectRatio", "none");
	
	add_sub_xelement(e,se);
}

void supprimons() {
	CHECK()
	if(e->contenu.raw==NULL||e->contenu.listSon->father!=e){
		fprintf(stderr,"Il n'y a aucune figure.\n");
		return;
	}
	
	listNum(e,0,true);
	
	char* answer = question("Supprimer quelle figure ? : ", true);
	
	fctD(atoi(answer)); 
	
	free(answer);
}

void translatons(){
	CHECK()
	
	int i = listNum(e,0,true);
	if(i==0){
		fprintf(stderr,"Il n'y a aucune figure.\n");
		return;
	}
	
	
	char* answer1 = question("Translater quelle figure ? : ", true); 
	int j = atoi(answer1); free(answer1);
	if(i<j||j<1){
		fprintf(stderr,"Cette figure [%d] n'existe pas, impossible de translater.\n", j);
		return;
	}
	
	answer1 = question("Quel premier flottant ? : ", true);
	char* answer2 = question("Quel second flottant ? : ", false);
	
	fctT(j,atof(answer1),atof(answer2));
	
	free(answer1); free(answer2);
}

void tournons(){
	CHECK()
	
	int i = listNum(e,0,true);
	if(i==0){
		fprintf(stderr,"Il n'y a aucune figure.\n");
		return;
	}
	
	
	char* answer = question("Tourner quelle figure ? : ", true); 
	int j = atoi(answer); free(answer);
	if(i<j||j<1){
		fprintf(stderr,"Cette figure [%d] n'existe pas, impossible de tourner.\n", j);
		return;
	}
	
	answer = question("Quel flottant ? : ", true);
	
	fctA(j,atof(answer));
	
	free(answer);
}

void etirons(){
	CHECK()
	
	int i = listNum(e,0,true);
	if(i==0){
		fprintf(stderr,"Il n'y a aucune figure.\n");
		return;
	}
	
	
	char* answer = question("Etirer quelle figure ? : ", true);
	int j = atoi(answer); free(answer);
	if(i<j||j<1){
		fprintf(stderr,"Cette figure [%d] n'existe pas, impossible d'étirer.\n", j);
		return;
	}
	
	answer = question("Quel flottant ? : ", true); 
	
	fctS(j,atof(answer));
	
	free(answer);
}

void groupons(){
	CHECK()
	if(e->contenu.raw==NULL||e->contenu.listSon->father!=e){
		fprintf(stderr,"Il n'y a aucune figure.\n");
		return;
	}
	
	listNum(e,0,true); 
	
	char* answer = question("Grouper quelles figures ? : ", true); 
	
	fctG(answer);
	
	free(answer); 
}

int listNumd (xelement_t* x, int i, bool b){
	xelement_t* temp = x->contenu.listSon; 
	while(temp!=NULL){
		if(temp->contenu.listSon!=NULL&&temp->contenu.listSon->father==temp) {
			i++;
			if(b) printf("\n %d - %s dans %s",i, temp->name, temp->father->name);
			i = listNumd(temp,i,b);
		}
		temp=temp->brother;
	}
	return i;
}

xelement_t* which_oned (xelement_t* x, int* i, int j, xelement_t* prev){
	xelement_t* temp=x->contenu.listSon;
	if(prev!=NULL){prev->father=x; prev->brother=NULL;}
	while((*i)<j&&temp!=NULL){
		if(temp->contenu.listSon!=NULL&&temp->contenu.listSon->father==temp) {
			(*i)++;
			if((*i)<j) temp = which_oned(temp,i,j,prev);
		}
		if((*i)<j&&temp!=NULL) {
			if(prev!=NULL) prev->brother=temp;
			temp=temp->brother;
		}
	}
	if((*i)<j) return x;
	else return temp;
}

void degroupons(){
	CHECK()

	int i = listNumd(e,0,true);
	if(i==0){
		fprintf(stderr,"Il n'y a aucun groupe.\n");
		return;
	}
	
	char* answer = question("Dissoudre quel groupe ? : ", true); 
	
	fctU(atoi(answer));
	
	free(answer);
}









void good_bye(){
	if(e!=NULL) delete_xelement(e);
}

void fctI(const char* str){
	good_bye();
	e = load_xml(str);
	
	if(strcmp(e->name,"svg")||(e->contenu.listSon!=NULL&&e->contenu.listSon->father!=e)){
		fprintf(stderr,"Erreur, votre fichier (%s) n'est pas en svg.\n", str);
		delete_xelement(e);
		e = NULL;
	}
}

void fctO(const char* str){
	CHECK()
	int a = seek_format(str);
	if(a==2) enregistrerPDF(str);
	else if(a==1) enregistrerSVG(str);
	else fprintf(stderr,"Erreur, ce format (%s) n'est pas proposé.\n", str);
}

void fctC(float f){
	CHECK()
	
	xelement_t* se = create_xelement("circle");
	add_xattribute(se, "cx", "0");
	add_xattribute(se, "cy", "0");
	
	char answer[MAXI]=""; sprintf(answer,"%g",f);
	add_xattribute(se, "r", answer);
	add_xattribute(se, "fill", "crimson"); 
	add_sub_xelement(e,se);
}

void fctR(float flo1, float flo2){
	CHECK()
	
	xelement_t* se = create_xelement("rect");
	add_xattribute(se, "x", "0");
	add_xattribute(se, "y", "0");
	
	char answer[MAXI]=""; sprintf(answer,"%g",flo1);
	add_xattribute(se, "width", answer);
	sprintf(answer,"%g",flo2);
	add_xattribute(se, "height", answer); 
	add_xattribute(se, "fill", "crimson");
	add_sub_xelement(e,se);
}

void fctL(float f){
	CHECK()
	
	xelement_t* se = create_xelement("line");
	add_xattribute(se, "stroke-width", "15"); 
	add_xattribute(se, "stroke", "crimson");
	
	char answer[MAXI]=""; 
	sprintf(answer,"%g",-f/2); add_xattribute(se, "x1", answer);
	add_xattribute(se, "y1", "0");
	sprintf(answer,"%g",f/2); add_xattribute(se, "x2", answer);
	add_xattribute(se, "y2", "0");	 
	add_sub_xelement(e,se);
}

void fctX(const char* str){
	CHECK()
	
	xelement_t* se = create_xelement("text");
	add_xattribute(se, "x", "0");
	add_xattribute(se, "y", "0");
	add_xattribute(se, "font-family", "Times-Roman");
	add_xattribute(se, "font-size", "25");
	
	add_raw(se, str); 
	add_xattribute(se, "fill", "crimson");
	add_sub_xelement(e,se);
}

void fctJ(const char* str){
	CHECK()
	
	xelement_t* se = create_xelement("image");
	
	add_xattribute(se, "href", str);
	
	add_xattribute(se, "x", "0");
	add_xattribute(se, "y", "0");
	
	add_xattribute(se, "width", "100");
	add_xattribute(se, "height", "100");
	
	add_xattribute(se, "preserveAspectRatio", "none");
	
	add_sub_xelement(e,se);
}

void fctD(int j) {
	CHECK()
	
	int i = listNum(e,0,false);
	
	if(i<j||j<1){
		fprintf(stderr,"Cette figure [%d] n'existe pas, impossible de supprimer.\n", j);
		return;
	}
	
	xelement_t* prev=create_xelement("prev"); 
	
	i=0;
	xelement_t* temp=which_one(e,&i,j,prev);
	
		
	if(prev->brother==NULL) prev->father->contenu.listSon=temp->brother;
	else prev->brother->brother=temp->brother;
	
	temp->father=NULL; temp->brother=NULL;
	delete_xelement(temp);
	
	prev->father=NULL; prev->brother=NULL;
	delete_xelement(prev);
}

void fctS(int j, float f){
	CHECK()
	
	int i = listNum(e,0,false);
	char* buff=NULL;
	
	if(i<j||j<1){
		fprintf(stderr,"Cette figure [%d] n'existe pas, impossible d'étirer.\n", j);
		return;
	}
	i=0;
	xelement_t* temp = which_one(e,&i,j,NULL);
	
	if(!strcmp(value(temp,"transform"),"0")){
		asprintf(&buff,"scale(%g)",f);
	}
	else{
		asprintf(&buff,"%scale(%g),%s",f,value(temp,"transform"));
	}
	add_xattribute(temp,"transform",buff);
	free(buff); 
}

void fctA(int j, float f){
	CHECK()
	
	int i = listNum(e,0,false);
	char* buff=NULL;
	
	if(i<j||j<1){
		fprintf(stderr,"Cette figure [%d] n'existe pas, impossible de tourner.\n", j);
		return;
	}
	i=0;
	xelement_t* temp = which_one(e,&i,j,NULL);
	
	if(!strcmp(value(temp,"transform"),"0")){
		asprintf(&buff,"rotate(%g)",f);
	}
	else{
		asprintf(&buff,"rotate(%g),%s",f,value(temp,"transform"));
	}
	add_xattribute(temp,"transform",buff);
	free(buff);
}

void fctT(int j, float flo1, float flo2){
	CHECK()
	
	int i = listNum(e,0,false);
	char* buff=NULL;
	
	if(i<j||j<1){
		fprintf(stderr,"Cette figure [%d] n'existe pas, impossible de translater.\n", j);
		return;
	}
	i=0;
	xelement_t* temp = which_one(e,&i,j,NULL);
	
	if(!strcmp(value(temp,"transform"),"0")){
		asprintf(&buff,"translate(%g %g)",flo1,flo2);
	}
	else{
		asprintf(&buff,"translate(%g %g),%s",flo1,flo2,value(temp,"transform"));
	}
	add_xattribute(temp,"transform",buff);
	free(buff);
}

void fctG(const char* listInt){
	CHECK()
	
	char* answer = my_realloc(NULL,listInt);

	int i = listNum(e,0,false); 
	int a, b, j, nb = 0; 
	int* tab = tabVal(answer, &nb);
	
	for(j=0;j<nb;j++){//Vérification au cas où l'utilisateur met n'importe quoi...
		a=0; b=0;
		if(i<tab[j]||tab[j]<1){
			fprintf(stderr,"Cette figure [%d] n'existe pas, impossible de grouper.\n",tab[j]);
			free(answer); free(tab);
			return;
		}
		if(j>0&&tab[j-1]==tab[j]){
			fprintf(stderr,"Avec des valeurs doublons, impossible de grouper.\n");
			free(answer); free(tab);
			return;
		}
		if(j>0&&which_one(e,&a,tab[j-1],NULL)->father!=which_one(e,&b,tab[j],NULL)->father){
			fprintf(stderr,"Les éléments [%d] et [%d] n'ont pas le même parent, impossible de grouper.\n",tab[j-1], tab[j]);
			free(answer); free(tab);
			return;
		}
	}
	
	xelement_t* g = create_xelement("g"); xelement_t* prev = create_xelement("prev"); xelement_t* temp = NULL;
	for(j=0;j<nb;j++){
		i=0;
		temp=which_one(e,&i,tab[j]-j,prev);
		if(prev->brother==NULL){
			prev->father->contenu.listSon = temp->brother;
		}
		else{
			prev->brother->brother = temp->brother;
		}
		temp->father = NULL; temp->brother = NULL;
		add_sub_xelement(g,temp);
		prev->father=NULL; prev->brother=NULL;
	}
	add_sub_xelement(e,g);

	
	delete_xelement(prev);
    free(tab);  free(answer);
}

void fctU(int j){
	CHECK()

	int i = listNumd(e,0,false);
	if(i==0){
		fprintf(stderr,"Il n'y a aucun groupe.\n");
		return;
	}
	
	if(i<j||j<1){
		fprintf(stderr,"Ce groupe [%d] n'existe pas.\n", j);
		return;
	}
	
	i=0; xelement_t* prev = create_xelement("prev");
	xelement_t* temp = which_oned(e,&i,j,prev); 
	

	//alpha et beta sont des sauvegardes...
	xelement_t* alpha = temp->contenu.listSon; xelement_t* beta = temp->brother;
	temp->contenu.listSon = NULL; temp->father = NULL; temp->brother = NULL; 
	delete_xelement(temp);

	if(prev->brother==NULL)	prev->father->contenu.listSon = beta;
	else prev->brother->brother = beta;
	
	temp = alpha;
	while(temp!=NULL){
		temp->father=NULL; 
		alpha = temp;
		temp = temp->brother;
		alpha->brother = NULL;
		add_sub_xelement(prev->father,alpha);
	}
	
	
	prev->father=NULL; prev->brother=NULL;
	delete_xelement(prev);
}