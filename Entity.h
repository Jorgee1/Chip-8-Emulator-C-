#ifndef ENTITY_H
#define ENTITY_H

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
extern TTF_Font* Font;
extern SDL_Color Negro;

class Entity{
    public:
        Entity(int val1 = 0, int val2 = 0, int height = 0, int width = 0);
        int Get_x();
        int Get_y();
        int Get_height();
        int Get_width();
        SDL_Rect GetEntity_rect();
        void Set_x(int val);
        void Set_y(int val);
        void SetEntity_texture(string Path);
        void CreateEntity_Texture();
        void Render_Entity();
    private:
        int x,y,h,w;
        SDL_Rect Entity_Rect;
        Textura Entity_Image;
};

#endif
