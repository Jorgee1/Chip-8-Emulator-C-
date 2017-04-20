#ifndef EVENT_H
#define EVENT_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_image.h>
#include "Init.h"
#include "Event.h"


using namespace std;

class Event{
    public:
        Event();
        void Event_Loop(bool* flag, int Key[16]);
        void Mover_Punto(int Key[16]);
        int Get_Velx();
        int Get_Vely();
        bool Quit();
    private:
        SDL_Event events;
        bool flag;
        int velx,vely;
        static const int vel = 1;
};


#endif
