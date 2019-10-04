#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include "menu.h"
#include "svg.h"
#include "args.h"
#define NOOPTION (option_t*)NULL;


bool cont = true;


int main(int argc, char** argv) {
	
	if(argc>1) {
		
		option_t* opt = NOOPTION;

		opt = opt_string(opt, "-i", fctI);
		opt = opt_string(opt, "-o", fctO);
		opt = opt_float(opt, "-c", fctC);
		opt = opt_floatfloat(opt, "-r", fctR);
		opt = opt_float(opt, "-l", fctL);
		opt = opt_string(opt, "-x", fctX);
		opt = opt_string(opt, "-j", fctJ);
		opt = opt_int(opt, "-d", fctD);
		opt = opt_intfloat(opt, "-s", fctS);
		opt = opt_intfloat(opt, "-a", fctA);
		opt = opt_intfloatfloat(opt, "-t", fctT);
		opt = opt_string(opt, "-g", fctG);
		opt = opt_int(opt, "-u", fctU);
		
		process_options(opt, argc, argv);

		opt_delete(opt);
		good_bye();
	}
	
	else {

		menu_t* m;
		menu_t* sm;
	
	
		m = createMenu("Main menu");
		
		
		sm = createMenu("Fichier");
		addMenuAction(sm,"Ouvrir",ouvrir);
		addMenuAction(sm,"Enregistrer",enregistrer);	
		addMenuAction(sm,"Information",information);
		addMenuAction(sm,"Afficher",afficher);
		addMenuAction(sm,"Quitter",quit);
		addSubMenu(m,sm);

		sm = createMenu("Figure de base");
		addMenuAction(sm,"Ajouter cercle",add_c);
		addMenuAction(sm,"Ajouter rectangle",add_r);
		addMenuAction(sm,"Ajouter ligne",add_l);
		addMenuAction(sm,"Ajouter texte",add_t);
		addMenuAction(sm,"Ajouter image",add_i);
		addMenuAction(sm,"Supprimer",supprimons);
		addSubMenu(m,sm);

		sm = createMenu("Transformation et groupe");
		addMenuAction(sm,"Translater",translatons);
		addMenuAction(sm,"Tourner",tournons);
		addMenuAction(sm,"Etirer",etirons);
		addMenuAction(sm,"Grouper",groupons);
		addMenuAction(sm,"Dégrouper",degroupons);
		addSubMenu(m,sm);



		while(cont) {launchMenu(m);}

		deleteMenu(m);
	}
	
	return EXIT_SUCCESS;
}