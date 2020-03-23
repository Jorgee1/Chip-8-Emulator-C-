#include <SDL.h>
#include <SDL_ttf.h>
#include <SDL_image.h>

#include <string>
#include <bitset>

#include "window.h"
#include "texture.h"
#include "action.h"
#include "chip8.h"

std::string to_format_str(char format[], int value){
    char hex[8];
    sprintf(hex, format, value);
    return std::string(hex);
}

int main( int argc, char* args[] ){
    std::string NAME = "Chip 8";
    
    int TEXT_SIZE   =  15;
    int upscale     =  10;
    int scrollIndex = 0x2E7; 
       
    bool exit  = false;
    bool debug = false;
    
    if (debug){
        upscale = 4;
    }
    
    /*
        CHIP 8 INIT
    */
    
    Chip8 chip8;
    chip8.step = false;
    
    int AnchoP = chip8.w*upscale;
    int LargoP = chip8.h*upscale;
    int SCREEN_WIDTH  = AnchoP;
    int SCREEN_HEIGHT = LargoP;
    
    if (debug){
        SCREEN_WIDTH  = 600;
        SCREEN_HEIGHT = 600;
    }

    /*
        LOAD MEMORY
    */
    if(argc>1){
        if(!chip8.load_rom(args[1])){
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
        SDL_SCANCODE_1, SDL_SCANCODE_2, SDL_SCANCODE_3, SDL_SCANCODE_4,
        SDL_SCANCODE_Q, SDL_SCANCODE_W, SDL_SCANCODE_E, SDL_SCANCODE_R,
        SDL_SCANCODE_A, SDL_SCANCODE_S, SDL_SCANCODE_D, SDL_SCANCODE_F,
        SDL_SCANCODE_Z, SDL_SCANCODE_X, SDL_SCANCODE_C, SDL_SCANCODE_V,
        
        SDL_SCANCODE_P, SDL_SCANCODE_RETURN,
        SDL_SCANCODE_UP, SDL_SCANCODE_DOWN
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
            chip8.fetch();
            
            if(
                (action->check_action(action->BUTTON_ACTION)) ||
                (action->get_state(action->BUTTON_START)) ||
                !chip8.step
            ){
                chip8.cycle();
            }
            
            for(int i=0;i<16;i++){
                chip8.key[i] = action->get_state(i);
            }

            /*
                Render Grafics
            */
            
            
            if(debug == true){
                int layout_y = 0;
                text_white.render(
                    AnchoP, layout_y,
                    "PC:" + to_format_str("0x%04X", chip8.pc) +
                    " " +
                    std::to_string(chip8.pc-0x200)
                );
                layout_y += TEXT_SIZE;
                
                text_white.render(
                    AnchoP, layout_y,
                    "OPCODE:" + to_format_str("0x%04X", chip8.op)
                );
                layout_y += TEXT_SIZE;
                
                text_white.render(
                    AnchoP, layout_y,
                    "INDEX:" + to_format_str("0x%04X", chip8.I)
                );
                layout_y += TEXT_SIZE;

                text_white.render(
                    AnchoP, layout_y,
                    "StackP:" + to_format_str("0x%04X", chip8.sp)
                );
                layout_y += TEXT_SIZE;

                text_white.render(
                    AnchoP, layout_y,
                    "TIMER_D:" + to_format_str("0x%04X", chip8.dt)
                );
                layout_y += TEXT_SIZE;
                
                text_white.render(
                    AnchoP, layout_y,
                    "TIMER_S:" + to_format_str("0x%04X", chip8.st)
                );
                layout_y += TEXT_SIZE;
                
                for(int i=0; i<16; i++){
                    text_white.render(
                        AnchoP, layout_y,
                        "V[" + to_format_str("%01X", i) + "]:" +
                        to_format_str("0x%04X", chip8.V[i])
                    );
                    layout_y += TEXT_SIZE;
                }
                
                layout_y += TEXT_SIZE;
                for(int i=0; i<16; i++){
                    text_white.render(
                        AnchoP, layout_y,
                        "Key[" + to_format_str("%01X", i) + "]:" +
                        to_format_str("0x%04X", chip8.key[i])
                    );
                    layout_y += TEXT_SIZE;
                }
                
                layout_y = 0;
                for(int i=15; i>=0; i--){
                    if (chip8.sp == i){
                        text_white.render(
                            AnchoP+150, layout_y,
                            "*STACK[" + to_format_str("%01X", i) + "]:" +
                            to_format_str("0x%04X", chip8.stack[i])
                        );
                    }else{
                        text_white.render(
                            AnchoP+150, layout_y,
                            " STACK[" + to_format_str("%01X", i) + "]:" +
                            to_format_str("0x%04X", chip8.stack[i])
                        );
                    }
                    layout_y += TEXT_SIZE;
                }
                
                /*        
                    Memory Scroll
                */

                layout_y = 0;
                for(int i=0; i<10; i++){
                    int memory_value_temp = chip8.memory[i + scrollIndex];
                    if(chip8.pc == i + scrollIndex){
                        text_white.render(
                            0, 350+layout_y,
                            to_format_str("*0x%04X", i + scrollIndex) + " " +
                            to_format_str("0x%02X", memory_value_temp) + " " +
                            std::bitset <8>(memory_value_temp).to_string()
                        );
                    }else if (chip8.I == i + scrollIndex){
                        text_white.render(
                            0, 350+layout_y,
                            to_format_str("I0x%04X", i + scrollIndex) + " " +
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
                if((action->get_state(action->BUTTON_MOVE_UP))&&(scrollIndex>0)){
                    scrollIndex--;
                }else if((action->get_state(action->BUTTON_MOVE_DOWN))&&(scrollIndex<0xFFFF-10)){
                    scrollIndex++;
                }
            }
            
            std::bitset <64> temp_screen;
            for(int y = 0;y < chip8.h;y++){
                temp_screen = std::bitset <64>(chip8.screen[y]);
                for(int x = 0;x < chip8.w; x++){
                    if(temp_screen[chip8.w-x-1] == 0){
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
            
            window.update_screen();
        }
    }
    return 0;
}








































