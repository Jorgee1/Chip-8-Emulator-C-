#ifndef INIT_H
#define INIT_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_image.h>
#include <iostream>
#include <string>
#include <stdio.h>
#include <sstream>
#include "Textura.h"
#include "Init.h"
#include "Event.h"
using namespace std;

extern SDL_Window* Pantalla;
extern SDL_Renderer* RenderP;

class Init{

    public:
        Init();
        void Quit();
        int Get_Ancho();
        int Get_Largo();
    private:
        static const int posxP = 500, posyP = 500, AnchoP = 640, LargoP = 480;
};


#endif
