#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include "SDL/SDL.h"

//Structures 

enum TYPE{
	VIDE, PION
};
typedef enum TYPE TYPE;

enum COUL{
	BLANC, NOIR
};
typedef enum COUL COUL;

struct NUMBOX{
	int l;
	int c;	
};
typedef struct NUMBOX NUMBOX;

struct BOX{
	TYPE typeP; /* {VIDE, PION} */
	COUL coulP; /* {BLANC, NOIR} */
};
typedef struct BOX BOX;

//Variable Globale contenant le plateau

BOX plateau[6][5];

//Sommaire des fonctions

//Modèle
void init_plateau();
COUL premier_joueur();
void placer_pion(NUMBOX position);
void retirer_pion(NUMBOX position);
void deplacer_pion(NUMBOX depart, NUMBOX destination);

//Vue
void affiche_plateau_debug();

int main(int argc, char *argv[]){
	init_plateau();

	NUMBOX placement;
	placement.c = 4;
	placement.l = 4;

	affiche_plateau_debug();
	placer_pion(placement);
	affiche_plateau_debug();

	NUMBOX destination;
	destination.c = 3;
	destination.l = 3;

	deplacer_pion(placement, destination);
	affiche_plateau_debug();

	retirer_pion(destination);
	affiche_plateau_debug();

    return 0;
}

//Modèle

void init_plateau(){
	int i,j;

	for (i=0;i < 6; i++){
		for(j=0; j < 5; j++){
				plateau[i][j].typeP = VIDE;
		}
	}
}

COUL premier_joueur(){
	srand(time(NULL));
	int tirage;

	tirage = rand()%2;

	if (tirage == 0)
	{
		return BLANC;
	}else{
		return NOIR;
	}
}

void placer_pion(NUMBOX position){
	plateau[position.l][position.c].typeP = PION;
}

void deplacer_pion(NUMBOX depart, NUMBOX destination){
	if (plateau[depart.l][depart.c].typeP == PION)
	{
		plateau[depart.l][depart.c].typeP = VIDE;
		plateau[destination.l][destination.c].typeP = PION;
	}
}

void retirer_pion(NUMBOX position){
	plateau[position.l][position.c].typeP = VIDE;
}


//Controlleur

//Vue

void affiche_plateau_debug(){

	int i,j;

	for (i = 0; i < 6; i++)
	{
		printf("(");
		for(j = 0; j < 5; j++){
			printf(" %d ", plateau[i][j].typeP);
		}

		printf(")\n");
	}
	printf("\n\n");
}