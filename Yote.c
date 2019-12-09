#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include "SDL/SDL.h"
#include "SDL/SDL_ttf.h"

//Taille de la fenêtre SDL
#define WIDTH 900
#define HEIGHT 700

typedef struct GameEntry { //Structure renvoyée par le Menu pour démarer le jeu
	int playerNumber; //1 or 2
	int gameMode; //1:rapide 2:longue
	char pseudoJ1[11]; //10 caractères + \0
	char pseudoJ2[11];
} GameEntry;

void WindowsNameIcon(SDL_Surface*);
GameEntry Menu(SDL_Surface*);
void LoadMenuText(SDL_Surface*);
void SpecialYote(SDL_Surface*);
int ScoreRegleCredit(SDL_Surface*,int);
void LoadCreditText(SDL_Surface*);
void LoadReglesText(SDL_Surface*);
void LoadScoreText(SDL_Surface*);
int Jouer(SDL_Surface*,int,GameEntry*);
void LoadPlayerText(SDL_Surface*);
void LoadModeText(SDL_Surface*);
void LoadPseudoText(SDL_Surface*,int,GameEntry*);
char GetLetter(int);
void LoadRecapText(SDL_Surface*,GameEntry*);
void DrawRectangle(SDL_Surface*,int,int,int,int,int,int,int);
void DrawText(SDL_Surface*,int,int,char[255],int,int,int,int,char[255]);
void DrawTextShaded(SDL_Surface*,int,int,char[255],int,int,int,int,char[255],int,int,int);
SDL_Color GetPixelColor(SDL_Surface*,int,int);
char *substr(char*,int,int);
void lastCharDel(char*);

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

	SDL_Surface *screen = NULL;
	GameEntry gameEntry; //Utilisée pour le Menu, contient les informations necessaires pour lancer le jeu

	//SDL_INIT
	if(SDL_Init(SDL_INIT_VIDEO) == -1) {
		fprintf(stderr, "SDL_Init error: %s\n", SDL_GetError());
		exit(1);
	}

	//SDL_SETVIDEOMODE
	screen = SDL_SetVideoMode(WIDTH, HEIGHT, 32, SDL_HWSURFACE | SDL_DOUBLEBUF);
	if (screen == NULL){
		fprintf(stderr, "SDL_SetVideoMode error : %s\n", SDL_GetError());
		exit(1);
	}

	//SDL_TTF
	if(TTF_Init() == -1){
		fprintf(stderr, "TTF_Init error : %s\n", TTF_GetError());
		exit(1);
	}

	WindowsNameIcon(screen); //Nom et icone fenêtre

	gameEntry = Menu(screen); //Menu

	printf("Nbrlayer:%d - GameMode:%d - PseudoJ1:%s - PseudoJ2:%s \n",gameEntry.playerNumber,gameEntry.gameMode,gameEntry.pseudoJ1,gameEntry.pseudoJ2); //Affichage terminal

	//Free
	TTF_Quit();
	SDL_FreeSurface(screen);
	SDL_Quit();

	return 0;
}

void WindowsNameIcon(SDL_Surface* screen){ //Donne un nom à la fenêtre SDL et définie un icone (32x32 format bmp) au programme
	SDL_Surface *icon = NULL;

	icon = SDL_LoadBMP("Images/Yote.bmp");
	SDL_WM_SetIcon(icon, NULL);

	SDL_WM_SetCaption("The Yote", NULL);

	SDL_FreeSurface(icon);
}

GameEntry Menu(SDL_Surface* screen){ //Menu principal
	GameEntry gameEntryMenu;
	int continuer = 1, fini=0; //continuer pour la boucle while (0 pour sortir), fini pour savoir si on a entièrement remplie GameEntry (1 pour valider)
	SDL_Event event;

	gameEntryMenu.pseudoJ1[0]='\0'; //Initialisation des deux pseudos à vide
	gameEntryMenu.pseudoJ2[0]='\0';

	DrawRectangle(screen,0,0,WIDTH,HEIGHT,50,50,50); //Fond

	LoadMenuText(screen); //Affichage menu principal
	SDL_Flip(screen);

	SDL_EnableKeyRepeat(10,150); //Permet de rester appuyé sur la même touche

	while (continuer){
		SDL_WaitEvent(&event);
		switch(event.type){
			case SDL_QUIT: //Croix de la fenêtre SDL = quitter
				continuer = 0;
				break;
			case SDL_KEYDOWN:
				switch (event.key.keysym.sym){
					case SDLK_ESCAPE: //Touche Echap = quitter
						continuer = 0;
						break;
					default:
						break;
				}
				break;
			case SDL_MOUSEBUTTONDOWN:
				if (event.button.button==SDL_BUTTON_LEFT){
					if(event.button.x>275 && event.button.x<630 && event.button.y>65 && event.button.y<160){ //Clic sur Yote
						SpecialYote(screen);
					} else if(event.button.x>370 && event.button.x<520 && event.button.y>255 && event.button.y<315){ //Clic sur Jouer
						do{ //Boucle des 5 pages consécutives de Jouer, permet d'avancer et de revenir en arrière
							//Jouer peut renvoyer +1 (étape suivante), -1 (étape précédente) ou -10 (quitter)
							switch(continuer){
								case 1:
									continuer += Jouer(screen,1,&gameEntryMenu);
									break;
								case 2:
									continuer += Jouer(screen,2,&gameEntryMenu);
									break;
								case 3:
									continuer += Jouer(screen,3,&gameEntryMenu);
									break;
								case 4:
									if(gameEntryMenu.playerNumber==2){ //Si on a choisie 1 joueur, on passe pseudoJ2 et on vas directement au récapitulatif
										continuer += Jouer(screen,4,&gameEntryMenu);
									} else {
										continuer=5;
									}
									break;
								case 5:
									continuer += Jouer(screen,5,&gameEntryMenu);
									if(gameEntryMenu.playerNumber==1 && continuer==4){  //Si on a choisie 1 joueur, Retour saute pseudoJ2 et ramène à pseudoJ1
										continuer=3;
									}
									break;
								default:
									break;
							}
						}while(continuer>0 && continuer<6);
						if(continuer==0){ //Revenir au menu - obtenue quand clic sur Retour dans la page de choix nombre de joueur
							DrawRectangle(screen,0,0,WIDTH,HEIGHT,50,50,50);
							LoadMenuText(screen);
							SDL_Flip(screen);
							gameEntryMenu.pseudoJ1[0]='\0'; //Si on est revenu on Menu principal, on oublis les pseudos et fini reviens à 0
							gameEntryMenu.pseudoJ2[0]='\0';
							continuer=1;
							fini=0;
						} else if(continuer<=-5){ //Quitter complètement - return -10 d'une des pages Jouer, continuer sera entre -5 et -9
							continuer=0;
						} else { //Complètement fini de remplir GameEntry, donc fini=1 et quitte boucle while avec continuer=0
							fini=1;
							continuer=0;
						}
					} else if(event.button.x>365 && event.button.x<530 && event.button.y>325 && event.button.y<370){ //Clic sur Score
						continuer = ScoreRegleCredit(screen,1);
					} else if(event.button.x>355 && event.button.x<540 && event.button.y>395 && event.button.y<445){ //Clic sur Quitter
						continuer = 0;
					} else if(event.button.y>635){
						if(event.button.x<175){
							continuer = ScoreRegleCredit(screen,2); //Clic sur Règles
						} else if(event.button.x>705){
							continuer = ScoreRegleCredit(screen,3); //Clic sur Crédits
						}
					}
				}
				break;
			case SDL_MOUSEMOTION:
				if(event.motion.x>370 && event.motion.x<520 && event.motion.y>255 && event.motion.y<315){ //Curseur sur Jouer
					DrawTextShaded(screen,375,250,"Polices/LibreBaskerville-Regular.ttf",50,255,0,0,"Jouer",50,50,50);
				} else if (event.motion.x>365 && event.motion.x<530 && event.motion.y>325 && event.motion.y<370){ //Curseur sur Score
					DrawTextShaded(screen,365,320,"Polices/LibreBaskerville-Regular.ttf",50,255,0,0,"Scores",50,50,50);
				} else if (event.motion.x>355 && event.motion.x<540 && event.motion.y>395 && event.motion.y<445){ //Curseur sur Quitter
					DrawTextShaded(screen,355,385,"Polices/LibreBaskerville-Regular.ttf",50,255,0,0,"Quitter",50,50,50);
				} else if (event.motion.y>635 && event.motion.x<175){ //Curseur sur Règles
					DrawTextShaded(screen,10,630,"Polices/LibreBaskerville-Regular.ttf",50,255,0,0,"Règles",50,50,50);
				} else if(event.motion.y>635 && event.motion.x>705){ //Curseur sur Crédits
					DrawTextShaded(screen,705,637,"Polices/LibreBaskerville-Regular.ttf",50,255,0,0,"Crédits",50,50,50);
				} else {
					DrawTextShaded(screen,375,250,"Polices/LibreBaskerville-Regular.ttf",50,255,255,255,"Jouer",50,50,50); //Si Curseur sur rien, on écrit tout en blanc pour effacer
					DrawTextShaded(screen,365,320,"Polices/LibreBaskerville-Regular.ttf",50,255,255,255,"Scores",50,50,50);
					DrawTextShaded(screen,355,385,"Polices/LibreBaskerville-Regular.ttf",50,255,255,255,"Quitter",50,50,50);
					DrawTextShaded(screen,10,630,"Polices/LibreBaskerville-Regular.ttf",50,255,255,255,"Règles",50,50,50);
					DrawTextShaded(screen,705,637,"Polices/LibreBaskerville-Regular.ttf",50,255,255,255,"Crédits",50,50,50);
					//Amélioration possible si on ne veut pas que le else se fasse à chaque fois que le curseur bouge et qu'il n'est pas sur un élément, à voir si ca bug
				}
				SDL_Flip(screen);
				break;
			default:
				break;
		}
	}

	if(fini!=1){
		gameEntryMenu.playerNumber = 0; //Si on a pas tout remplie GameEntry, on efface
		gameEntryMenu.gameMode = 0;
		gameEntryMenu.pseudoJ1[0]='\0';
		gameEntryMenu.pseudoJ2[0]='\0';
	}
	return gameEntryMenu;
}

void LoadMenuText(SDL_Surface* screen){ //Affichage menu principal
	DrawTextShaded(screen,280,50,"Polices/LibreBaskerville-Regular.ttf",110,255,255,255,"YOTE",50,50,50);
	DrawTextShaded(screen,375,250,"Polices/LibreBaskerville-Regular.ttf",50,255,255,255,"Jouer",50,50,50);
	DrawTextShaded(screen,365,320,"Polices/LibreBaskerville-Regular.ttf",50,255,255,255,"Scores",50,50,50);
	DrawTextShaded(screen,355,385,"Polices/LibreBaskerville-Regular.ttf",50,255,255,255,"Quitter",50,50,50);
	DrawTextShaded(screen,10,630,"Polices/LibreBaskerville-Regular.ttf",50,255,255,255,"Règles",50,50,50);
	DrawTextShaded(screen,705,637,"Polices/LibreBaskerville-Regular.ttf",50,255,255,255,"Crédits",50,50,50);
}

void SpecialYote(SDL_Surface* screen){ //Change couleur aléatoire du texte Yote dans le Menu
	SDL_Color pixelColorYote;
	do{
		pixelColorYote.r = rand()%(255-0)+0;
		pixelColorYote.g = rand()%(255-0)+0;
		pixelColorYote.b = rand()%(255-0)+0;
	}while(pixelColorYote.r==50 && pixelColorYote.g==50 && pixelColorYote.b==50);
	DrawTextShaded(screen,280,50,"Polices/LibreBaskerville-Regular.ttf",110,pixelColorYote.r,pixelColorYote.g,pixelColorYote.b,"YOTE",50,50,50);
	SDL_Flip(screen);
}

int ScoreRegleCredit(SDL_Surface* screen, int choix){ //Page Score, Règles et Crédits respectivement choix = 1, 2 ou 3	int continuer = 1;
	SDL_Event event;
	int continuer=1;

	DrawRectangle(screen,0,0,WIDTH,HEIGHT,50,50,50); //Fond
	switch(choix){ //Choisie quoi afficher selon choix
		case 1:
			LoadScoreText(screen);
			break;
		case 2:
			LoadReglesText(screen);
			break;
		case 3:
			LoadCreditText(screen);
			break;
		default:
			break;
	}
	DrawTextShaded(screen,20,10,"Polices/LibreBaskerville-Regular.ttf",50,255,255,255,"Retour",50,50,50); //Affichage bouton Retour
	SDL_Flip(screen);

	while (continuer){
		SDL_WaitEvent(&event);
		switch(event.type){
			case SDL_QUIT: //Croix de la fenêtre SDL = quitter
				return 0; //Le zero retourné vas aller dans la variable continuer de Menu, donc stopper Menu
			case SDL_KEYDOWN:
				switch (event.key.keysym.sym){
					case SDLK_ESCAPE: //Touche Echap = quitter
						return 0; //Le zero retourné vas aller dans la variable continuer de Menu, donc stopper Menu
					default:
						break;
				}
				break;
			case SDL_MOUSEBUTTONDOWN:
				if (event.button.button == SDL_BUTTON_LEFT){
					if(event.button.x<200 && event.button.y<60){ //Clic sur Retour
							continuer = 0; //Quitte boucle while de ScoreRegleCredit, vas donc renvoyer 1 dans la variable continuer de Menu, donc ne quittera pas le while de Menu
					}
				}
				break;
			case SDL_MOUSEMOTION:
				if(event.motion.x<200 && event.motion.y<60){ //Curseur sur Retour
					DrawTextShaded(screen,20,10,"Polices/LibreBaskerville-Regular.ttf",50,255,0,0,"Retour",50,50,50);
				} else { //Efface le surlignage
					DrawTextShaded(screen,20,10,"Polices/LibreBaskerville-Regular.ttf",50,255,255,255,"Retour",50,50,50);
				}
				SDL_Flip(screen);
				break;
			default:
				break;
		}
	}

	DrawRectangle(screen,0,0,WIDTH,HEIGHT,50,50,50); //Si on arrive içi, c'est que l'on a cliqué sur Retour, donc on affiche le Menu
	LoadMenuText(screen);
	SDL_Flip(screen);
	return 1; //Ce 1 va dans la variable continuer du Menu
}

void LoadCreditText(SDL_Surface* screen){ //Affichage Crédits
	DrawTextShaded(screen,180,130,"Polices/LibreBaskerville-Regular.ttf",22,255,255,255,"Ce jeu a été réalisé pour le projet algorithmique",50,50,50);
	DrawTextShaded(screen,65,160,"Polices/LibreBaskerville-Regular.ttf",22,255,255,255,"de première année d'école d'ingénieurs ISTY de Vélizy-Villacoublay.",50,50,50);
	DrawTextShaded(screen,205,190,"Polices/LibreBaskerville-Regular.ttf",22,255,255,255,"Ce projet a été dirigé par ABOUDA Dhekra",50,50,50);
	DrawTextShaded(screen,135,280,"Polices/LibreBaskerville-Regular.ttf",22,255,255,255,"Le programme a été pensé, modélisé et programmé par",50,50,50);
	DrawTextShaded(screen,360,330,"Polices/LibreBaskerville-Regular.ttf",25,255,255,255,"Julie GASPAR",50,50,50);
	DrawTextShaded(screen,340,370,"Polices/LibreBaskerville-Regular.ttf",25,255,255,255,"Rémy LEBAILLY",50,50,50);
	DrawTextShaded(screen,280,410,"Polices/LibreBaskerville-Regular.ttf",25,255,255,255,"Laureline PARTONNAUD",50,50,50);
	DrawTextShaded(screen,345,450,"Polices/LibreBaskerville-Regular.ttf",25,255,255,255,"Simon PEREIRA",50,50,50);
	DrawTextShaded(screen,325,490,"Polices/LibreBaskerville-Regular.ttf",25,255,255,255,"Yannick STACHIVI",50,50,50);
	DrawTextShaded(screen,180,600,"Polices/LibreBaskerville-Regular.ttf",22,255,255,255,"Nous vous remercions d'avoir joué au jeu YOTE",50,50,50);
}

void LoadReglesText(SDL_Surface* screen){ //Affichage Règles

}
void LoadScoreText(SDL_Surface* screen){ //Affichage Score

}

int Jouer(SDL_Surface* screen, int choix, GameEntry* gameEntry){ //Page Jouer et sa suite (Nombre joueur, Mode de jeu, PseudoJ1, PseudoJ2 et Récapitulatif) - variable choix définie quelle page on souhaite
	int continuer = 1;
	SDL_Event event;
	char letter[2]; //Variable récupérant une lettre + \0 - utilisé dans la formation des pseudos joueur

	DrawRectangle(screen,0,0,WIDTH,HEIGHT,50,50,50); //Fond

	switch(choix){ //Choisie quoi afficher selon choix
		case 1:
			LoadPlayerText(screen);
			break;
		case 2:
			LoadModeText(screen);
			break;
		case 3:
		case 4:
			LoadPseudoText(screen,choix-2,gameEntry); //choix-2 = 1 ou 2 respectivement pour J1 et J2
			break;
		case 5:
			LoadRecapText(screen,gameEntry);
		default:
			break;
	}
	DrawTextShaded(screen,20,10,"Polices/LibreBaskerville-Regular.ttf",50,255,255,255,"Retour",50,50,50); //Affichage bouton Retour
	SDL_Flip(screen);

	while (continuer){
		if(choix==3 || choix==4){
			LoadPseudoText(screen,choix-2,gameEntry); //L'affichage des pseudos doit être fait à chaque évênement
		}
		SDL_WaitEvent(&event);
		switch(event.type){
			case SDL_QUIT: //Croix de la fenêtre SDL = quitter
				return -10; //En retournant -10, la variable continuer du Menu sera obligatoirement <= -5
			case SDL_KEYDOWN:
				switch (event.key.keysym.sym){
					case SDLK_ESCAPE: //Touche Echap = quitter
						return -10; //En retournant -10, la variable continuer du Menu sera obligatoirement <= -5
					default:
						break;
				}
				if(choix==3){ //Gestion pseudo J1
					letter[0] = GetLetter(event.key.keysym.sym); //Remplie letter avec la touche appuyée
					letter[1] = '\0';
					if(letter[0]=='R'){ //Si appuyé sur la touche Entrer - Valide le pseudo, quitte while avec return +1 - si pseudo était vide, remplie par Joueur1
						if(strlen(gameEntry->pseudoJ1)==0){
							strcat(gameEntry->pseudoJ1,"Joueur1\0");
						}
						continuer=0;
					} else if(letter[0]=='B'){ //Si appuyé sur la touche Effacer
						lastCharDel(gameEntry->pseudoJ1); //Efface la fin du pseudo
					} else if(letter[0]!=' ' && strlen(gameEntry->pseudoJ1)<10){ //Si appuyé sur une touche valide du clavier et que l'on est à moins de 10 caractères dans le pseudo
						strcat(gameEntry->pseudoJ1,letter); //Ajoute la touche appuyée au pseudo
						strcat(gameEntry->pseudoJ2,"\0");
					}
				}
				if(choix==4){ //Gestion pseudo J2 - pareil à Joueur1 (choix=3 ci dessus)
					letter[0] = GetLetter(event.key.keysym.sym);
					letter[1] = '\0';
					if(letter[0]=='R'){
						if(strlen(gameEntry->pseudoJ2)==0){
							strcat(gameEntry->pseudoJ2,"Joueur2\0");
						}
						continuer=0;
					} else if(letter[0]=='B'){
						lastCharDel(gameEntry->pseudoJ2);
					} else if(letter[0]!=' ' && strlen(gameEntry->pseudoJ2)<10){
						strcat(gameEntry->pseudoJ2,letter);
						strcat(gameEntry->pseudoJ2,"\0");
					}
				}
				break;
			case SDL_MOUSEBUTTONDOWN:
				if (event.button.button == SDL_BUTTON_LEFT){
					if(event.button.x<200 && event.button.y<60){ //Clic sur Retour
						return -1; //Comme continuer += Jouer(...) , on vas revenur à l'étape précédente
					} else {
						switch(choix){
							case 1:
							case 2:
								if(event.button.x>55 && event.button.x<825 && event.button.y>260 && event.button.y<335){ //Clic sur JvJ / JvIA ou bien partie Rapide / Longue
									switch(choix){
										case 1:
											gameEntry->playerNumber = 2;
											strcpy(gameEntry->pseudoJ2,"\0"); //Si deux joueurs, On vide pseudoJ2 (car peut être remplie par CyberYote)
											break;
										case 2:
											gameEntry->gameMode = 1; //partie rapide
											break;
										default:
											break;
									}
									continuer = 0;
								} else if(event.button.x>115 && event.button.x<775 && event.button.y>410 && event.button.y<485){
									switch(choix){
										case 1:
											gameEntry->playerNumber = 1;
											strcpy(gameEntry->pseudoJ2,"CyberYote\0"); //Si un joueur, pseudoJ2 = CyberYote
											break;
										case 2:
											gameEntry->gameMode = 2; //partie longue
											break;
										default:
											break;
									}
									continuer = 0;
								}
								break;
							case 5:
								if(event.button.x>710 && event.button.y>590){ //Clic sur GO dans page Récapitulatif
									continuer = 0;
								}
								break;
							default:
								break;
						}
					}
				}
				break;
			case SDL_MOUSEMOTION:
				if(event.motion.x<200 && event.motion.y<60){ //Curseur sur Retour
					DrawTextShaded(screen,20,10,"Polices/LibreBaskerville-Regular.ttf",50,255,0,0,"Retour",50,50,50);
				} else if(event.motion.x>55 && event.motion.x<825 && event.motion.y>260 && event.motion.y<335){ //Curseur sur les choix de joueur ou de mode
					switch(choix){
						case 1:
							DrawTextShaded(screen,55,250,"Polices/LibreBaskerville-Regular.ttf",70,255,0,0,"JOUEUR vs JOUEUR",50,50,50);
							break;
						case 2:
							DrawTextShaded(screen,185,250,"Polices/LibreBaskerville-Regular.ttf",70,255,0,0,"partie RAPIDE",50,50,50);
							break;
						default:
							break;
					}
				} else if(event.motion.x>115 && event.motion.x<775 && event.motion.y>410 && event.motion.y<485){
					switch(choix){
						case 1:
							DrawTextShaded(screen,120,400,"Polices/LibreBaskerville-Regular.ttf",70,255,0,0,"JOUEUR vs ORDI",50,50,50);
							break;
						case 2:
							DrawTextShaded(screen,155,400,"Polices/LibreBaskerville-Regular.ttf",70,255,0,0,"partie LONGUE",50,50,50);
							break;
						default:
							break;
					}
				} else if(event.motion.x>710 && event.motion.y>590){ //Curseur sur Go
					if(choix==5){
						DrawTextShaded(screen,710,590,"Polices/LibreBaskerville-Regular.ttf",100,255,0,0,"GO",50,50,50);
					}
				} else { //Efface surlignage
					switch(choix){
						case 1:
							DrawTextShaded(screen,55,250,"Polices/LibreBaskerville-Regular.ttf",70,255,255,255,"JOUEUR vs JOUEUR",50,50,50);
							DrawTextShaded(screen,120,400,"Polices/LibreBaskerville-Regular.ttf",70,255,255,255,"JOUEUR vs ORDI",50,50,50);
							break;
						case 2:
							DrawTextShaded(screen,185,250,"Polices/LibreBaskerville-Regular.ttf",70,255,255,255,"partie RAPIDE",50,50,50);
							DrawTextShaded(screen,155,400,"Polices/LibreBaskerville-Regular.ttf",70,255,255,255,"partie LONGUE",50,50,50);
							break;
						case 5:
							DrawTextShaded(screen,710,590,"Polices/LibreBaskerville-Regular.ttf",100,255,255,255,"GO",50,50,50);
							break;
						default:
							break;
					}
					DrawTextShaded(screen,20,10,"Polices/LibreBaskerville-Regular.ttf",50,255,255,255,"Retour",50,50,50);
				}
				SDL_Flip(screen);
				break;
			default:
				break;
		}
	}
	return 1; //return 1 , donc continuer += 1 dans Menu, donc passe étape suivante
}

void LoadPlayerText(SDL_Surface* screen){ //Affichage choix nombre de joueur
	DrawTextShaded(screen,55,250,"Polices/LibreBaskerville-Regular.ttf",70,255,255,255,"JOUEUR vs JOUEUR",50,50,50);
	DrawTextShaded(screen,120,400,"Polices/LibreBaskerville-Regular.ttf",70,255,255,255,"JOUEUR vs ORDI",50,50,50);
}

void LoadModeText(SDL_Surface* screen){ //Affichage choix mode de jeu
	DrawTextShaded(screen,185,250,"Polices/LibreBaskerville-Regular.ttf",70,255,255,255,"partie RAPIDE",50,50,50);
	DrawTextShaded(screen,155,400,"Polices/LibreBaskerville-Regular.ttf",70,255,255,255,"partie LONGUE",50,50,50);
}

void LoadPseudoText(SDL_Surface* screen, int joueur, GameEntry* gameEntry){ //Affichage pseudo - int joueur pour J1 ou J2
	char text[17]; //On charge dans cette string le texte à afficher, dont on calculera l'espace qu'il occupera avant de l'afficher, puis on pourra le centrer
	TTF_Font *police = NULL;
	int w, h, size=80;

	police = TTF_OpenFont("Polices/LibreBaskerville-Regular.ttf",size);

	snprintf(text,17,"entrer pseudo J%d",joueur);
	DrawTextShaded(screen,175,180,"Polices/LibreBaskerville-Regular.ttf",70,255,255,255,text,50,50,50);

	DrawRectangle(screen,0,350,WIDTH,size*2,50,50,50); //Efface l'ancien pseudo entré
	SDL_Flip(screen);
	if(joueur==1){
		TTF_SizeText(police,gameEntry->pseudoJ1,&w,&h); //Calcul de la place qu'occupe le texte
		DrawTextShaded(screen,(WIDTH/2)-(w/2),350,"Polices/LibreBaskerville-Regular.ttf",size,255,255,255,gameEntry->pseudoJ1,50,50,50); //Le texte est centré
	} else {
		TTF_SizeText(police,gameEntry->pseudoJ2,&w,&h);
		DrawTextShaded(screen,(WIDTH/2)-(w/2),350,"Polices/LibreBaskerville-Regular.ttf",size,255,255,255,gameEntry->pseudoJ2,50,50,50);
	}
	SDL_Flip(screen);
	TTF_CloseFont(police);
}

char GetLetter(int eventKey){ //renvoie la lettre selon la touche appuyée, R si entrée et B si Effacer, ' ' si touche non valide
	switch(eventKey){
		case SDLK_a:
			return 'a';
		case SDLK_b:
			return 'b';
		case SDLK_c:
			return 'c';
		case SDLK_d:
			return 'd';
		case SDLK_e:
			return 'e';
		case SDLK_f:
			return 'f';
		case SDLK_g:
			return 'g';
		case SDLK_h:
			return 'h';
		case SDLK_i:
			return 'i';
		case SDLK_j:
			return 'j';
		case SDLK_k:
			return 'k';
		case SDLK_l:
			return 'l';
		case SDLK_m:
			return 'm';
		case SDLK_n:
			return 'n';
		case SDLK_o:
			return 'o';
		case SDLK_p:
			return 'p';
		case SDLK_q:
			return 'q';
		case SDLK_r:
			return 'r';
		case SDLK_s:
			return 's';
		case SDLK_t:
			return 't';
		case SDLK_u:
			return 'u';
		case SDLK_v:
			return 'v';
		case SDLK_w:
			return 'w';
		case SDLK_x:
			return 'x';
		case SDLK_y:
			return 'y';
		case SDLK_z:
			return 'z';
		case SDLK_0:
			return '0';
		case SDLK_1:
			return '1';
		case SDLK_2:
			return '2';
		case SDLK_3:
			return '3';
		case SDLK_4:
			return '4';
		case SDLK_5:
			return '5';
		case SDLK_6:
			return '6';
		case SDLK_7:
			return '7';
		case SDLK_8:
			return '8';
		case SDLK_9:
			return '9';
		case SDLK_RETURN:
			return 'R';
		case SDLK_BACKSPACE:
			return 'B';
		default:
			return ' ';
			break;
	}
}

void LoadRecapText(SDL_Surface* screen, GameEntry* gameEntry){ //Affichage page récapitulatif
	TTF_Font *police=NULL;
	int w, h, size=80;
	char text[15]; //Comme LoadPseudoText, utilise le principe de pré-calcul de la taille occupée pour centré les textes

	police = TTF_OpenFont("Polices/LibreBaskerville-Regular.ttf",size);

	if(gameEntry->playerNumber==1){
		sprintf(text,"%d joueur",gameEntry->playerNumber);
	} else {
		sprintf(text,"%d joueurs",gameEntry->playerNumber);
	}

	TTF_SizeText(police,text,&w,&h);
	DrawTextShaded(screen,(WIDTH/2)-(w/2),80,"Polices/LibreBaskerville-Regular.ttf",size,255,255,255,text,50,50,50);

	if(gameEntry->gameMode==1){
		strcpy(text,"Partie rapide");
	} else {
		strcpy(text,"Partie longue");
	}

	TTF_SizeText(police,text,&w,&h);
	DrawTextShaded(screen,(WIDTH/2)-(w/2),180,"Polices/LibreBaskerville-Regular.ttf",size,255,255,255,text,50,50,50);

	TTF_SizeText(police,gameEntry->pseudoJ1,&w,&h);
	DrawTextShaded(screen,(WIDTH/2)-(w/2),310,"Polices/LibreBaskerville-Regular.ttf",size,255,255,255,gameEntry->pseudoJ1,50,50,50);

	DrawTextShaded(screen,395,410,"Polices/LibreBaskerville-Regular.ttf",size,255,255,255,"VS",50,50,50);

	TTF_SizeText(police,gameEntry->pseudoJ2,&w,&h);
	DrawTextShaded(screen,(WIDTH/2)-(w/2),500,"Polices/LibreBaskerville-Regular.ttf",size,255,255,255,gameEntry->pseudoJ2,50,50,50);

	DrawTextShaded(screen,710,590,"Polices/LibreBaskerville-Regular.ttf",100,255,255,255,"GO",50,50,50);

	SDL_Flip(screen);
	TTF_CloseFont(police);
}

void DrawRectangle(SDL_Surface* screen, int posx, int posy, int width, int height, int red, int green, int blue){ //Affiche un rectangle, point haug-gauche de départ
	SDL_Surface *rectangle = NULL;
	SDL_Rect rectanglePos;
	Uint32 rectangleColor;

	rectanglePos.x=posx; rectanglePos.y=posy;
	rectangleColor = SDL_MapRGB(screen->format, red, green, blue);
	rectangle = SDL_CreateRGBSurface(SDL_HWSURFACE, width, height, 32, 0, 0, 0, 0);
	SDL_FillRect(rectangle, NULL, rectangleColor);
	SDL_BlitSurface(rectangle, NULL, screen, &rectanglePos);

	SDL_FreeSurface(rectangle);
}

void DrawText(SDL_Surface* screen, int posx, int posy, char font[255], int size, int red, int green, int blue, char text[255]){ //Affiche un texte, point haug-gauche de départ - voir différence sdl entre Solid Shaded et Blended
	TTF_Font *police = NULL;
	SDL_Color policeColor = {red,green,blue};

	SDL_Surface *texte = NULL;
	SDL_Rect textePosition;

	police = TTF_OpenFont(font,size);
	texte = TTF_RenderUTF8_Blended(police, text, policeColor); //Blended ou Solid
	textePosition.x = posx;
	textePosition.y = posy;

	SDL_BlitSurface(texte, NULL, screen, &textePosition);

	SDL_FreeSurface(texte);
	TTF_CloseFont(police);
}

void DrawTextShaded(SDL_Surface* screen, int posx, int posy, char font[255], int size, int red, int green, int blue, char text[255], int bgred, int bggreen, int bgblue){ //Affiche un texte, point haug-gauche de départ - donner la couleur de fond
	TTF_Font *police = NULL;
	SDL_Color policeColor = {red,green,blue};
	SDL_Color bgColor = {bgred,bggreen,bgblue};

	SDL_Surface *texte = NULL;
	SDL_Rect textePosition;

	police = TTF_OpenFont(font,size);
	texte = TTF_RenderUTF8_Shaded(police, text, policeColor,bgColor); //Blended ou Solid ou Shaded

	textePosition.x = posx;
	textePosition.y = posy;

	SDL_BlitSurface(texte, NULL, screen, &textePosition);

	SDL_FreeSurface(texte);
	TTF_CloseFont(police);
}

SDL_Color GetPixelColor(SDL_Surface* screen, int x, int y){ //renvoie une SDL_Color (continuant les valeurs RBG), du pixel aux coordonnées (x,y) - permet de savoir la couleur du pixel sur lequel on a cliqué
	SDL_Color pixelColor;
	Uint32* allPixels;
	Uint32 pixel;

	allPixels = (Uint32 *)screen->pixels;
	pixel = allPixels[(y * screen->w) + x];
	SDL_GetRGB(pixel, screen->format, &pixelColor.r, &pixelColor.g, &pixelColor.b);

	return pixelColor;
}

void lastCharDel(char* str){ //Supprime le dernier caractère d'une string - la string doit finir par un '\0'
	int len;
	len = strlen(str);
	str[len-1] = '\0';
}