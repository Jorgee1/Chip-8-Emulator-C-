#include "chip8.h"

Chip8::Chip8(){
    srand(time(0));
    
    for(int y=0;y<h;y++){
        screen[y] = 0;
    }

    for(int i=0;i<16;i++){
        V[i] = 0;
    }
    
    for(int i=0;i<16;i++){
        stack[i] = 0;
    }
    
    for(int i=0;i<16;i++){
        key[i]  = 0;
    }
    
    for(int i=0;i<4096;i++){
        if(i<80){
            memory[i] = font[i];
        }else{
            memory[i] = 0;
        }
    }
}

bool Chip8::load_rom(char* path){
    FILE* rom = fopen(path, "rb");
    int size = 0;
    char* buffer = nullptr;
    
    if(rom){
        fseek(
            rom, 0,
            SEEK_END
        );
        size = ftell(rom);
        rewind(rom);
        
        buffer = (char*) malloc(
            sizeof(char)*size
        );
        
        fread(
            buffer, 1,
            size,
            rom
        );
        
        for(int i=0; i<size; i++){
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


void Chip8::cpu0_code(){
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
}

void Chip8::cpu1_code(){
    pc = (op&0x0FFF);
}

void Chip8::cpu2_code(){
    stack[sp] = pc;
    sp++;
    pc = op&0x0FFF;
}

void Chip8::cpu3_code(){
    int x  = (op&0x0F00)>>8;
    int kk = op&0x00FF;
    
    if(V[x] == kk){
        pc+=2;
    }
    pc+=2;
}

void Chip8::cpu4_code(){
    int x  = (op&0x0F00)>>8;
    int kk = op&0x00FF;
    
    if(V[x] != kk){
        pc+=2;
    }
    pc+=2;
}

void Chip8::cpu5_code(){
    int x = (op&0x0F00)>>8;
    int y = (op&0x00F0)>>4;
    
    if(V[x] == V[y]){
        pc+=2;
    }
    pc+=2;
}

void Chip8::cpu6_code(){
    int x  = (op&0x0F00)>>8;
    int kk = op&0x00FF;
    
    V[x] = kk;
    pc+=2;
}

void Chip8::cpu7_code(){
    int x  = (op&0x0F00)>>8;
    int kk = op&0x00FF;

    V[x] += kk;
    V[x] &= 0xFF;
    pc+=2;
}

void Chip8::cpu8_code(){
    switch(op&0x000F){

        case 0x0000:{
            int x = (op&0x0F00)>>8;
            int y = (op&0x00F0)>>4;
            V[x] = V[y];
            break;
        }

        case 0x0001:{
            int x = (op&0x0F00)>>8;
            int y = (op&0x00F0)>>4;
            V[x] |= V[y];
            break;
        }

        case 0x0002:{
            int x = (op&0x0F00)>>8;
            int y = (op&0x00F0)>>4;
            V[x] &= V[y];
            break;
        }

        case 0x0003:{
            int x = (op&0x0F00)>>8;
            int y = (op&0x00F0)>>4;
            V[x] ^= V[y];
            break;
        }

        case 0x0004:{
            int x = (op&0x0F00)>>8;
            int y = (op&0x00F0)>>4;
            
            int vx = V[x];
            int vy = V[y];
            
            if(vx + vy > 0xFF){
                V[0xF] = 0x01;
            }else{
                V[0xF] = 0x00;
            }
            
            V[x] += vy;
            V[x] &= 0xFF;
            break;
        }

        case 0x0005:{
            int x = (op&0x0F00)>>8;
            int y = (op&0x00F0)>>4;
            
            int vx = V[x];
            int vy = V[y];
            
            if(vx > vy){
                V[0xF] = 0x01;
            }else{
                V[0xF] = 0x00;
            }
            
            V[x] -= vy;
            V[x] &= 0xFF;
            break;
        }

        case 0x0006:{
            int x = (op&0x0F00)>>8;
            V[0xF] = V[x]&1;
            V[x] >>= 1;
            break;
        }

        case 0x0007:{
            int x = (op&0x0F00)>>8;
            int y = (op&0x00F0)>>4;
            
            int vx = V[x];
            int vy = V[y];
            
            if(vy > vx){
                V[0xF] = 0x01;
            }else{
                V[0xF] = 0x00;
            }
            V[x] -= vy;
            V[x] &= 0xFF;
            break;
        }

        case 0x000E:{
            int x = (op&0x0F00)>>8;
            V[0xF] = (V[x]&0x80)>>7;
            V[x] <<= 1;
            V[x] &= 0xFF;
            break;
        }

    }
    pc+=2;
}

void Chip8::cpu9_code(){
    int x = (op&0x0F00)>>8;
    int y = (op&0x00F0)>>4;
    
    if(V[x] != V[y]){
        pc+=2;
    }
    
    pc+=2;
}

void Chip8::cpuA_code(){
    I = op&0x0FFF;
    pc+=2;
}

void Chip8::cpuB_code(){
    int nnn = op&0x0FFF;
    pc = (nnn + V[0])&0xFFFF;
}

void Chip8::cpuC_code(){
    int x  = (op & 0x0F00) >> 8;
    int kk = op & 0x00FF;
    int rnd = rand() % 256;
    
    V[x] = rnd & kk;
    pc+=2;
}

void Chip8::cpuD_code(){
    unsigned int x = V[(op & 0x0F00)>>8];
    unsigned int y = V[(op & 0x00F0)>>4];
    unsigned int n = (op & 0x000F);

    V[0xF] = 0;
    
    unsigned long temp_sprite = 0;
    unsigned long temp_screen = 0;

    if (x>=w){
        x -= (x/w)*w;
    }
                      
    int fase = w-8-x;

    
    for(int i=0;i<n;i++){
        if (y+i>=h){
            y =  i*(y/h) - i;
        }

        temp_screen = screen[y+i];
        temp_sprite = memory[I+i];

        if (fase>=0){
            screen[y+i] ^=(temp_sprite<<fase);
        }else{
            screen[y+i] ^=(temp_sprite>>fase*-1);
        }
                
        if (temp_screen>screen[y+i]){
            V[0xF] = 1;
        }

    }
    pc+=2;
}

void Chip8::cpuE_code(){
    switch(op&0x00FF){
        case 0x009E:{
            int x = (op&0x0F00)>>8;
            int vx = V[x];
            if(key[vx]==1){
               pc+=2;
            }
            break;
        }
        case 0x00A1:{
            int x = (op&0x0F00)>>8;
            int vx = V[x];
            if(key[vx]==0){
                pc+=2;
            }
            break;
        }
    }
    pc+=2;
}

void Chip8::cpuF_code(){
    switch(op&0x00FF){
        case 0x0007:{
            int x = (op&0x0F00)>>8;
            V[x] = dt;
            pc+=2;
            break;
        }
        case 0x000A:{
            int x = (op&0x0F00)>>8;
            for(int i=0;i<16;i++){
                if(key[i]==1){
                    V[x] = i;
                    pc+=2;
                    break;
                }
            }
            break;
        }
        case 0x0015:{
            int x = (op&0x0F00)>>8;
            dt = V[x];
            pc+=2;
            break;
        }
        case 0x0018:{
            int x = (op&0x0F00)>>8;
            st = V[x];
            pc+=2;
            break;
        }
        case 0x001E:{
            int x = (op&0x0F00)>>8;
            I += V[x];
            pc+=2;
            break;
        }
        case 0x0029:{
            int x = (op&0x0F00)>>8;
            I = V[x]*5;
            pc+=2;
            break;
        }
        case 0x0033:{
            int x = (op&0x0F00)>>8;
            
            int TEN = V[x]/100;
            int HUN = (V[x]/10) - TEN*10;
            int DEC = V[x] - HUN*10 - TEN*100;
        
            memory[ I ] = TEN;
            memory[I+1] = HUN;
            memory[I+2] = DEC;
            pc+=2;
            break;
        }
        case 0x0055:{
            int x = (op&0x0F00)>>8;
            for(int i=0;i<=x;i++){
                memory[I+i] = V[i];
            }
            pc+=2;
            break;
        }
        case 0x0065:{
            int x = (op&0x0F00)>>8;
            for(int i=0;i<=x;i++){
                V[i] = memory[I+i];
            }
            pc+=2;
            break;
        }

    }
}

void Chip8::fetch(){
    op = memory[pc] << 8 | memory[pc+1];
}

void Chip8::cycle(){
    fetch();
    
    (this->*Chip8Table[(op&0xF000)>>12])();

    if(dt > 0){
        dt--;
    }
    if(st > 0){
        printf("Beep\n");
        st--;
    }
}




