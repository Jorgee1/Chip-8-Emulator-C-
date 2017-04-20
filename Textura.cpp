#include "Textura.h"

Textura::Textura(){
    T_Ancho = 0,T_Largo = 0;
    TexturaC=NULL;
}


void Textura::Create_Textura(int width, int height){
    Liberar();

    TexturaC = SDL_CreateTexture(RenderP,  SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, width, height);
    T_Largo = height;
    T_Ancho = width;
    SDL_SetRenderTarget(RenderP, TexturaC);

    SDL_SetRenderDrawColor(RenderP,0xFF,0xFF,0xFF,0xFF);
    SDL_RenderClear(RenderP);
    SDL_RenderPresent(RenderP);

    SDL_SetRenderTarget(RenderP, NULL);
}

void Textura::Cargar_Textura(string Path){
    Liberar();
    SDL_Surface* Superficie = IMG_Load(Path.c_str());
    T_Ancho = Superficie->w;
    T_Largo = Superficie->h;
    SDL_SetColorKey(Superficie,SDL_TRUE,SDL_MapRGB(Superficie->format,0,0xFF,0xFF));
    TexturaC=SDL_CreateTextureFromSurface(RenderP,Superficie);
    SDL_FreeSurface(Superficie);
}

void Textura::Cargar_Texto(string Mensaje,SDL_Color Color_Texto, TTF_Font* Font){
    Liberar();
    int h,w;
    SDL_Surface* Texto_Superficie = TTF_RenderText_Solid(Font,Mensaje.c_str(),Color_Texto);
    TexturaC = SDL_CreateTextureFromSurface(RenderP,Texto_Superficie);
    SDL_QueryTexture(TexturaC,NULL,NULL,&w,&h);
    T_Largo = h;
    T_Ancho = w;
    SDL_FreeSurface(Texto_Superficie);
}

void Textura::Render(int x, int y, SDL_Rect *Clip){
    SDL_Rect Posicion = {x,y,T_Ancho,T_Largo};
    if(Clip != NULL){
        Posicion.h = Clip->h;
        Posicion.w = Clip->w;
    }
    SDL_RenderCopy(RenderP,TexturaC,Clip,&Posicion);
}

void Textura::Liberar(){
    if(TexturaC != NULL){
        SDL_DestroyTexture(TexturaC);
        TexturaC = NULL;
        T_Largo = 0;
        T_Ancho = 0;
    }
}

void Textura::Dibujar_rectangulos(int x, int y,int h, int w,int color ){
    SDL_Rect cuadrado = {x,y,h,w};
    SDL_SetRenderDrawColor(RenderP,0xFF,0xFF,0xFF,0xFF);
    if(color == 1){
        SDL_RenderDrawRect(RenderP,&cuadrado);
    }else if(color == -1){
       SDL_RenderFillRect(RenderP,&cuadrado);
    }
}

int Textura::GetAncho(){
    return T_Ancho;
}

int Textura::GetLargo(){
    return T_Largo;
}
