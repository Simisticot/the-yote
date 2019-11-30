#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include "SDL/SDL.h"

#define WIDTH 900
#define HEIGHT 700

void wait_close();
void draw_rectangle(SDL_Surface*,int,int,int,int,int,int,int);
void window_name_icon(SDL_Surface*);


int main(int argc, char *argv[]){

	SDL_Surface *screen = NULL;

	if(SDL_Init(SDL_INIT_VIDEO) == -1) {
		fprintf(stderr, "SDL_Init error: %s\n", SDL_GetError());
		exit(1);
	}
 
	screen = SDL_SetVideoMode(WIDTH, HEIGHT, 32, SDL_HWSURFACE | SDL_DOUBLEBUF);
	if (screen == NULL){
		fprintf(stderr, "SDL_SetVideoMode error : %s\n", SDL_GetError());
		exit(1);
	}

	window_name_icon(screen);

	draw_rectangle(screen,0,0,WIDTH,HEIGHT,50,50,50); //Fond

	draw_rectangle(screen,300,300,100,100,0,255,0);

	SDL_Flip(screen);

	wait_close();

	SDL_FreeSurface(screen);
	SDL_Quit();
	return 0;
}

void wait_close(){
	int continuer = 1;
	SDL_Event event;
	while (continuer){
		SDL_WaitEvent(&event);
		switch(event.type){
			case SDL_QUIT:
				continuer = 0;
		}
	}
}

void draw_rectangle(SDL_Surface* screen, int posx, int posy, int width, int height, int red, int green, int blue){//posx et posy le point haut-gauche
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

void window_name_icon(SDL_Surface* screen){
	SDL_Surface *icon = NULL;

	icon = SDL_LoadBMP("Images/Yote.bmp");
	SDL_WM_SetIcon(icon, NULL);

	SDL_WM_SetCaption("The Yote", NULL);

	SDL_FreeSurface(icon);
}