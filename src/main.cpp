#include <SDL.h>
#include <SDL_ttf.h>
#include <SDL_image.h>

#include <string>
#include <bitset>
#include <stdint.h>
#include <iostream>

#include "window.h"
#include "texture.h"
#include "action.h"

std::string to_format_str(char format[], int value){
    char hex[8];
    sprintf(hex, format, value);
    return std::string(hex);
}

class Chip8{
    public:
        int pc = 0x200;
        int op = 0;
        int I  = 0;
        int sp = 0;
        int dt = 0;
        int st = 0;
        int stack[16];
        int key[16];
        const int h = 32;
        const int w = 64;

        unsigned char memory[4096];
        unsigned char V[16];
        unsigned char font[80] = {
            0xF0, 0x90, 0x90, 0x90, 0xF0, //0
            0x20, 0x60, 0x20, 0x20, 0x70, //1
            0xF0, 0x10, 0xF0, 0x80, 0xF0, //2
            0xF0, 0x10, 0xF0, 0x10, 0xF0, //3
            0x90, 0x90, 0xF0, 0x10, 0x10, //4
            0xF0, 0x80, 0xF0, 0x10, 0xF0, //5
            0xF0, 0x80, 0xF0, 0x90, 0xF0, //6
            0xF0, 0x10, 0x20, 0x40, 0x40, //7
            0xF0, 0x90, 0xF0, 0x90, 0xF0, //8
            0xF0, 0x90, 0xF0, 0x10, 0xF0, //9
            0xF0, 0x90, 0xF0, 0x90, 0x90, //A
            0xE0, 0x90, 0xE0, 0x90, 0xE0, //B
            0xF0, 0x80, 0x80, 0x80, 0xF0, //C
            0xE0, 0x90, 0x90, 0x90, 0xE0, //D
            0xF0, 0x80, 0xF0, 0x80, 0xF0, //E
            0xF0, 0x80, 0xF0, 0x80, 0x80  //F
        };
        unsigned long screen[h]; // 8 bytes

        bool step = false;
        
        void init(){
            for(int y=0;y<h;y++){
                screen[y] = 0;
            }

            for(int i=0;i<16;i++){
                V[i]      = (char) 0;
            }
            
            for(int i=0;i<16;i++){
                stack[i]  = (char) 0;
            }
            
            for(int i=0;i<4096;i++){
                memory[i] = (char) 0;
            }
            
            for(int i=0;i<80;i++){
                memory[i] = font[i];
            }
        }
        
        bool load_rom(char* path){
            FILE* rom = fopen(path, "rb");
            if(rom){
                fseek(rom, 0, SEEK_END);
                int buffersize = ftell(rom);
                rewind(rom);
                char* buffer = (char*) malloc(sizeof(char)*buffersize);
                fread(buffer,1,buffersize,rom);
                for(int i=0;i<buffersize;i++){
                    memory[pc+i] = buffer[i];
                }
                free(buffer);
                fclose(rom);
                return true;
            }else{
                printf("Rom not found\n");
                return false;
            }        
        }
        
        
        void cycle(){
            op = memory[pc] << 8 | memory[pc+1];
            switch(op&0xF000){

                case 0x0000:{
                    switch(op&0x0FFF){
                        case 0x00E0:
                            for(int y = 0;y < h;y++){
                                screen[y] = 0;
                            }
                           
                            pc+=2;
                            break;
                        case 0x00EE:
                            sp--;
                            pc = stack[sp];
                            stack[sp] = 0x0000;
                            pc+=2;
                            break;
                    }
                    break;
                }

                case 0xA000:{
                    INDEX = op&0x0FFF;
                    pc+=2;
                    break;
                }

                case 0xB000:{
                    pc = V[0] + (op&0x0FFF);
                    break;
                }

                case 0xC000:{
                    int x = (rand () % 255);
                    V[(op & 0x0F00) >> 8] = x & (op & 0x00FF);
                    pc+=2;
                    break;
                }

                case 0xD000:{

                    unsigned int x = V[(op & 0x0F00)>>8];
                    unsigned int y = V[(op & 0x00F0)>>4];
                    unsigned int n = (op & 0x000F);

                    V[0xF] = 0x0;
                    
                    unsigned long temp_char = 0ULL;                     
                    int fase = w-8-x;

                    
                    for(int i=0; i<n; i++){
                    
                        if (y>=h){
                            y = y - h*(y/(h-1));
                        }
                    
	                    temp_char = memory[I+i];

	                    if (fase>=0){
		                    screen[y+i] ^=(temp_char<<fase);
	                    }else{
	                        screen[y+i] ^=(temp_char>>fase*-1);
	                    }

                    }
                    pc+=2;
                    break;
                }
                case 0xE000:{
                    switch(op&0x00FF){
                        case 0x00A1:
                            if(key[V[(op&0x0F00)>>8]]==1){
                                pc+=2;
                            }
                            break;
                        case 0x009E:
                            if(key[V[(op&0x0F00)>>8]]!=1){
                               pc+=2;
                            }
                            break;
                    }
                    pc+=2;
                    break;
                }

                case 0xF000:{
                    switch(op&0x00FF){
                        case 0x0007: {
                            V[(op&0x0F00)>>8] = dt;
                            break;
                        }
                        case 0x000A: {
                            // Wait for key press
                            break;
                        }
                        case 0x0015: {
                            dt = V[(op&0x0F00)>>8];
                            break;
                        }
                        case 0x0018: {
                            st = V[(op&0x0F00)>>8];
                            break;
                        }
                        case 0x001E:{
                            I += V[(op&0x0F00)>>8];
                            break;
                        }
                        case 0x0029:{
                            I = V[(op&0x0F00)>>8]*0x5;
                            break;
                        }
                        case 0x0033:{
                            int x = (op&0x0F00)>>8;
                            
                            int TEN = V[x]/100;
                            int HUN = (V[x]/10) - TEN*10;
                            int DEC = V[x] - HUN*10 - TEN*100;
                        
                            Memory[ I ] = TEN;
                            Memory[I+1] = HUN;
                            Memory[I+2] = DEC;
                            break;
                        }
                        case 0x0055:{
                            for(int i=0;i<=((op&0x0F00)>>8);i++){
                                memory[I+i] = V[i];
                            }
                            break;
                        }
                        case 0x0065:{
                            int x = (op&0x0F00)>>8;
                            for(int i=0;i<=x;i++){
                                V[i] = memory[I+i];
                            }
                            break;
                        }

                    }
                    pc+=2;
                    break;
                }

                case 0x1000:{
                    pc = (op&0x0FFF);
                    break;
                }

                case 0x2000:{
                    stack[sp] = pc;
                    sp++;
                    pc = op&0x0FFF;
                    break;
                }

                case 0x3000:{
                    if( V[(op&0x0F00)>>8] == (op&0x00FF) ){
                        pc+=2;
                    }
                    pc+=2;
                    break;
                }

                case 0x4000:{
                    if( V[(op&0x0F00)>>8] != (op&0x00FF)){
                        pc+=2;
                    }
                    pc+=2;
                    break;
                }

                case 0x5000:{
                    if(V[(op&0x0F00)>>8]==V[(op&0x00F0)>>4]){
                        pc+=2;
                    }
                    pc+=2;
                    break;
                }

                case 0x6000:{
                    V[(op&0x0F00)>>8] = (op&0x00FF);
                    pc+=2;
                    break;
                }

                case 0x7000:{
                    V[(op&0x0F00)>>8] += (op&0x00FF);
                    V[(op&0x0F00)>>8] &= 0xFF;
                    pc+=2;
                    break;
                }
                /*
                    ALU
                */
                case 0x8000:{
                    switch(op&0x000F){

                        case 0x0000:{
                            V[(op&0x0F00)>>8] = V[(op&0x00F0)>>4];
                            break;
                        }

                        case 0x0001:{
                            V[(op&0x0F00)>>8] |= V[(op&0x00F0)>>4];
                            break;
                        }

                        case 0x0002:{
                            V[(op&0x0F00)>>8] &= V[(op&0x00F0)>>4];
                            break;
                        }

                        case 0x0003:{
                            V[(op&0x0F00)>>8] ^= V[(op&0x00F0)>>4];
                            break;
                        }

                        case 0x0004:{
                            int x = V[(op&0x0F00)>>8]&0xFF;
                            int y = V[(op&0x00F0)>>4]&0xFF;
                            int z = 0;
                            if(x+y > 0xFF){
                                V[0xF] = 0x01;
                            }else{
                                V[0xF] = 0x00;
                            }
                            z = (x + y)&0xFF;
                            V[(op&0x0F00)>>8] = z;
                            break;
                        }

                        case 0x0005:{
                            int x = V[(op&0x0F00)>>8]&0xFF;
                            int y = V[(op&0x00F0)>>4]&0xFF;
                            if(x > y){
                                V[0xF] = 0x01;
                            }else{
                                V[0xF] = 0x00;
                            }
                            V[(op&0x0F00)>>8] -= V[(op&0x00F0)>>4];
                            V[(op&0x0F00)>>8] &= 0xFF;
                            break;
                        }

                        case 0x0006:{
                            V[0xF] = V[(op&0x0F00)>>8]&0x01)
                            V[(op&0x0F00)>>8] = V[(op&0x0F00)>>8] >> 1;
                            break;
                        }

                        case 0x0007:{
                            int x = V[(op&0x00F0)>>4]&0xFF;
                            int y = V[(op&0x0F00)>>8]&0xFF;
                            if(y > x){
                                V[0xF] = 0x01;
                            }else{
                                V[0xF] = 0x00;
                            }
                            V[(op&0x0F00)>>8] -= V[(op&0x0F00)>>4];
                            V[(op&0x0F00)>>8] &= 0xFF;
                            break;
                        }

                        case 0x000E:{
                            V[0xF] = (V[(op&0x0F00)>>8]&0x80)>>7;
                            V[(op&0x0F00)>>8] = (V[(op&0x0F00)>>8] << 1)&0xFF;
                            break;
                        }

                    }
                    pc+=2;
                    break;
                }
                case 0x9000:{
                    if(V[(op&0x0F00)>>8] != V[(op&0x00F0)>>4]){
                        pc+=2;
                    }
                    pc+=2;
                    break;
                }

            }
            if(dt > 0){
                dt--;
            }
            if(st > 0){
                printf("Beep\n");
                st--;
            }
        }

};

int main( int argc, char* args[] ){
    int SCREEN_WIDTH  = 640;
    int SCREEN_HEIGHT = 600;
    int TEXT_SIZE     =  15;
    int upscale       =   5;
    const int emuH    =  32;
    const int emuW    =  64;
    int AnchoP        = emuW*upscale;
    int LargoP        = emuH*upscale;
    int scrollIndex   =   0x2E7;    
    bool exit = false;
    bool step = false;
    
    std::string NAME = "Chip 8";
    
    /*
        CHIP 8 INIT
    */
    
    unsigned char Memory[4096];
    unsigned char V[16];
    int Stack[16];
    int Key[16];
    unsigned long Chip8_pantalla[emuH]; // 8 bytes
    
    unsigned char chip8_fontset[80] = {
        0xF0, 0x90, 0x90, 0x90, 0xF0, //0
        0x20, 0x60, 0x20, 0x20, 0x70, //1
        0xF0, 0x10, 0xF0, 0x80, 0xF0, //2
        0xF0, 0x10, 0xF0, 0x10, 0xF0, //3
        0x90, 0x90, 0xF0, 0x10, 0x10, //4
        0xF0, 0x80, 0xF0, 0x10, 0xF0, //5
        0xF0, 0x80, 0xF0, 0x90, 0xF0, //6
        0xF0, 0x10, 0x20, 0x40, 0x40, //7
        0xF0, 0x90, 0xF0, 0x90, 0xF0, //8
        0xF0, 0x90, 0xF0, 0x10, 0xF0, //9
        0xF0, 0x90, 0xF0, 0x90, 0x90, //A
        0xE0, 0x90, 0xE0, 0x90, 0xE0, //B
        0xF0, 0x80, 0x80, 0x80, 0xF0, //C
        0xE0, 0x90, 0x90, 0x90, 0xE0, //D
        0xF0, 0x80, 0xF0, 0x80, 0xF0, //E
        0xF0, 0x80, 0xF0, 0x80, 0x80  //F
    };


    int PC          = 0x200;
    int opcode      = 0;
    int INDEX       = 0;
    int StackP      = 0;
    int delay_timer = 0;
    int sound_timer = 0;

    /*
        LOAD MEMORY
    */

    for(int y = 0;y < emuH;y++){
        Chip8_pantalla[y] = 0ULL;
    }

    for(int i=0; i<16;i++){
        V[i]      = (char) 0;
    }
    
    for(int i=0; i<16;i++){
        Stack[i]  = (char) 0;
    }
    
    for(int i=0; i<4096;i++){
        Memory[i] = (char) 0;
    }
    
    for(int i=0;i<80;i++){
        Memory[i] = chip8_fontset[i];
    }

    if(argc>1){
        FILE* rom = fopen(args[1],"rb");
        if(rom){
            fseek(rom, 0, SEEK_END);
            int buffersize = ftell(rom);
            rewind(rom);
            char* buffer = (char*) malloc(sizeof(char)*buffersize);
            fread(buffer,1,buffersize,rom);
            for(int i=0;i<buffersize;i++){
                Memory[i+PC] = buffer[i];
            }
            free(buffer);
            fclose(rom);
        }else{
            printf("Rom not found\n");
            exit = true;
        }
    }else{
        printf("No rom specified\n");
        exit = true;
    }


    /*
        SDL INIT
    */
    

    char * PATH_P = SDL_GetBasePath();
    std::string PATH(PATH_P);
    std::string PATH_FONT = PATH + "asset/font/LiberationMono-Regular.ttf";
    std::string PATH_ICON = PATH + "asset/icon.bmp";
    
    SDL_free(PATH_P);
    SDL_Color COLOR_BLACK = {0x00, 0x00, 0x00, 0xFF};
    SDL_Color COLOR_RED   = {0xFF, 0x00, 0x00, 0xFF};
    SDL_Color COLOR_GREEN = {0x00, 0xFF, 0x00, 0xFF};
    SDL_Color COLOR_BLUE  = {0x00, 0x00, 0xFF, 0xFF};
    SDL_Color COLOR_WHITE = {0xFF, 0xFF, 0xFF, 0xFF};
    
    Window window(
        NAME.c_str(),
        SCREEN_WIDTH,
        SCREEN_HEIGHT,
        COLOR_RED
    );
    window.set_icon(PATH_ICON);

    TextureText text_white(
        window.get_render(),
        PATH_FONT,
        COLOR_WHITE,
        TEXT_SIZE
    );

    Action* action = Action::get_instance();
    action->init(
        SDL_SCANCODE_Z,
        SDL_SCANCODE_X,
        SDL_SCANCODE_RETURN,
        SDL_SCANCODE_UP,
        SDL_SCANCODE_DOWN,
        SDL_SCANCODE_LEFT,
        SDL_SCANCODE_RIGHT
    );

    TextureBlock block_black(
        window.get_render(),
        COLOR_BLACK,
        upscale,
        upscale
    );
    
    TextureBlock block_white(
        window.get_render(),
        COLOR_WHITE,
        upscale,
        upscale
    );

    while(exit == false){
        if(window.check_exit()){
            exit = true;
        }else{
            window.clear_screen();

            opcode = Memory[PC] << 8 | Memory[PC+1];
            if(
                (action->check_action(action->BUTTON_ACTION)) ||
                (action->get_state(action->BUTTON_START)) ||
                !step
            ){
                

            }
            
            /*
                Render Grafics
            */
            
            int layout_y = 0;
            text_white.render(
                AnchoP, layout_y,
                "PC:" + to_format_str("0x%04X", PC) + " " + std::to_string(PC-0x200)
            );
            layout_y += TEXT_SIZE;
            
            text_white.render(
                AnchoP, layout_y,
                "OPCODE:" + to_format_str("0x%04X", opcode)
            );
            layout_y += TEXT_SIZE;
            
            text_white.render(
                AnchoP, layout_y,
                "INDEX:" + to_format_str("0x%04X", INDEX)
            );
            layout_y += TEXT_SIZE;

            text_white.render(
                AnchoP, layout_y,
                "StackP:" + to_format_str("0x%04X", StackP)
            );
            layout_y += TEXT_SIZE;

            text_white.render(
                AnchoP, layout_y,
                "TIMER_D:" + to_format_str("0x%04X", delay_timer)
            );
            layout_y += TEXT_SIZE;
            
            text_white.render(
                AnchoP, layout_y,
                "TIMER_S:" + to_format_str("0x%04X", sound_timer)
            );
            layout_y += TEXT_SIZE;
            
            for(int i=0; i<16; i++){
                text_white.render(
                    AnchoP, layout_y,
                    "V[" + to_format_str("%01X", i) + "]:" + to_format_str("0x%04X", V[i])
                );
                layout_y += TEXT_SIZE;
            }
            layout_y = 0;
            for(int i=15; i>=0; i--){
                text_white.render(
                    AnchoP+150, layout_y,
                    "STACK[" + to_format_str("%01X", i) + "]:" + to_format_str("0x%04X", Stack[i])
                );
                layout_y += TEXT_SIZE;
            }
            
            /*        
                Memory Scroll
            */

            layout_y = 0;
            for(int i=0; i<10; i++){
                int memory_value_temp = Memory[i + scrollIndex];
                if(PC == i + scrollIndex){
                    text_white.render(
                        0, 350+layout_y,
                        to_format_str("*0x%04X", i + scrollIndex) + " " +
                        to_format_str("0x%02X", memory_value_temp) + " " +
                        std::bitset <8>(memory_value_temp).to_string()
                    );
                }else{
                    text_white.render(
                        0, 350+layout_y,
                        to_format_str(" 0x%04X", i + scrollIndex) + " " +
                        to_format_str("0x%02X", memory_value_temp) + " " +
                        std::bitset <8>(memory_value_temp).to_string()
                    );
                }

                layout_y += TEXT_SIZE;
            }
            std::bitset <64> temp_screen;
            for(int y = 0;y < emuH;y++){
                temp_screen = std::bitset <64>(Chip8_pantalla[y]);
                for(int x = 0;x < emuW; x++){
                    if(temp_screen[emuW-x-1] == 0){
                        block_black.render(
                            x*upscale,
                            y*upscale
                        );
                    }else{
                        block_white.render(
                            x*upscale,
                            y*upscale
                        );
                    }
                }
            }
            //printf("--------------------------------\n");
            
            
            if((action->get_state(action->BUTTON_MOVE_UP))&&(scrollIndex>0)){
                scrollIndex--;
            }else if((action->get_state(action->BUTTON_MOVE_DOWN))&&(scrollIndex<0xFFFF-10)){
                scrollIndex++;
            }
            
            window.update_screen();
        }
    }
    return 0;
}








































