#include "Init.h"

Init::Init()
{
    SDL_Init(SDL_INIT_EVERYTHING);
    TTF_Init();
    IMG_Init(IMG_INIT_PNG);
    Pantalla = SDL_CreateWindow("Test ground",posxP,posyP,AnchoP,LargoP, SDL_WINDOW_SHOWN);
    RenderP = SDL_CreateRenderer(Pantalla,-1,SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    SDL_SetRenderDrawColor(RenderP,0,0,0,0xFF);

}

void Init::Quit(){
    SDL_DestroyWindow(Pantalla);
    SDL_DestroyRenderer(RenderP);
    SDL_Quit();
}

int Init::Get_Ancho(){
    return AnchoP;
}

int Init::Get_Largo(){
    return LargoP;
}


