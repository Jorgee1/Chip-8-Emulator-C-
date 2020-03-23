#include <cstdio>
#include <stdlib.h>
#include <ctime>

#ifndef CHIP8_H
#define CHIP8_H

class Chip8{
    public:
        unsigned int pc = 0x200;
        unsigned int op = 0;
        unsigned int I  = 0;
        unsigned int sp = 0;
        unsigned int stack[16];
        unsigned int key[16];
        unsigned int V[16];
        
        int dt = 0;
        int st = 0;

        const int h = 32;
        const int w = 64;
        
        bool step = false;
        
        unsigned char memory[4096];
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
        unsigned long screen[32]; // 8 bytes
       
        
        void (Chip8::*Chip8Table[16]) () = {
            &Chip8::cpu0_code, &Chip8::cpu1_code,
            &Chip8::cpu2_code, &Chip8::cpu3_code,
            &Chip8::cpu4_code, &Chip8::cpu5_code,
            &Chip8::cpu6_code, &Chip8::cpu7_code,
            &Chip8::cpu8_code, &Chip8::cpu9_code,
            &Chip8::cpuA_code, &Chip8::cpuB_code,
            &Chip8::cpuC_code, &Chip8::cpuD_code,
            &Chip8::cpuE_code, &Chip8::cpuF_code,
        };

        Chip8();
        
        bool load_rom(char*);
        
        void cpu0_code();
        void cpu1_code();
        void cpu2_code();
        void cpu3_code();
        void cpu4_code();
        void cpu5_code();
        void cpu6_code();
        void cpu7_code();
        void cpu8_code();
        void cpu9_code();
        void cpuA_code();
        void cpuB_code();
        void cpuC_code();
        void cpuD_code();
        void cpuE_code();
        void cpuF_code();
        
        void fetch();
        void cycle();
};

#endif
