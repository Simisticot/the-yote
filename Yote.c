#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include "SDL/SDL.h"
#include "SDL/SDL_ttf.h"

#define WIDTH 900
#define HEIGHT 700

typedef struct GameEntry { 
	int playerNumber; //1 or 2
	int gameMode; //1:rapide 2:longue
	char pseudoJ1[11];
	char pseudoJ2[11];
} GameEntry;

typedef struct PixelColor { 
	int red;
	int green;
	int blue;
} PixelColor;

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
SDL_Color GetPixelColor(SDL_Surface*,int,int);
char *substr(char*,int,int);
void lastCharDel(char*);

int main(int argc, char *argv[]){

	SDL_Surface *screen = NULL;
	GameEntry gameEntry;

	if(SDL_Init(SDL_INIT_VIDEO) == -1) {
		fprintf(stderr, "SDL_Init error: %s\n", SDL_GetError());
		exit(1);
	}

	screen = SDL_SetVideoMode(WIDTH, HEIGHT, 32, SDL_HWSURFACE | SDL_DOUBLEBUF);
	if (screen == NULL){
		fprintf(stderr, "SDL_SetVideoMode error : %s\n", SDL_GetError());
		exit(1);
	}

	if(TTF_Init() == -1){
		fprintf(stderr, "TTF_Init error : %s\n", TTF_GetError());
		exit(1);
	}

	WindowsNameIcon(screen);

	gameEntry = Menu(screen);

	printf("Nbrlayer:%d\n",gameEntry.playerNumber);
	printf("GameMode:%d\n",gameEntry.gameMode);
	printf("PseudoJ1:%s\n",gameEntry.pseudoJ1);
	printf("PseudoJ2:%s\n",gameEntry.pseudoJ2);

	TTF_Quit();
	SDL_FreeSurface(screen);
	SDL_Quit();
	return 0;
}

void WindowsNameIcon(SDL_Surface* screen){
	SDL_Surface *icon = NULL;

	icon = SDL_LoadBMP("Images/Yote.bmp");
	SDL_WM_SetIcon(icon, NULL);

	SDL_WM_SetCaption("The Yote", NULL);

	SDL_FreeSurface(icon);
}

GameEntry Menu(SDL_Surface* screen){
	GameEntry gameEntryMenu;
	int continuer = 1, fini=0;
	SDL_Event event;

	gameEntryMenu.pseudoJ1[0]='\0';
	gameEntryMenu.pseudoJ2[0]='\0';

	DrawRectangle(screen,0,0,WIDTH,HEIGHT,50,50,50); //Fond

	LoadMenuText(screen);
	SDL_Flip(screen);

	SDL_EnableKeyRepeat(10,150);

	while (continuer){
		SDL_WaitEvent(&event);
		switch(event.type){
			case SDL_QUIT:
				continuer = 0;
				break;
			case SDL_KEYDOWN:
				switch (event.key.keysym.sym){
					case SDLK_ESCAPE:
						continuer = 0;
						break;
					default:
						break;
				}
				break;
			case SDL_MOUSEBUTTONDOWN:
				if (event.button.button==SDL_BUTTON_LEFT){
					if(event.button.x>275 && event.button.x<630 && event.button.y>65 && event.button.y<160){
						SpecialYote(screen);
					} else if(event.button.x>370 && event.button.x<520 && event.button.y>255 && event.button.y<315){
						do{
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
									if(gameEntryMenu.playerNumber==2){
										continuer += Jouer(screen,4,&gameEntryMenu);
									} else {
										continuer=5;
									}
									break;
								case 5:
									continuer += Jouer(screen,5,&gameEntryMenu);
									if(gameEntryMenu.playerNumber==1 && continuer==4){
										continuer=3;
									}
									break;
								default:
									break;
							}
						}while(continuer>0 && continuer<6);
						if(continuer>=-4 && continuer<=0){
							DrawRectangle(screen,0,0,WIDTH,HEIGHT,50,50,50);
							LoadMenuText(screen);
							SDL_Flip(screen);
							continuer=1;
						} else if(continuer<=-5){
							continuer=0;
						} else {
							fini=1;
							continuer=0;
						}
					} else if(event.button.x>365 && event.button.x<530 && event.button.y>325 && event.button.y<370){
						continuer = ScoreRegleCredit(screen,1);
					} else if(event.button.x>355 && event.button.x<540 && event.button.y>395 && event.button.y<445){
						continuer = 0;
					} else if(event.button.y>635){
						if(event.button.x<175){
							continuer = ScoreRegleCredit(screen,2);
						} else if(event.button.x>705){
							continuer = ScoreRegleCredit(screen,3);
						}
					}
				}
				break;
			default:
				break;
		}
	}

	// if(gameEntryMenu.playerNumber==1){
	// 	strcpy(gameEntryMenu.pseudoJ2,"CyberYote\0");
	// }

	if(fini==1){
		return gameEntryMenu;
	} else {
		gameEntryMenu.playerNumber = 0;
		gameEntryMenu.gameMode = 0;
		strcpy(gameEntryMenu.pseudoJ1,"\0");
		strcpy(gameEntryMenu.pseudoJ2,"\0");
		return gameEntryMenu;
	}
}

void LoadMenuText(SDL_Surface* screen){
	DrawText(screen,280,50,"Polices/LibreBaskerville-Regular.ttf",110,255,255,255,"YOTE");
	DrawText(screen,375,250,"Polices/LibreBaskerville-Regular.ttf",50,255,255,255,"Jouer");
	DrawText(screen,365,320,"Polices/LibreBaskerville-Regular.ttf",50,255,255,255,"Scores");
	DrawText(screen,355,385,"Polices/LibreBaskerville-Regular.ttf",50,255,255,255,"Quitter");
	DrawText(screen,10,630,"Polices/LibreBaskerville-Regular.ttf",50,255,255,255,"Règles");
	DrawText(screen,705,637,"Polices/LibreBaskerville-Regular.ttf",50,255,255,255,"Crédits");
}

void SpecialYote(SDL_Surface* screen){
	SDL_Color pixelColorYote;
	do{
		pixelColorYote.r = rand()%(255-0)+0;
		pixelColorYote.g = rand()%(255-0)+0;
		pixelColorYote.b = rand()%(255-0)+0;
	}while(pixelColorYote.r==50 && pixelColorYote.g==50 && pixelColorYote.b==50);
	DrawText(screen,280,50,"Polices/LibreBaskerville-Regular.ttf",110,pixelColorYote.r,pixelColorYote.g,pixelColorYote.b,"YOTE");
	SDL_Flip(screen);
}

int ScoreRegleCredit(SDL_Surface* screen, int choix){
	int continuer = 1;
	SDL_Event event;

	DrawRectangle(screen,0,0,WIDTH,HEIGHT,50,50,50); //Fond
	switch(choix){
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
	DrawText(screen,20,10,"Polices/LibreBaskerville-Regular.ttf",50,255,255,255,"Retour");
	SDL_Flip(screen);

	while (continuer){
		SDL_WaitEvent(&event);
		switch(event.type){
			case SDL_QUIT:
				return 0;
			case SDL_KEYDOWN:
				switch (event.key.keysym.sym){
					case SDLK_ESCAPE:
						return 0;
					default:
						break;
				}
				break;
			case SDL_MOUSEBUTTONDOWN:
				if (event.button.button == SDL_BUTTON_LEFT){
					if(event.button.x<200 && event.button.y<60){
							continuer = 0;
					}
				}
				break;
			default:
				break;
		}
	}

	DrawRectangle(screen,0,0,WIDTH,HEIGHT,50,50,50);
	LoadMenuText(screen);
	SDL_Flip(screen);
	return 1;
}

void LoadCreditText(SDL_Surface* screen){
	DrawText(screen,180,130,"Polices/LibreBaskerville-Regular.ttf",22,255,255,255,"Ce jeu a été réalisé pour le projet algorithmique");
	DrawText(screen,65,160,"Polices/LibreBaskerville-Regular.ttf",22,255,255,255,"de première année d'école d'ingénieurs ISTY de Vélizy-Villacoublay.");
	DrawText(screen,205,190,"Polices/LibreBaskerville-Regular.ttf",22,255,255,255,"Ce projet a été dirigé par ABOUDA Dhekra");
	DrawText(screen,135,280,"Polices/LibreBaskerville-Regular.ttf",22,255,255,255,"Le programme a été pensé, modélisé et programmé par");
	DrawText(screen,360,330,"Polices/LibreBaskerville-Regular.ttf",25,255,255,255,"Julie GASPAR");
	DrawText(screen,340,370,"Polices/LibreBaskerville-Regular.ttf",25,255,255,255,"Rémy LEBAILLY");
	DrawText(screen,280,410,"Polices/LibreBaskerville-Regular.ttf",25,255,255,255,"Laureline PARTONNAUD");
	DrawText(screen,345,450,"Polices/LibreBaskerville-Regular.ttf",25,255,255,255,"Simon PEREIRA");
	DrawText(screen,325,490,"Polices/LibreBaskerville-Regular.ttf",25,255,255,255,"Yannick STACHIVI");
	DrawText(screen,180,600,"Polices/LibreBaskerville-Regular.ttf",22,255,255,255,"Nous vous remercions d'avoir joué au jeu YOTE");
}

void LoadReglesText(SDL_Surface* screen){

}
void LoadScoreText(SDL_Surface* screen){

}

int Jouer(SDL_Surface* screen, int choix, GameEntry* gameEntry){
	int continuer = 1;
	SDL_Event event;
	char letter[2];

	DrawRectangle(screen,0,0,WIDTH,HEIGHT,50,50,50); //Fond

	switch(choix){
		case 1:
			LoadPlayerText(screen);
			break;
		case 2:
			LoadModeText(screen);
			break;
		case 3:
		case 4:
			LoadPseudoText(screen,choix-2,gameEntry);
			break;
		case 5:
			LoadRecapText(screen,gameEntry);
		default:
			break;
	}
	DrawText(screen,20,10,"Polices/LibreBaskerville-Regular.ttf",50,255,255,255,"Retour");
	SDL_Flip(screen);

	while (continuer){
		if(choix==3 || choix==4){
			LoadPseudoText(screen,choix-2,gameEntry);
		}
		SDL_WaitEvent(&event);
		switch(event.type){
			case SDL_QUIT:
				return -10;
			case SDL_KEYDOWN:
				switch (event.key.keysym.sym){
					case SDLK_ESCAPE:
						return -10;
					default:
						break;
				}
				if(choix==3){
					letter[0] = GetLetter(event.key.keysym.sym);
					letter[1] = '\0';
					if(letter[0]=='R'){
						if(strlen(gameEntry->pseudoJ1)==0){
							strcat(gameEntry->pseudoJ1,"Joueur1\0");
						}
						continuer=0;
					} else if(letter[0]=='B'){
						lastCharDel(gameEntry->pseudoJ1);
					} else if(letter[0]!=' ' && strlen(gameEntry->pseudoJ1)<10){
						strcat(gameEntry->pseudoJ1,letter);
						strcat(gameEntry->pseudoJ2,"\0");
					}
				}
				if(choix==4){
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
					if(event.button.x<200 && event.button.y<60){
						return -1;
					} else {
						switch(choix){
							case 1:
							case 2:
								if(event.button.x>55 && event.button.x<825 && event.button.y>260 && event.button.y<335){
									switch(choix){
										case 1:
											gameEntry->playerNumber = 2;
											strcpy(gameEntry->pseudoJ2,"\0");
											break;
										case 2:
											gameEntry->gameMode = 2;
											break;
										default:
											break;
									}
									continuer = 0;
								} else if(event.button.x>115 && event.button.x<775 && event.button.y>410 && event.button.y<485){
									switch(choix){
										case 1:
											gameEntry->playerNumber = 1;
											strcpy(gameEntry->pseudoJ2,"CyberYote\0");
											break;
										case 2:
											gameEntry->gameMode = 1;
											break;
										default:
											break;
									}
									continuer = 0;
								}
								break;
							case 5:
								if(event.button.x>710 && event.button.y>590){
									continuer = 0;
								}
								break;
							default:
								break;
						}
					}
				}
				break;
			default:
				break;
		}
	}
	return 1;
}

void LoadPlayerText(SDL_Surface* screen){
	DrawText(screen,55,250,"Polices/LibreBaskerville-Regular.ttf",70,255,255,255,"JOUEUR vs JOUEUR");
	DrawText(screen,120,400,"Polices/LibreBaskerville-Regular.ttf",70,255,255,255,"JOUEUR vs ORDI");
}

void LoadModeText(SDL_Surface* screen){
	DrawText(screen,185,250,"Polices/LibreBaskerville-Regular.ttf",70,255,255,255,"partie RAPIDE");
	DrawText(screen,155,400,"Polices/LibreBaskerville-Regular.ttf",70,255,255,255,"partie LONGUE");
}

void LoadPseudoText(SDL_Surface* screen, int joueur, GameEntry* gameEntry){
	char text[14];
	TTF_Font *police = NULL;
	int w, h, size=80;

	police = TTF_OpenFont("Polices/LibreBaskerville-Regular.ttf",size);

	snprintf(text,14,"input name J%d",joueur);
	DrawText(screen,205,180,"Polices/LibreBaskerville-Regular.ttf",70,255,255,255,text);

	DrawRectangle(screen,0,350,WIDTH,size*2,50,50,50);
	SDL_Flip(screen);
	if(joueur==1){
		TTF_SizeText(police,gameEntry->pseudoJ1,&w,&h);
		DrawText(screen,(WIDTH/2)-(w/2),350,"Polices/LibreBaskerville-Regular.ttf",size,255,255,255,gameEntry->pseudoJ1);
	} else {
		TTF_SizeText(police,gameEntry->pseudoJ2,&w,&h);
		DrawText(screen,(WIDTH/2)-(w/2),350,"Polices/LibreBaskerville-Regular.ttf",size,255,255,255,gameEntry->pseudoJ2);
	}
	SDL_Flip(screen);
	TTF_CloseFont(police);
}

char GetLetter(int eventKey){
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

void LoadRecapText(SDL_Surface* screen, GameEntry* gameEntry){
	TTF_Font *police=NULL;
	int w, h, size=80;
	char text[15];

	police = TTF_OpenFont("Polices/LibreBaskerville-Regular.ttf",size);

	if(gameEntry->playerNumber==1){
		sprintf(text,"%d joueur",gameEntry->playerNumber);
	} else {
		sprintf(text,"%d joueurs",gameEntry->playerNumber);
	}

	TTF_SizeText(police,text,&w,&h);
	DrawText(screen,(WIDTH/2)-(w/2),80,"Polices/LibreBaskerville-Regular.ttf",size,255,255,255,text);

	if(gameEntry->gameMode==1){
		strcpy(text,"Partie rapide");
	} else {
		strcpy(text,"Partie longue");
	}

	TTF_SizeText(police,text,&w,&h);
	DrawText(screen,(WIDTH/2)-(w/2),180,"Polices/LibreBaskerville-Regular.ttf",size,255,255,255,text);

	TTF_SizeText(police,gameEntry->pseudoJ1,&w,&h);
	DrawText(screen,(WIDTH/2)-(w/2),310,"Polices/LibreBaskerville-Regular.ttf",size,255,255,255,gameEntry->pseudoJ1);

	DrawText(screen,395,410,"Polices/LibreBaskerville-Regular.ttf",size,255,255,255,"VS");

	TTF_SizeText(police,gameEntry->pseudoJ2,&w,&h);
	DrawText(screen,(WIDTH/2)-(w/2),500,"Polices/LibreBaskerville-Regular.ttf",size,255,255,255,gameEntry->pseudoJ2);

	DrawText(screen,710,590,"Polices/LibreBaskerville-Regular.ttf",100,255,255,255,"GO");
	SDL_Flip(screen);
	TTF_CloseFont(police);
}

void DrawRectangle(SDL_Surface* screen, int posx, int posy, int width, int height, int red, int green, int blue){//posx et posy le point haut-gauche
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

void DrawText(SDL_Surface* screen, int posx, int posy, char font[255], int size, int red, int green, int blue, char text[255]){//posx et posy le point haut-gauche
	TTF_Font *police = NULL;
	SDL_Color policeColor = {red,green,blue};

	SDL_Surface *texte = NULL;
	SDL_Rect textePosition;

	police = TTF_OpenFont(font,size);
	texte = TTF_RenderUTF8_Blended(police, text, policeColor);

	textePosition.x = posx;
	textePosition.y = posy;

	SDL_BlitSurface(texte, NULL, screen, &textePosition);

	SDL_FreeSurface(texte);
	TTF_CloseFont(police);
}

SDL_Color GetPixelColor(SDL_Surface* screen, int x, int y){
	SDL_Color pixelColor;
	Uint32* allPixels;
	Uint32 pixel;

	allPixels = (Uint32 *)screen->pixels;
	pixel = allPixels[(y * screen->w) + x];
	SDL_GetRGB(pixel, screen->format, &pixelColor.r, &pixelColor.g, &pixelColor.b);

	return pixelColor;
}

void lastCharDel(char* str){
	int len;
	len = strlen(str);
	str[len-1] = '\0';
}