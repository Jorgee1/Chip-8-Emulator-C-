#include "Event.h"

Event::Event()
{
    velx = 0;
    vely = 0;
}

void Event::Event_Loop(bool* flag,int Key[16]){
    while( SDL_PollEvent(&events) != 0 ){
        if(Quit()){*flag = true;};
        Mover_Punto(Key);
    }
}

bool Event::Quit(){
    if((events.type == SDL_QUIT)||((events.type==SDL_KEYDOWN) && (events.key.keysym.sym == SDLK_ESCAPE))){
        return true;
    }
    return false;
}

void Event::Mover_Punto(int Key[16]){
    if(events.type == SDL_KEYDOWN && events.key.repeat == 0 ){
        switch(events.key.keysym.sym){
                case SDLK_1:    Key[0x1] = 1;break;
                case SDLK_2:    Key[0x2] = 1;break;
                case SDLK_3:    Key[0x3] = 1;break;
                case SDLK_4:    Key[0xC] = 1;break;
                case SDLK_q:    Key[0x4] = 1;break;
                case SDLK_w:    Key[0x5] = 1;break;
                case SDLK_e:    Key[0x6] = 1;break;
                case SDLK_r:    Key[0xD] = 1;break;
                case SDLK_a:    Key[0x7] = 1;break;
                case SDLK_s:    Key[0x8] = 1;break;
                case SDLK_d:    Key[0x9] = 1;break;
                case SDLK_f:    Key[0xE] = 1;break;
                case SDLK_z:    Key[0xA] = 1;break;
                case SDLK_x:    Key[0x0] = 1;break;
                case SDLK_c:    Key[0xB] = 1;break;
                case SDLK_v:    Key[0xF] = 1;break;
                default: break;
        }
    }else if(events.type == SDL_KEYUP && events.key.repeat == 0 ){
        switch(events.key.keysym.sym){
                case SDLK_1:    Key[0x1] = 0;break;
                case SDLK_2:    Key[0x2] = 0;break;
                case SDLK_3:    Key[0x3] = 0;break;
                case SDLK_4:    Key[0xC] = 0;break;
                case SDLK_q:    Key[0x4] = 0;break;
                case SDLK_w:    Key[0x5] = 0;break;
                case SDLK_e:    Key[0x6] = 0;break;
                case SDLK_r:    Key[0xD] = 0;break;
                case SDLK_a:    Key[0x7] = 0;break;
                case SDLK_s:    Key[0x8] = 0;break;
                case SDLK_d:    Key[0x9] = 0;break;
                case SDLK_f:    Key[0xE] = 0;break;
                case SDLK_z:    Key[0xA] = 0;break;
                case SDLK_x:    Key[0x0] = 0;break;
                case SDLK_c:    Key[0xB] = 0;break;
                case SDLK_v:    Key[0xF] = 0;break;
                default: break;
        }
    }
}

int Event::Get_Velx(){
    return velx;
}

int Event::Get_Vely(){
    return vely;
}
