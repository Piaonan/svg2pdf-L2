#ifndef XML_H_DU_PROJET
#define XML_H_DU_PROJET

typedef struct xattribute_s{
    char* name;
    char* value;
    struct xattribute_s* next;
}xattribute_t;

typedef struct xelement_s{
    char* name;
    xattribute_t* listAttrib;
    struct xelement_s* father;
    struct xelement_s* brother;
    union{
    char* raw;
    struct xelement_s* listSon;
    }contenu;
}xelement_t;

xelement_t* create_xelement(const char* name);
xelement_t* load_xelement(FILE* fd, const char* end_tag);
xelement_t* load_xml(const char* fname);
xattribute_t* add_xattribute(xelement_t* e, const char* name, const char* value);
void add_sub_xelement(xelement_t* e, xelement_t* s);
void add_raw(xelement_t* e, const char* r);
void delete_xelement(xelement_t* e);
void save_xml(const char* fname, xelement_t* e);






#endif