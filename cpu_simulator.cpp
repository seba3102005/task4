#include "cpu_simulator.h"
#include <iostream>
#include <iomanip>
#include <bitset>
#include <cmath>
#include <sstream>

using namespace std;

// Registers Implementation
Registers::Registers(int count) : reg(count, 0) {
    reg[0] = 0x00; // R0 initialized to zero
    reg[1] = 0x01; // R1 initialized to 1
    reg[2] = 0x02; // R2 initialized to 2
    reg[3] = 0x03; // R3 initialized to 3
    reg[4] = 0x04; // R4 initialized to 4
    reg[5] = 0x05; // R5 initialized to 5
    reg[6] = 0x06; // R6 initialized to 6
    reg[7] = 0x07; // R7 initialized to zero
    reg[8] = 0x08; // R8 initialized to zero
    reg[9] = 0x09; // R9 initialized to 80 (hex)
    reg[10] = 0xA0; // RA initialized to A0
    reg[11] = 0xB0; // RB initialized to B0
    reg[12] = 0xC0; // RC initialized to C0
    reg[13] = 0xD0; // RD initialized to D0
    reg[14] = 0xE0; // RE initialized to E0
    reg[15] = 0xF0; // RF initialized to F0
}

int Registers::get(int index) const {
    return reg[index];
}

void Registers::set(int index, int value) {
    if (index >= 0xA && index <= 0xF) {
        int regIndex = index - 0xA + 10;
        reg[regIndex] = reg[value];
    } else if (index >= 0 && index <= 9) {
        reg[index] = value;
    }
}

void Registers::print() const {
    cout << "\nRegisters:" << endl;
    for (int i = 0; i < reg.size(); ++i) {
        cout << "R" << hex << i << ": " << setw(2) << setfill('0') << hex << uppercase << reg[i] << endl;
    }
    cout << "----------------------------\n";
}

// Memory Implementation
Memory::Memory(int size) : memory(size, 0) {}

int Memory::get(int address) const {
    return memory[address];
}

void Memory::set(int address, int value) {
    memory[address] = value;
}

void Memory::print() const {
    cout << "\nMemory: " << endl;
    for (int i = 0; i < memory.size(); ++i) {
        if (i % 16 == 0) cout << "\n" << hex << setw(2) << setfill('0') << i << ": ";
        cout << setw(2) << setfill('0') << memory[i] << " ";
    }
    cout << "\n----------------------------\n";
}

// Control Unit Implementation
void cu::load_from_memory(Registers &registers, Memory &memory, int R, int XY) {
    int value = memory.get(XY);
    registers.set(R, value);
}

void cu::load_immediate(Registers &registers, int R, int XY) {
    registers.set(R, XY);
}

void cu::store_in_memory(Registers &registers, Memory &memory, int R, int XY) {
    int value = registers.get(R);
    memory.set(XY, value);
}

void cu::move(Registers &registers, int destReg, int srcReg) {
    int value = registers.get(srcReg);
    registers.set(destReg, value);
}

void cu::jump_if_equal(Registers &registers, int &program_counter, int R, int XY) {
    if (registers.get(R) == registers.get(0)) {
        program_counter = XY - 1;
    }
}

void cu::jump_if_greater(Registers &registers, int &program_counter, int R, int XY) {
    if (registers.get(R) > registers.get(0)) {
        program_counter = XY - 1;
    }
}

void cu::halt() {
    cout << "Program halted." << endl;
    exit(0);
}

// ALU Implementation
int alu::hex_dec(char destReg) {
    int destReg2 = -1;
    if (destReg >= '0' && destReg <= '9') {
        destReg2 = destReg - '0';
    } else if (destReg >= 'A' && destReg <= 'F') {
        destReg2 = 10 + (destReg - 'A');
    }
    return destReg2;
}

int alu::add_twos_complement(Registers &registers, int R, int S, int T) {
    int b = S + T;
    int bitCount = (b == 0) ? 1 : static_cast<int>(std::log2(b)) + 1;
    std::bitset<32> binary(b);
    std::string s = binary.to_string().substr(32 - bitCount);
    bool flag = false;
    string number;

    for (int i = s.size() - 1; i >= 0; i--) {
        if (!flag) {
            if (s[i] == '0') {
                number += "0";
            } else {
                number += "1";
                flag = true;
            }
        } else {
            number = (s[i] == '0') ? "1" + number : "0" + number;
        }
    }

    int decimalValue = 0;
    int bit = 0;
    for (int i = number.size() - 1; i >= 0; i--) {
        if (number[i] == '1') {
            decimalValue += pow(2, bit);
        }
        bit++;
    }
    return decimalValue;
}

string alu::intToBinary(int num) {
    // Convert integer to binary
    string binary = "";
    if (num == 0) {
        binary = "0";
    } else {
        while (num > 0) {
            binary = to_string(num % 2) + binary;
            num /= 2;
        }
    }
    return binary;
}


int alu::floatToBinary(Registers registers,int  R,char X,char Y) {
    float num = hex_dec(X)+ hex_dec(Y);
    // Separate integer and fractional parts
    int integerPart = static_cast<int>(num);


    // Convert integer part to binary
    string integerBinary = "";
    if (integerPart == 0) {
        integerBinary = "0";
    } else {
        while (integerPart > 0) {
            integerBinary = to_string(integerPart % 2) + integerBinary;
            integerPart /= 2;
        }
    }

    // Convert fractional part to binary


    // Combine integer and fractional parts
    string finalBinary = integerBinary ;

    // Calculate exponent part
    int index = integerBinary.size() - 1;
    int e = index + 8; // example bias of 8 for a 4-bit exponent representation
    string exponentBinary = intToBinary(e);

    // Calculate mantissa part
    string mantissa;
    for (int i = 1; i < finalBinary.size(); i++) {
        if (finalBinary[i] != '.') {
            mantissa += finalBinary[i];
        }
    }


    string answer=exponentBinary+mantissa;
    unsigned long long fin_ans = bitset<64>(answer).to_ullong();
    std::ostringstream hexStream;
    hexStream << std::hex << uppercase << fin_ans;
    std::string hexString = hexStream.str();

    int fin_ans2 = stoi(hexString, nullptr, 16);


    return fin_ans2;
}



void alu::bitwise_or(Registers &registers, int R, int S, int T) {
    int value_s = registers.get(S);
    int value_t = registers.get(T);
    int result = value_s | value_t;
    registers.set(R, result);
}

void alu::bitwise_and(Registers &registers, int R, int S, int T) {
    int value_s = registers.get(S);
    int value_t = registers.get(T);
    int result = value_s & value_t;
    registers.set(R, result);
}

void alu::bitwise_xor(Registers &registers, int R, int S, int T) {
    int value_s = registers.get(S);
    int value_t = registers.get(T);
    int result = value_s ^ value_t;
    registers.set(R, result);
}