#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <math.h>
#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <SDL/SDL_ttf.h>
#include <SDL/SDL_gfxPrimitives.h>

//Taille de la fenêtre SDL
#define WIDTH 900
#define HEIGHT 700

////Structures////

typedef struct GameEntry { //Structure renvoyée par le Menu pour démarer le jeu
	int playerNumber; //1 ou 2
	int gameMode; //1:rapide 2:longue
	char pseudoJ1[11]; //10 caractères + \0
	char pseudoJ2[11];
} GameEntry;

enum TYPE{
	VIDE, PION
};
typedef enum TYPE TYPE;

enum COUL{
	BLANC, NOIR, NUL
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
	int Accessible; // 0 par défaut, 1 si accessible par le pion sélectionné lors du premier clic
};
typedef struct BOX BOX;

struct POINT{
	int x;
	int y;
};
typedef struct POINT POINT;

struct COUP{
	NUMBOX depart;
	NUMBOX arrivee;
	int typeCoup; // 0 pour un mouvement, 1 pour un placement de pion
};
typedef struct COUP COUP;

struct JOUEUR{
	int reserve;	/*[0;12]*/
	COUL coulj;     //?
	COUP dernierCoup;
};
typedef struct JOUEUR JOUEUR;



//Variable Globale contenant le plateau

BOX plateau[6][5];
int TOUR;   		//variable contenant 1 ou 2 et est mis à jour tour à tour.
JOUEUR J1;			//contenant la réserve 1
JOUEUR J2;			//contenant la réserve 2

////Sommaire des fonctions////

//Fonction du Menu
void WindowsNameIcon(SDL_Surface*); //Modele
GameEntry Menu(SDL_Surface*); //Modele
void LoadMenuText(SDL_Surface*); //Vue
void Fond(SDL_Surface*); //Vue
void SpecialYote(SDL_Surface*); //Vue
int ScoreRegleCredit(SDL_Surface*,int); //Modele
void LoadCreditText(SDL_Surface*); //Vue
void LoadReglesText(SDL_Surface*,int); //Vue
void LoadScoreText(SDL_Surface*); //Vue
void AddScore(char[11],char[11],int,int); //Controleur
int Jouer(SDL_Surface*,int,GameEntry*); //Modele
void LoadPlayerText(SDL_Surface*); //Vue
void LoadModeText(SDL_Surface*); //Vue
void LoadPseudoText(SDL_Surface*,int,GameEntry*); //Vue
char GetLetter(int); //Controleur
void LoadRecapText(SDL_Surface*,GameEntry*); //Vue
void DrawRectangle(SDL_Surface*,int,int,int,int,int,int,int); //Vue
void DrawTriangle(SDL_Surface*,int,int,int,int,int,int,int,int,int); //Vue
void DrawText(SDL_Surface*,int,int,char[255],int,int,int,int,char[255]); //Vue
void DrawTextShaded(SDL_Surface*,int,int,char[255],int,int,int,int,char[255],int,int,int); //Vue
SDL_Color GetPixelColor(SDL_Surface*,int,int); //Controleur
void lastCharDel(char*); //Controleur

//Modèle
void init_plateau();
COUL premier_joueur();
void placer_pion(NUMBOX position);
void retirer_pion(NUMBOX position);
void deplacer_pion(NUMBOX depart, NUMBOX destination);

void alterne_tour();				//change la valeur de tour {1;2}
void init_J1vsJ2();					//initialise les variables J1 et J2
void prendre_reserve();				//réduit la réserve correspondante à tour
int est_case_j(NUMBOX NB);			//vérifie si la case sélectionnée contient un pion du joueur 
int est_case_vide(NUMBOX NB);		//vérifie si la case sélectionnée est vide
int est_clic_reserve(POINT clic);	//vérifie si la case sélectionnée est dans la réserve correspondant à tour
int est_clic_plateau(POINT clic);	//vérifie si la case sélectionnée est dans le plateau
int est_clic1_valide();
int est_clic2_valide();
int est_coup_valide(COUP coup_courant);
int est_coup_capture(COUP coup);
int est_coup_inverse(COUP coup1, COUP coup2);
void identifier_cases_accessibles(NUMBOX depart);
void reinitialiser_cases_accessibles();
int est_case_accessible(NUMBOX destination);
int explorer_case(NUMBOX curseur);
int est_case_dans_le_plateau(NUMBOX curseur);
void applique_coup(COUP coup);
void choix_seconde_capture();
int nombre_pions_joueur(JOUEUR joueur);

//Vue
void affiche_plateau_debug();
void affiche_plateau(SDL_Surface* screen);

void affiche_pion(SDL_Surface* screen,NUMBOX NB);
void efface_pion(SDL_Surface* screen,NUMBOX NB);
void affichage_info_jeu(SDL_Surface* screen, GameEntry* gameEntry); //Affichage des pseudos, type de partie
void affiche_reserve(SDL_Surface* screen, JOUEUR J1, JOUEUR J2);
void affiche_tour(SDL_Surface* screen);
void affiche_coup(COUP coup, SDL_Surface* screen);

//Controleur
void wait_esc(SDL_Surface*);
POINT wait_clic(SDL_Surface* screen);
POINT numbox_to_point(NUMBOX NB);
NUMBOX point_to_numbox(POINT P);
void quitter(SDL_Surface* screen);
POINT event_to_point(SDL_Event event);

//     MAIN     ///////////////////////////////////////////////////////////////////////////////////////////
int main(int argc, char *argv[]){

	SDL_Surface *screen = NULL;
	GameEntry gameEntry; //Utilisée pour le Menu, contient les informations necessaires pour lancer le jeu

	int i;
	int hauteur;
	int largeur;

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

	if(gameEntry.playerNumber==0){
		printf("Quit Menu\n");
		return 0;
	} else {
		Fond(screen);
		
	//INITIALISATION
	init_J1vsJ2(J1,J2);
	init_plateau();
	affiche_plateau(screen);
	affichage_info_jeu(screen, &gameEntry);
	affiche_reserve(screen, J1, J2);
	TOUR=1;
	affiche_tour(screen);
	
	SDL_Flip(screen);
	//TOURS
	
	SDL_Event input;
	POINT clic1; 
	POINT clic2;
	NUMBOX seconde_capture;
	NUMBOX NB1; 
	NUMBOX NB2; 
	COUP coup_courant;
	int k; k=0; // partie en cours si k = 0, partie terminée si k = 1
	
	while (k==0){
				printf("J%d \n", TOUR);
		do{
			do{
				do{
					SDL_WaitEvent(&input);
					if(input.type == SDL_QUIT || (input.type == SDL_KEYDOWN && input.key.keysym.sym == SDLK_ESCAPE)){
						k=1;
					}
				}while(!(input.type == SDL_MOUSEBUTTONDOWN && input.button.button == SDL_BUTTON_LEFT) && k==0);
				printf("est un clic\n");
				clic1=event_to_point(input);
				printf("clic1 %d %d \n",clic1.x,clic1.y);

			}while(k==0 && !est_clic1_valide(clic1));
			do{
				do{
					SDL_WaitEvent(&input);
					if(input.type == SDL_QUIT || (input.type == SDL_KEYDOWN && input.key.keysym.sym == SDLK_ESCAPE)){
						k=1;
					}
				}while(!(input.type == SDL_MOUSEBUTTONDOWN && input.button.button == SDL_BUTTON_LEFT) && k==0);
				clic2=event_to_point(input);
				printf("clic2 %d %d \n",clic2.x,clic2.y);

			}while(k==0 && !est_clic_plateau(clic2));

			if(!est_clic_plateau(clic1)){
				coup_courant.arrivee = point_to_numbox(clic2);
				coup_courant.typeCoup = 1;
				printf("type 1\n");
			}else{
				coup_courant.depart = point_to_numbox(clic1);
				coup_courant.arrivee = point_to_numbox(clic2);
				coup_courant.typeCoup = 0;
				printf("type 0\n");
			}
			printf("coup cree\n");
			printf("%d\n",est_coup_valide(coup_courant));

		}while(k==0 && !est_coup_valide(coup_courant));

		if(k==0){
			applique_coup(coup_courant);
			printf("applique\n");
			affiche_coup(coup_courant, screen);
			SDL_Flip(screen);
		}

		if(est_coup_capture(coup_courant)){
			do{
				SDL_WaitEvent(&input);
			}while(!(input.type == SDL_MOUSEBUTTONDOWN && input.button.button == SDL_BUTTON_LEFT) && k==0);
			clic1 = event_to_point(input);
			if(est_clic_plateau(clic1)){
				seconde_capture=point_to_numbox(clic1);
				if(!est_case_vide(seconde_capture) && !est_case_j(seconde_capture)){
					retirer_pion(seconde_capture);
					efface_pion(screen, seconde_capture);
					SDL_Flip(screen);
				}
			}else if(est_clic_reserve(clic1)){

				if(TOUR == 1 && nombre_pions_joueur(J2) == 0){
					prendre_reserve(2);
					printf("prise dans reserve 2\n");
				}else if(TOUR == 2 && nombre_pions_joueur(J1) == 0){
					prendre_reserve(1);
					printf("prise dans reserve 1\n");
				}
			}
		}
		printf("affiche\n");
		if(TOUR == 1){
			J1.dernierCoup = coup_courant;
		}else{
			J2.dernierCoup = coup_courant;
		}

		alterne_tour();
		affiche_tour(screen);
		affiche_reserve(screen, J1, J2);
		SDL_Flip(screen);
		printf("J1 : %d pions\n", nombre_pions_joueur(J1));
		printf("J2 : %d pions\n", nombre_pions_joueur(J2));
		affiche_plateau_debug();
	}
		quitter(screen);
		printf("Quit Jeu\n");
		return 0;
	}
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

	Fond(screen);

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
							Fond(screen);
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

void Fond(SDL_Surface* screen){ //Affiche le fond - Efface l'écran
	DrawRectangle(screen,0,0,WIDTH,HEIGHT,50,50,50);
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
	int continuer=1, page=1;

	Fond(screen); //Fond
	switch(choix){ //Choisie quoi afficher selon choix
		case 1:
			LoadScoreText(screen);
			break;
		case 2:
			LoadReglesText(screen,page);
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
					if(choix==2){
						if(event.button.x>830 && event.button.y<80){ //Clic sur Retour
							page=2;
						}
						if(event.button.x>790 &&event.button.x<829 && event.button.y<80){ //Clic sur Retour
							page=1;
						}
						LoadReglesText(screen,page);
					}
				}
				break;
			case SDL_MOUSEMOTION:
				if(event.motion.x<200 && event.motion.y<60){ //Curseur sur Retour
					DrawTextShaded(screen,20,10,"Polices/LibreBaskerville-Regular.ttf",50,255,0,0,"Retour",50,50,50);
				} else if(event.motion.x>830 && event.motion.y<80 && page==1 && choix==2){ //Curseur sur flèche
					DrawTriangle(screen,840,20,870,50,840,80,255,0,0);
				} else if(event.motion.x>790 && event.motion.x<829 && event.motion.y<80 && page==2 && choix==2){
					DrawTriangle(screen,820,20,790,50,820,80,255,0,0);
				} else { //Efface le surlignage
					DrawTextShaded(screen,20,10,"Polices/LibreBaskerville-Regular.ttf",50,255,255,255,"Retour",50,50,50);
					if(page==1 && choix==2){
						DrawTriangle(screen,840,20,870,50,840,80,255,255,255);
					} else if(page==2 && choix==2){
						DrawTriangle(screen,820,20,790,50,820,80,255,255,255);
					}
				}
				SDL_Flip(screen);
				break;
			default:
				break;
		}
	}

	Fond(screen); //Si on arrive içi, c'est que l'on a cliqué sur Retour, donc on affiche le Menu
	LoadMenuText(screen);
	SDL_Flip(screen);
	return 1; //Ce 1 va dans la variable continuer du Menu
}

void LoadCreditText(SDL_Surface* screen){ //Affichage Crédits
	DrawTextShaded(screen,380,10,"Polices/LibreBaskerville-Regular.ttf",40,255,255,255,"Crédits",50,50,50);
	DrawRectangle(screen,380,60,150,2,255,255,255);
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

void LoadReglesText(SDL_Surface* screen, int page){ //Affichage Règles
	Fond(screen);
	DrawTextShaded(screen,380,10,"Polices/LibreBaskerville-Regular.ttf",40,255,255,255,"Règles",50,50,50);
	DrawRectangle(screen,380,60,135,2,255,255,255);

	if(page==1){
		DrawTextShaded(screen,20,100,"Polices/LibreBaskerville-Regular.ttf",19,255,255,255,"Le Yoté se joue sur un plateau de 5 cases sur 6 et 24 pions (12 de chaque couleur).",50,50,50);
		DrawTextShaded(screen,20,150,"Polices/LibreBaskerville-Regular.ttf",19,255,255,255,"Au début de la partie, le plateau de jeu est vide. Chaque joueur dispose de 12 pions dans",50,50,50);
		DrawTextShaded(screen,20,175,"Polices/LibreBaskerville-Regular.ttf",19,255,255,255,"une réserve.",50,50,50);
		DrawTextShaded(screen,20,225,"Polices/LibreBaskerville-Regular.ttf",19,255,255,255,"Le joueur qui commence à jouer est tiré au sort. Le but du jeu est d’éliminer tous les pions",50,50,50);
		DrawTextShaded(screen,20,250,"Polices/LibreBaskerville-Regular.ttf",19,255,255,255,"de l'adversaire du plateau de jeu. Les pions ne sont pas placés sur le plateau en début de",50,50,50);
		DrawTextShaded(screen,20,275,"Polices/LibreBaskerville-Regular.ttf",19,255,255,255,"partie mais au fur et à mesure de cette dernière.",50,50,50);
		DrawTextShaded(screen,20,325,"Polices/LibreBaskerville-Regular.ttf",19,255,255,255,"Tour à tour, chaque joueur peut soit poser un de ses pions sur une case libre du plateau,",50,50,50);
		DrawTextShaded(screen,20,350,"Polices/LibreBaskerville-Regular.ttf",19,255,255,255,"soit déplacer un de ses pions déjà posé, horizontalement ou verticalement, d’une case.",50,50,50);
		DrawTextShaded(screen,20,395,"Polices/LibreBaskerville-Regular.ttf",19,255,255,255,"Cette dernière doit être libre. Il n'est pas nécessaire d'avoir posé tous ses pions pour",50,50,50);
		DrawTextShaded(screen,20,420,"Polices/LibreBaskerville-Regular.ttf",19,255,255,255,"commencer les déplacements.",50,50,50);
		DrawTextShaded(screen,20,470,"Polices/LibreBaskerville-Regular.ttf",19,255,255,255,"Pour prendre un pion adverse il suffit de sauter par-dessus celui-ci, en avant ou en",50,50,50);
		DrawTextShaded(screen,20,495,"Polices/LibreBaskerville-Regular.ttf",19,255,255,255,"arrière, horizontalement ou verticalement, mais jamais en diagonale. Il faut donc que la",50,50,50);
		DrawTextShaded(screen,20,520,"Polices/LibreBaskerville-Regular.ttf",19,255,255,255,"case derrière celle du pion à prendre soit libre tout en étant dans l’alignement pion du",50,50,50);
		DrawTextShaded(screen,20,545,"Polices/LibreBaskerville-Regular.ttf",19,255,255,255,"joueur et du pion adverse. Lors d’une prise, le pion avance donc de deux cases.",50,50,50);
		DrawTextShaded(screen,20,595,"Polices/LibreBaskerville-Regular.ttf",19,255,255,255,"De plus, chaque pion pris permet d'en capturer un deuxième, choisi parmi les pions",50,50,50);
		DrawTextShaded(screen,20,620,"Polices/LibreBaskerville-Regular.ttf",19,255,255,255,"adverses présents sur le plateau. On capture donc 2 pions à chaque prise. Il n’est pas",50,50,50);
		DrawTextShaded(screen,20,645,"Polices/LibreBaskerville-Regular.ttf",19,255,255,255,"possible de prendre plus de deux pions par coup.",50,50,50);
	} else if(page==2){
		DrawTextShaded(screen,20,100,"Polices/LibreBaskerville-Regular.ttf",19,255,255,255,"Lors d’une prise, si un pion est capturé et que l’on souhaite capturer le deuxième, si jamais",50,50,50);
		DrawTextShaded(screen,20,125,"Polices/LibreBaskerville-Regular.ttf",19,255,255,255,"il n’y a plus de pion présent sur le plateau, exceptionnellement un pion sera pris dans la",50,50,50);
		DrawTextShaded(screen,20,150,"Polices/LibreBaskerville-Regular.ttf",19,255,255,255,"réserve de l’adversaire.",50,50,50);
		DrawTextShaded(screen,20,200,"Polices/LibreBaskerville-Regular.ttf",19,255,255,255,"Lorsqu’un pion est introduit en jeu, il ne peut pas être utilisé lors du même tour pour",50,50,50);
		DrawTextShaded(screen,20,225,"Polices/LibreBaskerville-Regular.ttf",19,255,255,255,"faire une prise.",50,50,50);
		DrawTextShaded(screen,20,275,"Polices/LibreBaskerville-Regular.ttf",19,255,255,255,"Si lors d’un tour, un joueur avance son pion, il ne peut pas faire l’inverse de ce mouvement",50,50,50);
		DrawTextShaded(screen,20,300,"Polices/LibreBaskerville-Regular.ttf",19,255,255,255,"lors de son prochain tour.",50,50,50);
		DrawTextShaded(screen,20,350,"Polices/LibreBaskerville-Regular.ttf",19,255,255,255,"Le jeu prend fin de deux manière différentes :",50,50,50);
		DrawTextShaded(screen,60,375,"Polices/LibreBaskerville-Regular.ttf",19,255,255,255,"-La version rapide : un joueur perd lorsqu'il n’a plus de pion sur le plateau de jeu",50,50,50);
		DrawTextShaded(screen,70,400,"Polices/LibreBaskerville-Regular.ttf",19,255,255,255,"même s'il lui en reste dans sa réserve.",50,50,50);
		DrawTextShaded(screen,60,425,"Polices/LibreBaskerville-Regular.ttf",19,255,255,255,"-La version longue : un joueur perd lorsqu’il n’a plus de pion ni sur le plateau ni sans",50,50,50);
		DrawTextShaded(screen,70,450,"Polices/LibreBaskerville-Regular.ttf",19,255,255,255,"la réserve.",50,50,50);
		DrawTextShaded(screen,20,500,"Polices/LibreBaskerville-Regular.ttf",19,255,255,255,"Si les deux joueurs se retrouvent à égalité (tous les pions du plateau ont été capturés et qu’il",50,50,50);
		DrawTextShaded(screen,20,525,"Polices/LibreBaskerville-Regular.ttf",19,255,255,255,"ne reste plus que 4 pions sur le plateau, deux pour chaque joueur), la fin de partie sera",50,50,50);
		DrawTextShaded(screen,20,550,"Polices/LibreBaskerville-Regular.ttf",19,255,255,255,"jouée en 10 coups. À la fin de ces 10 coups s’il n’y a pas de prise par un joueur la partie est",50,50,50);
		DrawTextShaded(screen,20,575,"Polices/LibreBaskerville-Regular.ttf",19,255,255,255,"considérée comme nulle.",50,50,50);
		DrawTextShaded(screen,20,625,"Polices/LibreBaskerville-Regular.ttf",19,255,255,255,"À tout moment, il est possible d'appuyer sur la touche 'Echap' de votre clavier pour quitter",50,50,50);
		DrawTextShaded(screen,20,650,"Polices/LibreBaskerville-Regular.ttf",19,255,255,255,"le jeu.",50,50,50);
	}
}

void LoadScoreText(SDL_Surface* screen){ //Affichage Score
	FILE* file = NULL;
	TTF_Font *police = NULL;
	char chaine[40], pseudoJ1[11], pseudoJ2[11];
	int scoreJ1, scoreJ2, i=0, hauteur=110, w, h, size=25;

	police = TTF_OpenFont("Polices/LibreBaskerville-Regular.ttf",size);

	DrawTextShaded(screen,380,10,"Polices/LibreBaskerville-Regular.ttf",40,255,255,255,"Scores",50,50,50);
	DrawRectangle(screen,380,60,135,2,255,255,255);

	file = fopen("scores.txt","r");

	if(!file){
		fprintf(stderr, "Impossible lire fichier score\n");
		exit(1);
	} else {
		while(fgets(chaine,40,file) && i<10){ //Recupere les 1à premières lignes du fichier.txt et les affichent centrées
			lastCharDel(chaine);
			TTF_SizeText(police,chaine,&w,&h);
			DrawTextShaded(screen,(WIDTH/2)-(w/2),hauteur,"Polices/LibreBaskerville-Regular.ttf",size,255,255,255,chaine,50,50,50);
			hauteur+=55;
			i++;
		}
	}
	fclose(file);
	TTF_CloseFont(police);
}

void AddScore(char pseudoG[11], char pseudoP[11], int scoreG, int scoreP){ //Ajoute une nouvelle ligne à score.txt
	char c;
	char chaine[40], scoreText[5];

	strcat(chaine,pseudoG); //Fabrication de la string à écrire
	strcat(chaine," VS ");
	strcat(chaine,pseudoP);
	strcat(chaine," : ");
	sprintf(scoreText,"%d",scoreG);
	strcat(chaine,scoreText);
	strcat(chaine,"-");
	sprintf(scoreText,"%d",scoreP);
	strcat(chaine,scoreText);

	FILE* file = fopen("scores.txt","r+");	//score.txt
	FILE* tampon = tmpfile();				//fichier temporaire, on y colle tout le contenu de score.txt, on ajoute notre string dans score.txt, puis on remet tout le fichier temporaire dans score.txt

	if(!file || !tampon){
		fprintf(stderr, "Impossible ecrire fichier score\n");
		exit(1);
	} else {
		while((c=fgetc(file))!=EOF){
			fputc(c,tampon);
		}

		rewind(file);
		rewind(tampon);
		fputs(chaine,file);
		fputs("\n",file);

		while((c=fgetc(tampon))!=EOF){
			fputc(c,file);
		}
	}
	fclose(tampon);
	fclose(file);
}

int Jouer(SDL_Surface* screen, int choix, GameEntry* gameEntry){ //Page Jouer et sa suite (Nombre joueur, Mode de jeu, PseudoJ1, PseudoJ2 et Récapitulatif) - variable choix définie quelle page on souhaite
	int continuer = 1;
	SDL_Event event;
	char letter[2]; //Variable récupérant une lettre + \0 - utilisé dans la formation des pseudos joueur

	Fond(screen); //Fond

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
				if(choix==3 || choix==4){
					LoadPseudoText(screen,choix-2,gameEntry); //L'affichage des pseudos doit être fait à chaque évênement - ne dois pas clignoter
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

void DrawTriangle(SDL_Surface* screen, int x1, int y1, int x2, int y2, int x3, int y3, int red, int green, int blue){
	filledTrigonRGBA(screen,x1,y1,x2,y2,x3,y3,red,green,blue,255);
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

void DrawTextShaded(SDL_Surface* screen, int posx, int posy, char font[255], int size, int red, int green, int blue, char text[255], int bgred, int bggreen, int bgblue){ //Affiche un texte, point haut-gauche de départ - donner la couleur de fond
	TTF_Font *police = NULL;
	SDL_Color policeColor = {red,green,blue};
	SDL_Color bgColor = {bgred,bggreen,bgblue};

	SDL_Surface *texte = NULL;
	SDL_Rect textePosition;

	police = TTF_OpenFont(font,size);
	texte = TTF_RenderUTF8_Shaded(police, text, policeColor, bgColor); //Blended ou Solid ou Shaded

	textePosition.x = posx;
	textePosition.y = posy;

	SDL_BlitSurface(texte, NULL, screen, &textePosition);

	SDL_FreeSurface(texte);
	TTF_CloseFont(police);
}

SDL_Color GetPixelColor(SDL_Surface* screen, int x, int y){ //renvoie une SDL_Color (continuant les valeurs RBG), du pixel aux coordonnées (x,y) - permet de savoir la couleur du pixel sous le curseur
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


//Modele

void init_plateau(){
	int i,j;

	for (i=0;i < 6; i++){
		for(j=0; j < 5; j++){
				plateau[i][j].typeP = VIDE;
				plateau[i][j].coulP = NUL;
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
	if (TOUR==1) plateau[position.l][position.c].coulP = J1.coulj;
		else plateau[position.l][position.c].coulP = J2.coulj;
}

void deplacer_pion(NUMBOX depart, NUMBOX destination){
	if (plateau[depart.l][depart.c].typeP == PION)
	{
		plateau[depart.l][depart.c].typeP = VIDE;
		plateau[depart.l][depart.c].coulP = NUL;
		plateau[destination.l][destination.c].typeP = PION;
		
		if (TOUR==1) plateau[destination.l][destination.c].coulP = J1.coulj;
		else plateau[destination.l][destination.c].coulP = J2.coulj;
	}
}

void retirer_pion(NUMBOX position){
	plateau[position.l][position.c].typeP = VIDE;
}

void init_J1vsJ2(){
	
	J1.reserve=12;
	J2.reserve=12;
	
	J1.coulj=BLANC;
	J2.coulj=NOIR;
	
}
	
int est_clic_reserve(POINT clic){
	
	int bool;
	bool=0;
	
	if ((600<clic.x && clic.x<900 && 200<clic.y && clic.y<250) ||600<clic.x && clic.x<900 && 650<clic.y && clic.y<700){
		bool = 1;
	}
	return bool;
}

int est_clic_plateau(POINT clic){
	
	int bool;
	bool=0;
	
	if (clic.x<550 && 50<clic.x && clic.y<650 && 50<clic.y) bool=1;

	return bool;
}

int est_clic1_valide(POINT clic1, int TOUR){
	int validite;
	validite = 0;

	if(est_clic_reserve(clic1)){
		validite = 1;
	}
	if(est_clic_plateau(clic1)){
		NUMBOX depart;
		depart = point_to_numbox(clic1);

		if(!est_case_vide(depart) && est_case_j(depart)){
			validite = 1;
		}
	}
	return validite;
}

int est_clic2_valide(POINT clic2, int TOUR){
	int validite;
	validite = 0;

	if(est_clic_plateau(clic2)){
		NUMBOX arrivee;
		arrivee = point_to_numbox(clic2);

		if (est_case_vide(arrivee)){
			validite = 1;
		}
	}
}

int est_coup_valide(COUP coup){
	int validite;
	validite = 1;

	printf("check :\n");

	if(!est_case_vide(coup.arrivee)){
		validite = 0;
		printf("0\n");
	}

	if(coup.typeCoup == 0){
		identifier_cases_accessibles(coup.depart);

		if(est_case_vide(coup.depart)){
			validite = 0;
			printf("1\n");
		}

		if(!est_case_j(coup.depart)){
			validite = 0;
			printf("2\n");
		}
		if(!est_case_accessible(coup.arrivee)){
			validite = 0;
			printf("3\n");
		}

		if((TOUR == 1) && (est_coup_inverse(coup,J1.dernierCoup))){
			validite = 0;
			printf("4\n");
		}else if((TOUR == 2) && (est_coup_inverse(coup,J2.dernierCoup))){
			validite = 0;

			printf("5\n");
		}

		printf("Dernier coup J1 :%d c ; %d l to %d c; %d l\n",J1.dernierCoup.depart.c, J1.dernierCoup.depart.l, J1.dernierCoup.arrivee.c, J1.dernierCoup.arrivee.l);
		printf("Dernier coup J2 :%d c ; %d l to %d c; %d l\n",J2.dernierCoup.depart.c, J2.dernierCoup.depart.l, J2.dernierCoup.arrivee.c, J2.dernierCoup.arrivee.l);
		printf("Ce coup : %d c ; %d l to %d c ; %d l\n",coup.depart.c, coup.depart.l, coup.arrivee.c, coup.arrivee.l);


		reinitialiser_cases_accessibles();
	}
	return validite;
}

int est_coup_inverse(COUP coup1, COUP coup2){
	return(
		coup1.depart.c == coup2.arrivee.c &&
		coup1.depart.l == coup2.arrivee.l &&
		coup1.arrivee.l == coup2.depart.l &&
		coup1.arrivee.c == coup2.depart.c &&
		coup1.typeCoup == 0 &&
		coup2.typeCoup == 0);
}

int est_coup_capture(COUP coup){
	return(abs((coup.depart.l+coup.depart.c)-(coup.arrivee.l+coup.arrivee.c)) == 2 && coup.typeCoup == 0);
}

NUMBOX pion_capture(COUP coup){
	NUMBOX pion;
	pion.c = coup.depart.c;
	pion.l = coup.depart.l;

	if(coup.depart.c > coup.arrivee.c){
		pion.c--;
	}else if(coup.depart.l > coup.arrivee.l){
		pion.l--;
	}else if(coup.depart.c < coup.arrivee.c){
		pion.c++;
	}else if(coup.depart.l < coup.arrivee.l){
		pion.l++;
	}

	return pion;
}

void identifier_cases_accessibles(NUMBOX depart){
	NUMBOX curseur;
	curseur.l = depart.l+1;
	curseur.c = depart.c;
	if(explorer_case(curseur)){
		curseur.l = depart.l+2;
		curseur.c = depart.c;
		explorer_case(curseur);
	}

	curseur.l = depart.l-1;
	curseur.c = depart.c;
	if(explorer_case(curseur)){
		curseur.l = depart.l-2;
		curseur.c = depart.c;
		explorer_case(curseur);
	}

	curseur.l = depart.l;
	curseur.c = depart.c+1;
	if(explorer_case(curseur)){
		curseur.l = depart.l;
		curseur.c = depart.c+2;
		explorer_case(curseur);
	}

	curseur.l = depart.l;
	curseur.c = depart.c-1;
	if(explorer_case(curseur)){
		curseur.l = depart.l;
		curseur.c = depart.c-2;
		explorer_case(curseur);
	}

	for (int i = 0; i < 5; i++){
		printf("( ");
		for (int j = 0; j < 6; j++)
		{
			printf("%d ", plateau[j][i].Accessible);
		}
		printf(")\n");
	}
}

void reinitialiser_cases_accessibles(){
	for (int i = 0; i < 5; i++){
		printf("( ");
		for (int j = 0; j < 6; j++)
		{
			plateau[j][i].Accessible = 0;
		}
		printf(")\n");
	}
}

int explorer_case(NUMBOX curseur){
	int continuer;
	continuer = 1; 
	/* le marqueur continuer indique à la fonction identifier_cases_accessibles
	 si elle doit explorer la seconde case dans cette direction,
	 si la première case rencontrée est vide ou un de nos pions, on ne continue pas.*/

	if(est_case_dans_le_plateau(curseur)){
		if (est_case_vide(curseur)){
			plateau[curseur.l][curseur.c].Accessible = 1;
			continuer = 0;
		}else if(est_case_j(curseur)){
			continuer = 0;
		}
	}

	return continuer;
}

int est_case_accessible(NUMBOX destination){
	return (plateau[destination.l][destination.c].Accessible == 1);
}

int est_case_dans_le_plateau(NUMBOX box){
	int dansPlateau;
	dansPlateau = 0;

	if(box.l >= 0 && box.l < 6 && box.c >= 0 && box.c < 5){
		dansPlateau = 1;
	}

	return dansPlateau;
}

void applique_coup(COUP coup){
	if(coup.typeCoup == 0){
		deplacer_pion(coup.depart,coup.arrivee);
		if(est_coup_capture(coup)){
			retirer_pion(pion_capture(coup));
		}
	}else{
		placer_pion(coup.arrivee);
		prendre_reserve(TOUR);
	}
}

void choix_seconde_capture(){
	SDL_Event input;
	POINT clic;
	do{
		SDL_WaitEvent(&input);
		if(input.type == SDL_MOUSEBUTTONDOWN && input.button.button == SDL_BUTTON_LEFT){
			clic.x = input.button.x;
			clic.y = input.button.y;
		}

	}while(!est_clic_plateau(clic) && !est_clic_reserve(clic));

	if(est_clic_plateau(clic)){
		NUMBOX seconde_capture;
		seconde_capture = point_to_numbox(clic);

		if(!est_case_vide(seconde_capture) && !est_case_j(seconde_capture)){
			retirer_pion(seconde_capture);
		}
	}else{
		if(TOUR == 1){
			if(nombre_pions_joueur(J2) == 0){
				prendre_reserve(2);
			}
		}else{
			if(nombre_pions_joueur(J1) == 0){
				prendre_reserve(1);
			}
		}
	}
}

int nombre_pions_joueur(JOUEUR joueur){
	int compte;
	compte = 0;
	int i;
	int j;

	for (int i = 0; i < 5; i++){
		for (int j = 0; j < 6; j++)
		{
			if(plateau[j][i].typeP == PION && plateau[j][i].coulP == joueur.coulj){
				compte++;
			}
		}
	}
	return compte;
}

void prendre_reserve(int joueur){
	
	if(joueur == 1){
		J1.reserve--;
	}else if(joueur == 2){
		J2.reserve--;
	}
}

int est_case_vide(NUMBOX NB)
{
	int bool;
	bool=0;
	
	if (NB.l<6 && NB.c<5){
		if (plateau[NB.l][NB.c].typeP == VIDE) bool=1; 
	}
	
	return bool;
}

int est_case_j(NUMBOX NB)	//vérifie que la case appartient au joueur courant
{
	int bool;
	bool=0;
	
	if (TOUR == 1){
		if (plateau[NB.l][NB.c].coulP == J1.coulj) bool = 1;}
	else{
		if (plateau[NB.l][NB.c].coulP == J2.coulj) bool = 1;}
		
	return bool;
}

void alterne_tour()
{	
	if (TOUR==1) TOUR=2;
	else TOUR=1;
}

//Controlleur

void wait_esc(SDL_Surface* screen){

	int esc;
	esc = 0;

	while(esc == 0){
		SDL_Event event;
		SDL_WaitEvent(&event);
		if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE)
		{
			//Free
			quitter(screen);
			esc = 1;
		}
	}
}

void quitter(SDL_Surface* screen){
	TTF_Quit();
	SDL_FreeSurface(screen);
	SDL_Quit();
}

POINT event_to_point(SDL_Event event){
	POINT clic;

	clic.x = event.button.x;
	clic.y = event.button.y;

	return clic;
}

POINT wait_clic(SDL_Surface* screen){

	int clic;
	POINT P;
	clic = 0;

	while(clic == 0){
		SDL_Event event;
		SDL_WaitEvent(&event);
		if (event.type == SDL_MOUSEBUTTONDOWN)
		{
			P.x=event.button.x;
			P.y=event.button.y;
			clic = 1;
		}
	}
	return P;
}

NUMBOX point_to_numbox(POINT P) // on considère que le point est dans le plateau :  
{	NUMBOX NB;		// => 50 <P.x< 550 et 50 <P.y< 650
	
	NB.c= ((P.x-50)/100); // ici la 1ere case est [0;0] 
	NB.l= ((P.y-50)/100);

	return NB;
}	

POINT numbox_to_point(NUMBOX NB) 
{	POINT P;
	
	P.x=(100*NB.c)+50;
	P.y=(100*NB.l)+50;

	return P;
}	

//Vue

void affiche_pion(SDL_Surface* screen,NUMBOX NB){
	
	POINT HG;
	HG=numbox_to_point(NB);
	
	HG.x=HG.x+10;
	HG.y=HG.y+10;
	
	if (TOUR==1) {DrawRectangle(screen,HG.x,HG.y,80,80,255,0,255);}
	else {DrawRectangle(screen,HG.x,HG.y,80,80,0,255,255);}
}

void efface_pion(SDL_Surface* screen,NUMBOX NB){
	
	POINT HG;
	HG=numbox_to_point(NB);
	
	HG.x=HG.x+1;
	HG.y=HG.y+1;
	
	DrawRectangle(screen,HG.x,HG.y,99,99,50,50,50);
}

void affiche_plateau_debug(){

	int i,j;

	for (i = 0; i < 6; i++)
	{
		printf("(");
		for(j = 0; j < 5; j++){
			printf(" %d ", plateau[i][j].typeP);
			printf(" %d | ", plateau[i][j].coulP);
		}

		printf(")\n");
	}
	printf("\n\n");
}

void affiche_plateau(SDL_Surface* screen){

	int i;
	int hauteur;
	int largeur;


	Fond(screen);

	DrawRectangle(screen,50,50,500,1,255,255,255);
	DrawRectangle(screen,50,650,500,1,255,255,255);
	DrawRectangle(screen,550,50,1,600,255,255,255);
	DrawRectangle(screen,50,50,1,600,255,255,255);


	hauteur = 50;
	largeur = 50;
	for (i = 0; i < 4; i++){
		largeur += 100;
		DrawRectangle(screen,largeur,hauteur,1,600,255,255,255);
	}
	largeur = 50;
	for (i = 0; i < 5; i++){
		hauteur += 100;
		DrawRectangle(screen,largeur,hauteur,500,1,255,255,255);
	}

	DrawRectangle(screen,600,0,1,HEIGHT,255,255,255);
	DrawRectangle(screen,600,50,300,1,255,255,255);
	DrawRectangle(screen,600,250,300,1,255,255,255);
	DrawRectangle(screen,600,450,300,1,255,255,255);
	DrawRectangle(screen,600,500,300,1,255,255,255);



	SDL_Flip(screen);
}

void affichage_info_jeu(SDL_Surface* screen, GameEntry* gameEntry){ //Affichage des pseudos, type de partie
	TTF_Font *police=NULL;
	int w, h;
	char text[15]; //Comme LoadPseudoText, utilise le principe de pré-calcul de la taille occupée pour centrer les textes

	police = TTF_OpenFont("Polices/LibreBaskerville-Regular.ttf",30);

	if(gameEntry->playerNumber==1){
		sprintf(text,"%d joueur",gameEntry->playerNumber);
	} else {
		sprintf(text,"%d joueurs",gameEntry->playerNumber);
	}

	TTF_SizeText(police,text,&w,&h);
	DrawTextShaded(screen,650,290,"Polices/LibreBaskerville-Regular.ttf",40,255,255,255,text,50,50,50); //Affiche nombre de joueurs

	if(gameEntry->gameMode==1){
		strcpy(text,"Partie rapide");
	} else {
		strcpy(text,"Partie longue");
	}

	TTF_SizeText(police,text,&w,&h);
	DrawTextShaded(screen,620,350,"Polices/LibreBaskerville-Regular.ttf",40,255,255,255,text,50,50,50); //Affiche type de partie


	TTF_SizeText(police,gameEntry->pseudoJ1,&w,&h);
	DrawTextShaded(screen,610,8,"Polices/LibreBaskerville-Regular.ttf",30,255,255,255,gameEntry->pseudoJ1,50,50,50);

	TTF_SizeText(police,gameEntry->pseudoJ2,&w,&h);
	DrawTextShaded(screen,610,458,"Polices/LibreBaskerville-Regular.ttf",30,255,255,255,gameEntry->pseudoJ2,50,50,50);


	SDL_Flip(screen);
	TTF_CloseFont(police);
}

void affiche_reserve(SDL_Surface* screen, JOUEUR J1, JOUEUR J2){
	TTF_Font *police=NULL;
	int w, h;
	char text[5];

	police = TTF_OpenFont("Polices/LibreBaskerville-Regular.ttf",30);

	DrawTextShaded(screen,602,202,"Polices/LibreBaskerville-Regular.ttf",20,255,255,255,"Réserve J1 - ",50,50,50);
	
	sprintf(text,"%d ",J1.reserve);
	TTF_SizeText(police,text,&w,&h);
	
	DrawTextShaded(screen,732,202,"Polices/LibreBaskerville-Regular.ttf",20,255,255,255,text,50,50,50);
	
	DrawTextShaded(screen,602,652,"Polices/LibreBaskerville-Regular.ttf",20,255,255,255,"Réserve J2 - ",50,50,50);
	
	sprintf(text,"%d ",J2.reserve);
	TTF_SizeText(police,text,&w,&h);
	DrawTextShaded(screen,732,652,"Polices/LibreBaskerville-Regular.ttf",20,255,255,255,text,50,50,50);

}

void affiche_tour(SDL_Surface* screen){
	
	if(TOUR==1){
		DrawRectangle(screen,850,458,40,40,50,50,50);
		DrawRectangle(screen,850,8,35,35,255,0,255);
	}
	else{
		DrawRectangle(screen,850,8,40,40,50,50,50);
		DrawRectangle(screen,850,458,35,35,0,255,255);
	}
}

void affiche_coup(COUP coup, SDL_Surface* screen){
	if(coup.typeCoup == 1){
		affiche_pion(screen, coup.arrivee);
	}else{
		if(est_coup_capture(coup)){
			efface_pion(screen,pion_capture(coup));
		}
		efface_pion(screen, coup.depart);
		affiche_pion(screen, coup.arrivee);
	}
}