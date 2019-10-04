#ifndef SVG_H_DU_PROJET
#define SVG_H_DU_PROJET

typedef char color_t;

typedef struct {
	double x;
	double y;
}coord_t;

typedef enum {
	Cercle,
	Rectangle,
	Ligne
} figure_e;

struct circle_s{
	double carreRayon;
};

struct rectangle_s{
	double demiLargeur;
	double demiHauteur;
};

struct line_s{
	double demiLongueur;
};

typedef struct figure_s{
	color_t couleur;
	coord_t centre;
	double rotation;
	figure_e type;
	union{
		struct circle_s cercle;
		struct rectangle_s rectangle;
		struct line_s ligne;
	};
	struct figure_s* next;
}figure_t;

typedef struct {
	unsigned int width;
	unsigned int height;
	double grain;
	figure_t* first;
    unsigned int nbFigure;
}image_t;

void ouvrir();
void enregistrerSVG();
void enregistrerPDF();
void enregistrer();
void information();
void afficher();
void quit();

void add_c();
void add_r();
void add_l();
void add_t();
void add_i();
void supprimons();

void translatons();
void tournons();
void etirons();
void groupons();
void degroupons();

void good_bye();
void fctI(const char* str);
void fctO(const char* str);
void fctC(float f);
void fctR(float flo1, float flo2);
void fctL(float f);
void fctX(const char* str);
void fctJ(const char* str);
void fctD(int i);
void fctS(int i, float f);
void fctA(int i, float f);
void fctT(int i, float flo1, float flo2);
void fctG(const char* listInt);
void fctU(int i);














#endif