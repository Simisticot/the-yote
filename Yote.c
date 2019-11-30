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
	int gameMode; //1:rapide 2:long
	char pseudoJ1[11];
	char pseudoJ2[11];
} GameEntry;

typedef struct PixelColor { 
	int red;
	int green;
	int blue;
} PixelColor;

GameEntry Menu(SDL_Surface*);
int Credit(SDL_Surface*);
void DrawRectangle(SDL_Surface*,int,int,int,int,int,int,int);
void WindowsNameIcon(SDL_Surface*);
void DrawText(SDL_Surface*,int,int,char[255],int,int,int,int,char[255]);
void LoadMenuText(SDL_Surface*);
SDL_Color GetPixelColor(SDL_Surface*,int,int);

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

	printf("NbrPlayer:%d\n",gameEntry.playerNumber);
	printf("GameMode:%d\n",gameEntry.gameMode);
	printf("PseudoJ1:%s\n",gameEntry.pseudoJ1);
	printf("PseudoJ2:%s\n",gameEntry.pseudoJ2);

	TTF_Quit();
	SDL_FreeSurface(screen);
	SDL_Quit();
	return 0;
}

GameEntry Menu(SDL_Surface* screen){
	GameEntry gameEntryMenu;
	int continuer = 1;
	SDL_Event event;
	SDL_Color pixelColor, pixelColorYote={0,0,0};

	DrawRectangle(screen,0,0,WIDTH,HEIGHT,50,50,50); //Fond

	LoadMenuText(screen);

	SDL_Flip(screen);

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
					// case SDLK_RETURN: //GERER UN RETOUR EN ARRIERE
					// 	continuer = 0;
					// 	break;
					// }
					case SDLK_m:
						LoadMenuText(screen);
						SDL_Flip(screen);
						break;
					default:
						break;
				}
				break;
			case SDL_MOUSEBUTTONDOWN:
				if (event.button.button == SDL_BUTTON_LEFT){
					pixelColor = GetPixelColor(screen,event.button.x,event.button.y);
					if((pixelColor.r==255 && pixelColor.g==255 && pixelColor.b==255) || (pixelColor.r==pixelColorYote.r && pixelColor.g==pixelColorYote.g && pixelColor.b==pixelColorYote.b)){
						if(event.button.y < 160){
							do{
								pixelColorYote.r = rand()%(255-0)+0;
								pixelColorYote.g = rand()%(255-0)+0;
								pixelColorYote.b = rand()%(255-0)+0;
							}while(pixelColorYote.r==50 && pixelColorYote.g==50 && pixelColorYote.b==50);
							DrawText(screen,280,50,"Polices/LibreBaskerville-Regular.ttf",110,pixelColorYote.r,pixelColorYote.g,pixelColorYote.b,"YOTE");
							SDL_Flip(screen);
						} else if(event.button.y < 320){
							//Jouer
						} else if(event.button.y < 380){
							//Score
						} else if(event.button.y < 450){
							//Quitter
							continuer = 0;
						} else {
							if(event.button.x < 450){
								//Règle
							} else {
								continuer = Credit(screen);
								DrawRectangle(screen,0,0,WIDTH,HEIGHT,50,50,50); //Fond
								LoadMenuText(screen);
								SDL_Flip(screen);
							}
						}
					}
				}
				break;
			default:
				break;
		}
	}

	gameEntryMenu.playerNumber = 1;
	gameEntryMenu.gameMode = 2;
	strcpy(gameEntryMenu.pseudoJ1,"Martin\0");
	strcpy(gameEntryMenu.pseudoJ2,"Alphonse\0");

	return gameEntryMenu;
}

int Credit(SDL_Surface* screen){
	int continuer = 1;
	SDL_Event event;
	SDL_Color pixelColor;

	DrawRectangle(screen,0,0,WIDTH,HEIGHT,50,50,50); //Fond
	DrawText(screen,20,10,"Polices/LibreBaskerville-Regular.ttf",50,255,255,255,"Retour");
	SDL_Flip(screen);

	while (continuer){
		SDL_WaitEvent(&event);
		switch(event.type){
			case SDL_QUIT:
				continuer = 0;
				break;
			case SDL_KEYDOWN:
				switch (event.key.keysym.sym){
					case SDLK_ESCAPE:
						return 0;
						break;
					case SDLK_BACKSPACE:
					case SDLK_m:
						continuer = 0;
						break;
					default:
						break;
				}
				break;
			case SDL_MOUSEBUTTONDOWN:
				if (event.button.button == SDL_BUTTON_LEFT){
					if(event.button.y < 60){
						pixelColor = GetPixelColor(screen,event.button.x,event.button.y);
						if(pixelColor.r==255 && pixelColor.g==255 && pixelColor.b==255){
							continuer = 0;
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

void WindowsNameIcon(SDL_Surface* screen){
	SDL_Surface *icon = NULL;

	icon = SDL_LoadBMP("Images/Yote.bmp");
	SDL_WM_SetIcon(icon, NULL);

	SDL_WM_SetCaption("The Yote", NULL);

	SDL_FreeSurface(icon);
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

void LoadMenuText(SDL_Surface* screen){
	DrawText(screen,280,50,"Polices/LibreBaskerville-Regular.ttf",110,255,255,255,"YOTE");
	DrawText(screen,375,250,"Polices/LibreBaskerville-Regular.ttf",50,255,255,255,"Jouer");
	DrawText(screen,365,320,"Polices/LibreBaskerville-Regular.ttf",50,255,255,255,"Scores");
	DrawText(screen,355,385,"Polices/LibreBaskerville-Regular.ttf",50,255,255,255,"Quitter");
	DrawText(screen,10,630,"Polices/LibreBaskerville-Regular.ttf",50,255,255,255,"Règles");
	DrawText(screen,705,637,"Polices/LibreBaskerville-Regular.ttf",50,255,255,255,"Crédits");
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