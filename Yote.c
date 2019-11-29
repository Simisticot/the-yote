#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include "SDL/SDL.h"

#define WIDTH 900
#define HEIGHT 700

void wait_close();

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
	SDL_WM_SetIcon(SDL_LoadBMP("Images/Yote.bmp"), NULL);
	SDL_WM_SetCaption("The Yote", NULL);

	Uint32 backgroundColor = SDL_MapRGB(screen->format, 50, 50, 50);
	SDL_FillRect(screen, NULL, backgroundColor);

	SDL_Surface *rectangle = NULL;
	int largeurRectangle = 100;
	SDL_Rect position; position.x=(WIDTH/2)-(largeurRectangle/2); position.y=(HEIGHT/2)-(largeurRectangle/2);
	Uint32 rectangleColor = SDL_MapRGB(screen->format, 255, 255, 255);
	rectangle = SDL_CreateRGBSurface(SDL_HWSURFACE, 100, 100, 32, 0, 0, 0, 0);
	SDL_FillRect(rectangle, NULL, rectangleColor);
	SDL_BlitSurface(rectangle, NULL, screen, &position);

	SDL_Flip(screen);

	wait_close();

	SDL_FreeSurface(rectangle);
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