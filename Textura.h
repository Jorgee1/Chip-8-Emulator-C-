#ifndef TEXTURA_H
#define TEXTURA_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_image.h>
#include <iostream>
#include <string>
#include <stdio.h>
#include <sstream>
#include "Textura.h"
#include "Init.h"

using namespace std;

extern SDL_Window* Pantalla;
extern SDL_Renderer* RenderP;

class Textura{
    public:
        Textura();

        void Create_Textura(int width, int height);

        void Cargar_Textura(string Path);

        void Cargar_Texto(string Mensaje,SDL_Color Color_Texto, TTF_Font* Font);

        void Render(int x, int y, SDL_Rect *Clip=NULL);

        void Dibujar_rectangulos(int x, int y,int h, int w,int color);

        void Liberar();

        int GetAncho();

        int GetLargo();
    private:
        SDL_Texture* TexturaC;
        int T_Largo,T_Ancho;
};


#endif
