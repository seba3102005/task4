//
// Created by sebaa on 10/31/2024.
//

#ifndef VOLE_MACHINE_CPU_SIMULATOR_H
#define VOLE_MACHINE_CPU_SIMULATOR_H


#ifndef CPU_SIMULATOR_H
#define CPU_SIMULATOR_H

#include <vector>
#include <string>
using namespace std;
class Registers {
private:
    std::vector<int> reg;
public:
    Registers(int count);
    int get(int index) const;
    void set(int index, int value);
    void print() const;
};

class Memory {
private:
    std::vector<int> memory;
public:
    Memory(int size);
    int get(int address) const;
    void set(int address, int value);
    void print() const;
};

class cu {
public:
    void load_from_memory(Registers &registers, Memory &memory, int R, int XY);
    void load_immediate(Registers &registers, int R, int XY);
    void store_in_memory(Registers &registers, Memory &memory, int R, int XY);
    void move(Registers &registers, int destReg, int srcReg);
    void jump_if_equal(Registers &registers, int &program_counter, int R, int XY,Memory memory);
    void jump_if_greater(Registers &registers, int &program_counter, int R, int XY);
    void halt();
};

class alu {
public:
    int hex_dec(char destReg);
    int add_twos_complement(Registers &registers, int R, int S, int T);
    int floatToBinary(Registers registers,int  R,char X,char Y);
    string intToBinary(int num);
    void bitwise_or(Registers &registers, int R, int S, int T);
    void bitwise_and(Registers &registers, int R, int S, int T);
    void bitwise_xor(Registers &registers, int R, int S, int T);
};

void execute_instruction(const std::string &instruction, Registers &registers, Memory &memory, int &program_counter);

#endif


#endif //VOLE_MACHINE_CPU_SIMULATOR_H
