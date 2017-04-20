#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_image.h>
#include <iostream>
#include <curses.h>
#include <bitset>
#include <string>
#include "Textura.h"
#include "Init.h"
#include "Event.h"
#include "Init.h"
#include "Textura.h"
#include "Event.h"

using namespace std;
//Init SDL_Inicio;

SDL_Window*  Pantalla  = NULL;
SDL_Renderer* RenderP  = NULL;
TTF_Font*        Font  = NULL;
FILE*            room  = NULL;
SDL_Color Negro     = {0,0,0};
Event evento;
unsigned char Memory[4096],V[16];

int Stack[16];
int PC,StackP,INDEX,upscale = 15,AnchoP = 64*upscale,LargoP = 32*upscale,opcode;
int Chip6_pantalla[64][32],delay_timer,sound_timer;
/////////////////   x   y  ////////////////
int Key[16];

void clearscrn()
{
    for(int i=0; i<32; i++)
    {
        for(int j=0; j<64; j++)
        {
            Chip6_pantalla[j][i] = 0;
            move(i+20,j+2);
            printw("%i",Chip6_pantalla[j][i]);
        }
    }
}

void printscr()
{
    for(int i=0; i<32; i++)
    {
        for(int j=0; j<64; j++)
        {
            move(i+20,j);
            printw("%i",Chip6_pantalla[j][i]);
        }
    }
}

void Fill(unsigned char x[],int n)
{
    for(int i=0; i<n; i++)
    {
        x[i] = (char) 0;
    }
}

int Key_check(){
    int pressed = 16;
    for(int i=0;i<16;i++){
        if(Key[i]==1){
            pressed = i;
        }
    }
    return pressed;
}

unsigned char chip8_fontset[80] =
{
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

void Chip_8_INICIO()
{
    PC             = 0x200;
    opcode         = 0;
    INDEX          = 0;
    StackP         = 0;
    delay_timer    = 0;
    sound_timer    = 0;
    Fill(Memory, sizeof( Memory ) / sizeof( Memory[0] ));
    Fill(V, sizeof( V ) / sizeof( V[0] ));
    for(int i=0;i<16;i++){
        Key[i]=0;
    }
    clearscrn();
}

void Chip8_Cycle()
{
    opcode = Memory[PC] << 8 | Memory[PC+1];
    move(2,13);

    printw("0x%04X",int(opcode));
    //cout << hex << "0x" << uppercase << int(opcode) << endl;
    //cout << hex << "0x" << uppercase << int((opcode&0x0F00)>>8) << endl;
    switch(opcode&0xF000)
    {

    case 0x0000:
    {
        switch(opcode&0x0FFF)
        {
        case 0x00E0:
            clear();
            PC+=2;
            clearscrn();
            break;
        case 0x00EE:
            StackP--;
            PC = Stack[StackP];
            Stack[StackP] = 0x0000;
            PC+=2;
            break;
        }
        break;
    }

    case 0xA000:
    {
        INDEX = opcode&0x0FFF;
        PC+=2;
        break;
    }

    case 0xB000:
    {
        PC = V[0] + (opcode&0x0FFF);
        break;
    }

    case 0xC000:
    {
        int x = (rand () % 255);
        //string Numb = bitset<8> (x).to_string();
        //string Numb1 = bitset<8> (opcode & 0x00FF).to_string();
        //string Res = bitset<8> (x & (opcode & 0x00FF)).to_string();
        //mvaddstr(9,0,Numb1.c_str());
        //mvaddstr(10,0,Numb.c_str());
        //mvaddstr(11,0,Res.c_str());
        //printw("rand = 0x%06X",x);
        V[(opcode & 0x0F00) >> 8] = x & (opcode & 0x00FF);
        PC+=2;
        break;
    }

    case 0xD000:
    {
        int x_cor = V[(opcode&0x0F00)>>8];
        int y_cor = V[(opcode&0x00F0)>>4];
        V[0xF] = 0x0;

        if(x_cor > 64){
            while(x_cor>64){
                x_cor = x_cor -64;
            }
        }


        // Draw Sprite //////////////////////

        for(int i=0; i<16; i++)
        {
            if(i<(opcode&0x000F)){
                bitset<8> Numb((int)Memory[INDEX+i]);
                move(0+i,90);
                printw(Numb.to_string().c_str());
            }else{
                move(0+i,90);
                printw("        ");
            }
        }


        for(int i=0; i<16; i++)
        {
            if(i<(opcode&0x000F)){
                for(int j=0; j<8; j++)
                {
                    int Draw = Chip6_pantalla[x_cor+j][y_cor+i];
                    move(i,99+j);
                    printw("%X",Draw);
                }
            }else{
                    move(i,99);
                    printw("        ");
            }
        }


        /////////////////////////////////////

        for(int i=0;i<(opcode&0x000F);i++){
            bitset<8> Numb((int)Memory[INDEX+i]);
            for(int j = 0;j<8;j++){

                //while(getch() !='a'){}


                move(4,4);
                printw("x = %i ",x_cor+j);
                move(4,16);
                printw("y = %i ",y_cor+i);
                printscr();

                //while(getch() !='a'){}

                if(Chip6_pantalla[x_cor+j][y_cor+i] == Numb[7-j]){
                    move(5,5);
                    printw("entro   ");
                    if(Chip6_pantalla[x_cor+j][y_cor+i]==1){
                        V[0xF] = 0x1;
                        move(5,5);
                        printw("COlision");
                    }
                    Chip6_pantalla[x_cor+j][y_cor+i] = 0;
                }else{
                    Chip6_pantalla[x_cor+j][y_cor+i] = 1;
                    move(5,5);
                    printw("no entro");
                }

            }
        }
        PC+=2;
        break;
    }

    case 0xE000:
    {
        switch(opcode&0x00FF)
        {
        case 0x00A1:
            if(Key[V[(opcode&0x0F00)>>8]]==0)
            {
                PC+=2;
            }
            break;
        case 0x009E:
            if(Key[V[(opcode&0x0F00)>>8]]==1)
            {
                PC+=2;
            }
            break;
        }
        PC+=2;
        break;
    }

    case 0xF000:
    {
        switch(opcode&0x00FF)
        {
        case 0x0007:
            V[(opcode&0x0F00)>>8] = delay_timer;
            //V[(opcode&0x0F00)>>8] = 0;
            break;
        case 0x000A:
            {
                int Pressed = Key_check();

                if(Pressed == 16){
                    PC-=2;
                }else{
                    V[(opcode&0x0F00)>>8] = Pressed;
                }
                break;
            }
        case 0x0015:
            delay_timer = V[(opcode&0x0F00)>>8];
            break;
        case 0x0018:
            break;

        case 0x001E:
            INDEX = INDEX+V[(opcode&0x0F00)>>8];
            break;

        case 0x0029:
            INDEX = V[(opcode&0x0F00)>>8]*0x5;
            break;

        case 0x0033:
            Memory[INDEX]   =  V[(opcode&0x0F00)>>8]/100;
            Memory[INDEX+1] = (V[(opcode&0x0F00)>>8]/10)%10;
            Memory[INDEX+2] = (V[(opcode&0x0F00)>>8]%100)%10;
            break;

        case 0x0055:
            for(int i=0; i<=((opcode&0x0F00)>>8); i++)
            {
                Memory[INDEX+i] = V[i];
            }
            break;

        case 0x0065:
            for(int i=0; i<=((opcode&0x0F00)>>8); i++)
            {
                V[i] = Memory[INDEX+i];
            }
            break;
        }
        PC+=2;
        break;
    }

    case 0x1000:
    {
        PC = (opcode&0x0FFF);
        break;
    }

    case 0x2000:
    {
        Stack[StackP] = PC;
        StackP++;
        PC = opcode&0x0FFF;
        break;
    }

    case 0x3000:
    {
        if( V[(opcode&0x0F00)>>8] == (opcode&0x00FF) )
        {
            PC+=2;
        }
        PC+=2;
        break;
    }

    case 0x4000:
    {
        if( V[(opcode&0x0F00)>>8] != (opcode&0x00FF))
        {
            PC+=2;
        }
        PC+=2;
        break;
    }

    case 0x5000:
    {
        if(V[(opcode&0x0F00)>>8]==V[(opcode&0x00F0)>>4])
        {
            PC+=2;
        }
        PC+=2;
        break;
    }

    case 0x6000:
    {
        V[(opcode&0x0F00)>>8] = (opcode&0x00FF);
        PC+=2;
        break;
    }

    case 0x7000:
    {
        V[(opcode&0x0F00)>>8] = (V[(opcode&0x0F00)>>8]+(opcode&0x00FF))&0xFF;
        PC+=2;
        break;
    }

    /// ALU /////////////////////////
    case 0x8000:
    {
        switch(opcode&0x000F)
        {

        case 0x0000:
        {
            V[(opcode&0x0F00)>>8] = V[(opcode&0x00F0)>>4];
            break;
        }

        case 0x0001:
        {
            V[(opcode&0x0F00)>>8] = V[(opcode&0x0F00)>>8] | V[(opcode&0x00F0)>>4];
            break;
        }

        case 0x0002:
        {
            V[(opcode&0x0F00)>>8] = V[(opcode&0x0F00)>>8] & V[(opcode&0x00F0)>>4];
            break;
        }

        case 0x0003:
        {
            V[(opcode&0x0F00)>>8] = V[(opcode&0x0F00)>>8] ^ V[(opcode&0x00F0)>>4];
            break;
        }

        case 0x0004:
        {
            int x1 = V[(opcode&0x0F00)>>8]&0xFF;
            int y1 = V[(opcode&0x00F0)>>4]&0xFF;
            int z = 0;                                                                          /// add
            if( (x1+y1) > 0xFF )
            {
                V[0xF] = 0x01;
            }
            else
            {
                V[0xF] = 0x00;
            }
            z = ( x1 + y1 )&0xFF;
            V[(opcode&0x0F00)>>8] = z;
            break;
        }

        case 0x0005:
        {
            int x2 = V[(opcode&0x0F00)>>8]&0xFF;
            int y2 = V[(opcode&0x00F0)>>4]&0xFF;                                                                           /// sub
            if( x2 > y2)
            {
                V[0xF] = 0x01;
            }
            else
            {
                V[0xF] = 0x00;
            }
            V[(opcode&0x0F00)>>8] = (V[(opcode&0x0F00)>>8] - V[(opcode&0x00F0)>>4])&0xFF;
            break;
        }

        case 0x0006:                                                                                /// div2
        {
            V[0xF] = (V[(opcode&0x0F00)>>8]&0x01);
            V[(opcode&0x0F00)>>8] = V[(opcode&0x0F00)>>8] >> 1;
            break;
        }

        case 0x0007:
        {
            int x3 = V[(opcode&0x00F0)>>4]&0xFF;
            int y3 = V[(opcode&0x0F00)>>8]&0xFF;                                                                      /// sub
            if( y3 > x3)
            {
                V[0xF] = 0x01;
            }
            else
            {
                V[0xF] = 0x00;
            }
            V[(opcode&0x0F00)>>8] = (V[(opcode&0x00F0)>>8] - V[(opcode&0x0F00)>>4])&0xFF;
            break;
        }

        case 0x000E:                                                                                  /// mult*2
        {
            V[0xF] = ((V[(opcode&0x0F00)>>8]&0x80)>>7);
            V[(opcode&0x0F00)>>8] = (V[(opcode&0x0F00)>>8] << 1)&0xFF;
            break;
        }

        }
        PC+=2;
        break;
    }
    /// ALU /////////////////////////

    case 0x9000:
    {
        if( V[(opcode&0x0F00)>>8] != V[(opcode&0x00F0)>>4] )
        {
            PC+=2;
        }
        PC+=2;
        break;
    }

    }
    if(delay_timer >0)
    {
        delay_timer--;
    }
    move(1,0);
    printw("PC = 0x%04X",PC);
}

void printV()
{
    move(1,13);
    printw("Index = 0x%04X",INDEX);
    move(0,13);
    printw("SP = 0x%04X",StackP);
    for(int i=0; i<16; i++)
    {
        move(0+i,75);
        printw("V[%X] = ",i);
        move(0+i,75 + 7);
        printw("0x%02X",V[i]);

    }
    for(int i=0; i<16; i++)
    {
        move(0+i,66-11);
        printw("Stack[%X] = ",i);
        move(0+i,66);
        printw("0x%04X",Stack[i]);

    }

    for(int i=0; i<16; i++)
    {
        move(0+i,50-9);
        printw("Key[%X] = ",i);
        move(0+i,50);
        printw("0x%i",Key[i]);

    }

    move(7,7);
    printw(" caracter = ");
    refresh();
    printscr();

}


void cargador()
{
    Chip_8_INICIO();
    srand( time(0) );
    int buffersize;
    char * buffer;
    Font = TTF_OpenFont("FONTS/arial.ttf",20);
    /// Memory //////////
    for(int i=0; i<80; i++)
    {
        Memory[i] = chip8_fontset[i];
    }

    room = fopen("ROOM/UFO","rb");
    fseek (room, 0, SEEK_END);
    buffersize = ftell(room);
    rewind(room);
    buffer = (char*) malloc (sizeof(char)*buffersize);
    fread (buffer,1,buffersize,room);
    for(int i=0; i<buffersize; i++)
    {
        Memory[i+512] = buffer[i];
    }
    /// /////////////////



    move(0,0);
    printw("Inicio");
    move(2,0);
    printw("Codigo op: ");
    nodelay(stdscr,true);

}


int main( int argc, char* args[] )
{
    bool Salir = false;
    initscr();
    Init inicio;
    //while(getch() !='a'){}
    cargador();
    printV();

    while(Salir == false)
    {
        SDL_RenderClear(RenderP);
        SDL_SetRenderDrawColor(RenderP,0,0,0,0xFF);
        evento.Event_Loop(&Salir,Key);

        //while(getch() !='a'){}

        Chip8_Cycle();

        printV();

        SDL_RenderPresent(RenderP);
    }
    inicio.Quit();
    endwin();
    return 0;

}
