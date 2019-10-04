#ifndef PDF_H_DU_PROJET
#define PDF_H_DU_PROJET
#include <stdbool.h>

typedef enum kind_e{
    Null,
    Boolean,
    Entier,
    Reel,
    Reference,
    Nom,
    Chaine,
    Tableau,
    Stream,
    Dictionnaire
}kind_t;

typedef struct dictionary_s{
    char* key;
    struct pdf_object_s* value;
    struct dictionary_s* next;
}dictionary_t;

typedef struct stream_s{
    void* data;
    struct pdf_object_s* sDico;
}stream_t;

typedef struct pdf_object_s{
    union{
        bool b;
        int i;
        float f;
        unsigned int idRef;
        char* nom;
        char* chaine;
        struct pdf_object_s** table;
        stream_t stream;
        dictionary_t* dico;
    };
    kind_t genre;
    unsigned int nbTable;
}pdf_object_t;

typedef struct pdf_s{
    pdf_object_t** table;
    unsigned int nbTable;
    unsigned int nbImg;
}pdf_t;

pdf_object_t* pdf_dict_get(pdf_object_t* d, const char* cle);
pdf_t* pdf_create(unsigned int nb, float w, float h);
void pdf_delete(pdf_t* p);
void pdf_save(const char* fname, pdf_t* p);
void pdf_set_content(pdf_t* p, unsigned int n, const char* instr);
const char* pdf_load_image(pdf_t* p, const char* fname);
int asprintf(char *strp[], const char *fmt, ...);












#endif