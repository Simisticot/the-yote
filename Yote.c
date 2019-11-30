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

int main(int argc, char *argv[]){
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