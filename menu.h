#ifndef MENU_H_DU_PROJET
#define MENU_H_DU_PROJET

typedef struct {
  char* description;
  void (*fonction)();
}action_t;

typedef union item_u {
  struct menu_s* sous_menu;
  action_t action;
}item_t;

typedef struct menu_s {
  struct menu_s* papa;
  char* description;
  item_t set[9];
  int nbItem;
}menu_t;

menu_t* createMenu(const char* text);
void addMenuAction(menu_t* m, const char* text, void(*f)());
void addSubMenu(menu_t* m, menu_t* sm);
void deleteMenu(menu_t* m);
void launchMenu(menu_t* m);









#endif