# the-yot-
Jeux de société sénégalais implémenté en c avec SDL.

////////////////////////////////////////

Pour compiler le jeu, il faut installer les bibliothèques suivantes:
  -SDL1.2
  -SDL_image
  -SDL_mixer
  -SDL_ttf
  -SDL_gfxPrimitives

Voici les commandes à effectuer pour installer les bibliothèques:

sudo apt-get install libSLD1.2-devel
sudo apt-get install libsdl1.2-dev
sudo apt-get install libsdl-image1.2
sudo apt-get install libsdl-image1.2-dev
sudo apt-get install libsdl-mixer1.2
sudo apt-get install libsdl-mixer1.2-dev
sudo apt-get install libsdl-ttf2.0-0
sudo apt-get install libsdl-ttf2.0-dev
sudo apt-get install libsdl-gfx1.2-dev
sudo apt-get install libsdl-gfx1.2-doc
sudo apt-get install libsdl-gfx1.2-5

Voici la commande complete d'installation:
sudo apt-get install libSLD1.2-devel libsdl1.2-dev libsdl-image1.2 libsdl-image1.2-dev libsdl-mixer1.2 libsdl-mixer1.2-dev libsdl-ttf2.0-0 libsdl-ttf2.0-dev libsdl-gfx1.2-dev libsdl-gfx1.2-doc libsdl-gfx1.2-5

////////////////////////////////////////

A tout moment du jeu, appuyer sur la touche échap pour quitter.
Le dossier "Polices", "Images", ainsi que les fichiers "scores.txt" et "makefile" doivent être dans le même répertoire que "Yote.c".

Effectuer la commande "make clean" pour nettoyer les fichiers *.o
Effectuer la commande "make" pour compiler
Effectuer la commande "./Yote" pour démarrer le programme

