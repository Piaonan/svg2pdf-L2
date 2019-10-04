#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "pdf.h"
#define FAIL() {fprintf(stderr,"memory allocation failure in pdf\n"); exit(EXIT_FAILURE);}
#include <stdarg.h>


void pdf_object_free(pdf_object_t* o){
    if(o==NULL) {fprintf(stderr,"Erreur, o est nul pour pdf_object_free.\n"); exit(0);}
    unsigned int i=0; dictionary_t* temp=NULL; dictionary_t* prev=NULL;
    switch(o->genre){
    case 5: free(o->nom); break;
    case 6: free(o->chaine); break;
    case 7: for(i=0; i<(o->nbTable);i++){
                pdf_object_free(o->table[i]);
                free(o->table[i]);
            }
            free(o->table);
            break;
    case 8: free(o->stream.data);
            pdf_object_free(o->stream.sDico);
            break;
    case 9: temp = o->dico;
            while(temp!=NULL){
				prev=temp; temp=temp->next;
                free(prev->key);
                pdf_object_free(prev->value);
                free(prev);
            } break;
	default:  break;
    }
}

pdf_object_t* pdf_null(pdf_object_t* o){
    if(o!=NULL) pdf_object_free(o);
    else if( (o=(pdf_object_t*)malloc(sizeof(pdf_object_t)))==NULL ) FAIL()
    o->genre=Null;
    return o;
}

pdf_object_t* pdf_bool(pdf_object_t* o, bool b){
    if(o!=NULL) pdf_object_free(o);
    else if( (o=(pdf_object_t*)malloc(sizeof(pdf_object_t)))==NULL ) FAIL()
    o->genre=Boolean;
    o->b=b;
    return o;
}

pdf_object_t* pdf_int(pdf_object_t* o, int i){
    if(o!=NULL) pdf_object_free(o);
    else if( (o=(pdf_object_t*)malloc(sizeof(pdf_object_t)))==NULL ) FAIL()
    o->genre=Entier;
    o->i=i;
    return o;
}

pdf_object_t* pdf_real(pdf_object_t* o, float f){
    if(o!=NULL) pdf_object_free(o);
    else if( (o=(pdf_object_t*)malloc(sizeof(pdf_object_t)))==NULL ) FAIL()
    o->genre=Reel;
    o->f=f;
    return o;
}

pdf_object_t* pdf_reference(pdf_object_t* o, unsigned int id){
    if(o!=NULL) pdf_object_free(o);
    else if( (o=(pdf_object_t*)malloc(sizeof(pdf_object_t)))==NULL ) FAIL()
    o->genre=Reference;
    o->idRef=id;
    return o;
}

pdf_object_t* pdf_name(pdf_object_t* o, const char* n){
    if(o!=NULL) pdf_object_free(o);
    else if( (o=(pdf_object_t*)malloc(sizeof(pdf_object_t)))==NULL ) FAIL()
    o->genre=Nom;
    if( (o->nom=(char*)malloc(sizeof(char)*strlen(n)+1))==NULL ) FAIL()
    strcpy(o->nom,n);
    return o;
}

pdf_object_t* pdf_string(pdf_object_t* o, const char* s){
    if(o!=NULL) pdf_object_free(o);
    else if( (o=(pdf_object_t*)malloc(sizeof(pdf_object_t)))==NULL ) FAIL()
    o->genre=Chaine;
    if( (o->chaine=(char*)malloc(sizeof(char)*strlen(s)+1))==NULL ) FAIL()
    strcpy(o->chaine,s);
    return o;
}

pdf_object_t* pdf_array(pdf_object_t* o, size_t n){
    if(o!=NULL) pdf_object_free(o);
    else if( (o=(pdf_object_t*)malloc(sizeof(pdf_object_t)))==NULL ) FAIL()
    o->genre=Tableau;
    if( (o->table=(pdf_object_t**)malloc(n*sizeof(pdf_object_t*)))==NULL ) FAIL()
    unsigned int i=0; o->nbTable=(unsigned int)n;
    for(i=0; i<n; i++) {
        o->table[i]=NULL;
        o->table[i]=pdf_null(o->table[i]);
    }
    return o;
}

pdf_object_t* pdf_dictionary(pdf_object_t* o){
    if(o!=NULL) pdf_object_free(o);
    else if( (o=(pdf_object_t*)malloc(sizeof(pdf_object_t)))==NULL ) FAIL()
    o->genre=Dictionnaire;
    o->dico=NULL;
    return o;
}

pdf_object_t* pdf_stream(pdf_object_t* o, size_t sz, void* data){
    if(o!=NULL) pdf_object_free(o);
    else if( (o=(pdf_object_t*)malloc(sizeof(pdf_object_t)))==NULL ) FAIL()
    o->genre=Stream; o->stream.data=data;
    o->stream.sDico=NULL; o->stream.sDico=pdf_dictionary(o->stream.sDico);
	pdf_int(pdf_dict_get(o->stream.sDico,"Length"),(int)sz);
    return o;
}

pdf_object_t* pdf_stream_from_file(pdf_object_t* o, const char* fname){
    FILE* fd = NULL; void* data=NULL;
    if( (fd = fopen(fname,"rb"))==NULL ){
        fprintf(stderr,"Erreur, fichier (%s) inexistant.\n",fname);
        exit(EXIT_FAILURE);
    }
    fseek(fd,0,SEEK_END);
    long int sz=ftell(fd);
    fseek(fd,0,SEEK_SET);
    if( (data=(void*)calloc(sz,sizeof(void)))==NULL ) FAIL()
    fread(data,sizeof(void),sz,fd);
    fclose(fd);
    return pdf_stream(o,sz,data);
}

pdf_object_t* pdf_stream_from_string(pdf_object_t* o, const char* str){
    char* data=NULL;
    if( (data=(char*)malloc(strlen(str)*sizeof(char)+1))==NULL ) FAIL();
    strcpy(data,str);
    return pdf_stream(o,strlen(str),data);
 }

pdf_object_t* pdf_array_get(pdf_object_t* a, size_t i){
    if(a->genre!=Tableau){
        fprintf(stderr,"Erreur, vous accÈdez ‡ un objet qui n'est pas un tableau.\n");
        exit(EXIT_FAILURE);
    }
    if(i>=(a->nbTable)){
        fprintf(stderr,"Erreur, le tableau a n'a pas cette taille-l‡.\n");
        exit(EXIT_FAILURE);
    }
    return a->table[i];
}

pdf_object_t* pdf_dict_get(pdf_object_t* d, const char* cle){
    if(d->genre!=Dictionnaire){
        fprintf(stderr,"Erreur, vous accÈdez ‡ un objet qui n'est pas un dictionnaire.\n");
        exit(EXIT_FAILURE);
    }
    dictionary_t* temp = d->dico; dictionary_t* prev = NULL;
    while(temp!=NULL){
        if(!strcmp(temp->key,cle))return temp->value;
        prev=temp;
        temp=temp->next;
    }
    if(prev==NULL){
        if( (d->dico=(dictionary_t*)malloc(sizeof(dictionary_t)))==NULL ) FAIL()
        d->dico->next=NULL; d->dico->value=NULL; d->dico->value=pdf_null(d->dico->value);
        if( (d->dico->key=(char*)malloc(strlen(cle)+1))==NULL ) FAIL()
        strcpy(d->dico->key,cle);
        return d->dico->value;
    }
    if( (prev->next=(dictionary_t*)malloc(sizeof(dictionary_t)))==NULL ) FAIL()
    prev->next->next=NULL; prev->next->value=NULL; prev->next->value=pdf_null(prev->next->value);
    if( (prev->next->key=(char*)malloc(strlen(cle)+1))==NULL ) FAIL()
    strcpy(prev->next->key,cle);
    return prev->next->value;
 }

void pdf_fprint_object(FILE* fd, pdf_object_t* o){
    unsigned int i=0; dictionary_t* temp=NULL;
    if(o==NULL) {
        fprintf(stderr,"Erreur, o est nul dans pdf_fprint_object.\n");
        exit(EXIT_FAILURE);
    }
    switch(o->genre){
    case 0: fprintf(fd,"null"); break;
    case 1: if(o->b) fprintf(fd,"true"); else fprintf(fd,"false"); break;
    case 2: fprintf(fd,"%d",o->i); break;
    case 3: fprintf(fd,"%.04f",o->f); break;
    case 4: fprintf(fd,"%u 0 R",o->idRef+1); break;
    case 5: fprintf(fd,"/%s",o->nom); break;
    case 6: fprintf(fd,"(%s)",o->chaine); break;
    case 7: fprintf(fd,"[");
            for(i=0; i<o->nbTable; i++){
                fprintf(fd," ");
                pdf_fprint_object(fd,o->table[i]);
            }
            fprintf(fd," ]"); break;
    case 8: pdf_fprint_object(fd,o->stream.sDico);
      //      fprintf(fd,"\n>>\nstream\n%sendstream",((char*)(o->stream.data))); break;
            fprintf(fd,"\nstream\n");
            fwrite(o->stream.data,sizeof(void),o->stream.sDico->dico->value->i,fd);
            fprintf(fd,"endstream"); break;
    case 9: fprintf(fd,"<<"); temp=o->dico;
            while(temp!=NULL){
                fprintf(fd,"\n  /%s ",temp->key);
                if(temp->value!=NULL) pdf_fprint_object(fd,temp->value);
                temp=temp->next;
            }
            fprintf(fd,"\n>>");
    }
}

pdf_object_t* pdf_default_fonts(pdf_object_t* f) {
  f = pdf_dictionary(f);
  pdf_object_t* font;
 
  font = pdf_dictionary(pdf_dict_get(f, "FT"));
  pdf_name(pdf_dict_get(font, "Type"), "Font");
  pdf_name(pdf_dict_get(font, "Subtype"), "Type1");
  pdf_name(pdf_dict_get(font, "BaseFont"), "Times-Roman");
 
  font = pdf_dictionary(pdf_dict_get(f, "FTB"));
  pdf_name(pdf_dict_get(font, "Type"), "Font");
  pdf_name(pdf_dict_get(font, "Subtype"), "Type1");
  pdf_name(pdf_dict_get(font, "BaseFont"), "Times-Bold");
 
  font = pdf_dictionary(pdf_dict_get(f, "FTI"));
  pdf_name(pdf_dict_get(font, "Type"), "Font");
  pdf_name(pdf_dict_get(font, "Subtype"), "Type1");
  pdf_name(pdf_dict_get(font, "BaseFont"), "Times-Italic");
 
  font = pdf_dictionary(pdf_dict_get(f, "FTBI"));
  pdf_name(pdf_dict_get(font, "Type"), "Font");
  pdf_name(pdf_dict_get(font, "Subtype"), "Type1");
  pdf_name(pdf_dict_get(font, "BaseFont"), "Times-BoldItalic");
 
  font = pdf_dictionary(pdf_dict_get(f, "FV"));
  pdf_name(pdf_dict_get(font, "Type"), "Font");
  pdf_name(pdf_dict_get(font, "Subtype"), "Type1");
  pdf_name(pdf_dict_get(font, "BaseFont"), "Helvetica");
 
  font = pdf_dictionary(pdf_dict_get(f, "FVB"));
  pdf_name(pdf_dict_get(font, "Type"), "Font");
  pdf_name(pdf_dict_get(font, "Subtype"), "Type1");
  pdf_name(pdf_dict_get(font, "BaseFont"), "Helvetica-Bold");
 
  font = pdf_dictionary(pdf_dict_get(f, "FVI"));
  pdf_name(pdf_dict_get(font, "Type"), "Font");
  pdf_name(pdf_dict_get(font, "Subtype"), "Type1");
  pdf_name(pdf_dict_get(font, "BaseFont"), "Helvetica-Oblique");
 
  font = pdf_dictionary(pdf_dict_get(f, "FVBI"));
  pdf_name(pdf_dict_get(font, "Type"), "Font");
  pdf_name(pdf_dict_get(font, "Subtype"), "Type1");
  pdf_name(pdf_dict_get(font, "BaseFont"), "Helvetica-BoldOblique");
 
  return f;
}

pdf_t* pdf_create(unsigned int nb, float w, float h){
    pdf_t* p=NULL; unsigned int i=0;
    if( (p=(pdf_t*)malloc(sizeof(pdf_t)))==NULL ) FAIL()
    p->nbTable=(4+((unsigned int)nb)); p->table=NULL; p->nbImg=0;
    if( (p->table=(pdf_object_t**)malloc((p->nbTable)*sizeof(pdf_object_t*)))==NULL ) FAIL()

    for(i=0; i<(p->nbTable); i++) {
            p->table[i]=NULL;
            p->table[i]=pdf_dictionary(p->table[i]);
    }

    pdf_name(pdf_dict_get(p->table[0], "Type"), "Catalog");
    pdf_reference(pdf_dict_get(p->table[0], "Pages"), 2);

    p->table[1]=pdf_default_fonts(p->table[1]);

    pdf_name(pdf_dict_get(p->table[2], "Type"), "Pages");
    pdf_int(pdf_dict_get(p->table[2], "Count"), nb);
    pdf_array(pdf_dict_get(p->table[2], "Kids"), nb);


    for(i=4; i<(p->nbTable); i++) {
        pdf_reference(pdf_array_get(pdf_dict_get(p->table[2], "Kids"),i-4),i);
        pdf_name(pdf_dict_get(p->table[i], "Type"), "Page");
        pdf_reference(pdf_dict_get(p->table[i], "Parent"),2);
        pdf_array(pdf_dict_get(p->table[i], "MediaBox"),4);
        pdf_int(pdf_array_get(pdf_dict_get(p->table[i],"MediaBox"),0),0);
        pdf_int(pdf_array_get(pdf_dict_get(p->table[i],"MediaBox"),1),0);
        pdf_real(pdf_array_get(pdf_dict_get(p->table[i],"MediaBox"),2),w);
        pdf_real(pdf_array_get(pdf_dict_get(p->table[i],"MediaBox"),3),h);
        pdf_dictionary(pdf_dict_get(p->table[i], "Resources"));
        pdf_reference(pdf_dict_get(pdf_dict_get(p->table[i], "Resources"),"Font"),1);
        pdf_reference(pdf_dict_get(pdf_dict_get(p->table[i], "Resources"),"XObject"),3);
        pdf_null(pdf_dict_get(p->table[i], "Contents"));
   }
    return p;
}

void pdf_delete(pdf_t* p){
    unsigned int i=0;
    for(i=0; i<(p->nbTable); i++){
        pdf_object_free(p->table[i]);
        free(p->table[i]);
    }
    free(p);
}

size_t pdf_new_id(pdf_t* p){
    unsigned int i = p->nbTable;  p->nbTable++;
    if( ( p->table=(pdf_object_t**)realloc(p->table,(p->nbTable)*sizeof(pdf_object_t*)) )==NULL  ) FAIL()
    p->table[i]=NULL;
    p->table[i]=pdf_null(p->table[i]);
    return (size_t)i;
}

void pdf_save(const char* fname, pdf_t* p){
    unsigned int i=0; long int* octets = NULL;
    if( (octets=(long int*)calloc((p->nbTable),sizeof(long int)))==NULL ) FAIL()
    FILE* fd = fopen(fname,"wb+");
    if (fd==NULL) {
        fprintf(stderr,"Erreur d'accËs sur le fichier (%s).\n",fname);
        exit(EXIT_FAILURE);
    }
    fprintf(fd,"%%PDF-1.7\n%%”œ„‚\n");

    for(i=0; i<(p->nbTable); i++){
        octets[i]=ftell(fd);
        fprintf(fd,"%u 0 obj\n",i+1);
        pdf_fprint_object(fd,p->table[i]);
        fprintf(fd,"\nendobj\n\n");
    }

    long int octetsRef = ftell(fd);
    fprintf(fd,"xref\n0 %u\n%010d 65535 f\n",(p->nbTable)+1,0);
    for(i=0; i<p->nbTable; i++) fprintf(fd,"%010ld %05d n\n",octets[i],0);

    fprintf(fd,"trailer\n<<\n  /Root 1 0 R\n  /Size %u\n>>\n",(p->nbTable)+1);
    fprintf(fd,"startxref\n%ld\n",octetsRef);
    fprintf(fd,"%%%%EOF");

    free(octets);
    fclose(fd);
}

void pdf_set_content(pdf_t* p, unsigned int n, const char* instr) {
    size_t id = pdf_new_id(p);
    pdf_stream_from_string(p->table[id],instr);
    pdf_reference(pdf_dict_get(p->table[4+n],"Contents"),id);
}

bool jpeg_info(const void* i, size_t* width, size_t* height, size_t* bpc, const char** cs) {
  const unsigned char* im = (const unsigned char*)i;
  if (im[0]!=0xff || im[1]!=0xd8) return false;
  im += 2;

  while(im[0] == 0xff) {
    unsigned char frame_id = im[1];
    if (frame_id == 0xc0) break;
    size_t frame_length = 256 * im[2] + im[3];
    im += frame_length + 2;
  }
  size_t h = im[5] * 256 + im[6];
  size_t w = im[7] * 256 + im[8];
  size_t b = im[4];
  const char* c;
  switch(im[9]) {
    case 1: c = "DeviceGray"; break;
    case 3: c = "DeviceRGB"; break;
    case 4: c = "DeviceCMYK"; break;
    default: return false;
  }
  if (width) *width = w;
  if (height) *height = h;
  if (bpc) *bpc = b;
  if (cs) *cs = c;
  return true;
}

const char* pdf_load_image(pdf_t* p, const char* fname){
    char* nomIm = NULL;
    asprintf(&nomIm,"Im%u",p->nbImg); p->nbImg++; //pour crÈer Im0, Im1, Im2, Im3...

    size_t id = pdf_new_id(p);
    pdf_stream_from_file(p->table[id],fname);

    pdf_reference(pdf_dict_get(p->table[3],nomIm),id);
	free(nomIm);

    pdf_name(pdf_dict_get(p->table[id]->stream.sDico,"Type"),"XObject");
    pdf_name(pdf_dict_get(p->table[id]->stream.sDico,"Subtype"),"Image");

    size_t wIm=0; size_t hIm=0; size_t bpcIm=0; const char* csIm="";

    if(!jpeg_info(p->table[id]->stream.data,&wIm,&hIm,&bpcIm,&csIm)){
        fprintf(stderr,"Erreur, ce fichier (%s) ne semble pas Ítre un jpeg...\n",fname);
        exit(EXIT_FAILURE);
    }
    pdf_int(pdf_dict_get(p->table[id]->stream.sDico,"Width"),wIm);
    pdf_int(pdf_dict_get(p->table[id]->stream.sDico,"Height"),hIm);
    pdf_int(pdf_dict_get(p->table[id]->stream.sDico,"BitsPerComponent"),bpcIm);
    pdf_name(pdf_dict_get(p->table[id]->stream.sDico,"ColorSpace"),csIm);
    pdf_name(pdf_dict_get(p->table[id]->stream.sDico,"Filter"),"DCTDecode");

    dictionary_t* temp = p->table[3]->dico;
    while (temp->next!=NULL) temp=temp->next;
    return temp->key;
}

// Je n'ai pas aspritnf sur windows, donc je l'ai directement pris sur ce site : https://stackoverflow.com/questions/40159892/using-asprintf-on-windows
int _vscprintf_so(const char * format, va_list pargs) {
    int retval;
    va_list argcopy;
    va_copy(argcopy, pargs);
    retval = vsnprintf(NULL, 0, format, argcopy);
    va_end(argcopy);
    return retval;
}

int vasprintf(char **strp, const char *fmt, va_list ap) {
    int len = _vscprintf_so(fmt, ap);
    if (len == -1) return -1;
    char *str = malloc((size_t) len + 1);
    if (!str) return -1;
    int r = vsnprintf(str, len + 1, fmt, ap);
    if (r == -1) return free(str), -1;
    *strp = str;
    return r;
}

int asprintf(char *strp[], const char *fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    int r = vasprintf(strp, fmt, ap);
    va_end(ap);
    return r;
}