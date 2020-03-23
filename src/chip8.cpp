#include "chip8.h"

Chip8::Chip8(){
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
        if(i<80){
            memory[i] = font[i];
        }else{
            memory[i] = (char) 0;
        }
    }
}

bool Chip8::load_rom(char* path){
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
    pc+=2;
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
    if(V[(op&0x0F00)>>8] == (op&0x00FF) ){
        pc+=2;
    }
    pc+=2;
}

void Chip8::cpu4_code(){
    if(V[(op&0x0F00)>>8] != (op&0x00FF)){
        pc+=2;
    }
    pc+=2;
}

void Chip8::cpu5_code(){
    if(V[(op&0x0F00)>>8] == V[(op&0x00F0)>>4]){
        pc+=2;
    }
    pc+=2;
}

void Chip8::cpu6_code(){
    V[(op&0x0F00)>>8] = (op&0x00FF);
    pc+=2;
}

void Chip8::cpu7_code(){
    V[(op&0x0F00)>>8] += (op&0x00FF);
    V[(op&0x0F00)>>8] &= 0xFF;
    pc+=2;
}

void Chip8::cpu8_code(){
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
            V[0xF] = V[(op&0x0F00)>>8]&0x01;
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
}

void Chip8::cpu9_code(){
    if(V[(op&0x0F00)>>8] != V[(op&0x00F0)>>4]){
        pc+=2;
    }
    pc+=2;
}

void Chip8::cpuA_code(){
    I = op&0x0FFF;
    pc+=2;
}

void Chip8::cpuB_code(){
    pc = V[0] + (op&0x0FFF);
}

void Chip8::cpuC_code(){
    int x = rand () % 255;
    V[(op & 0x0F00) >> 8] = x & (op & 0x00FF);
    pc+=2;
}

void Chip8::cpuD_code(){
    unsigned int x = V[(op & 0x0F00)>>8];
    unsigned int y = V[(op & 0x00F0)>>4];
    unsigned int n = (op & 0x000F);

    V[0xF] = 0x0;
    
    unsigned long temp_char = 0ULL;                     
    int fase = w-8-x;


    for(int i=0; i<n; i++){
        if (y>=h){
            y = y - h*(y/(h));
        }
        temp_char = memory[I+i];

        if (fase>=0){
            screen[y+i] ^=(temp_char<<fase);
        }else{
            screen[y+i] ^=(temp_char>>fase*-1);
        }

    }
    pc+=2;
}

void Chip8::cpuE_code(){
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
}

void Chip8::cpuF_code(){
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
        
            memory[ I ] = TEN;
            memory[I+1] = HUN;
            memory[I+2] = DEC;
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
}
        
void Chip8::cycle(){
    op = memory[pc] << 8 | memory[pc+1];
    (this->*Chip8Table[(op&0xF000)>>12])();
    
    if(dt > 0){
        dt--;
    }
    if(st > 0){
        printf("Beep\n");
        st--;
    }
}




