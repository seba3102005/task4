#include "cpu_simulator.h"
#include <iostream>
#include <iomanip>
#include <bitset>
#include <cmath>
#include <sstream>
#include <regex>

using namespace std;

bool is_valid_instruction(const string& instruction) {
    const regex load_store("^[123][0-9A-F][0-9A-F]{2}$");  // Instructions 1,2,3
    const regex move("^4[0-9A-F][0-9A-F][0-9A-F]$");       // Instruction 4
    const regex arithmetic("^5[0-9A-F][0-9][0-9]$");       // Instruction 5
    const regex floating("^6[0-9A-F][0-9A-F][0-9A-F]$");   // Instruction 6
    const regex bitwise("^[789][0-9A-F][0-9A-F][0-9A-F]$"); // Instructions 7,8,9
    const regex rotate("^A[0-9A-F][0-9A-F][0-9A-F]$");     // Instruction A
    const regex jump("^B[0-9A-F][0-9A-F]{2}$");           // Instruction B
    const regex halt("^C000$");                            // Instruction C
    const regex jump_greater("^D[0-9A-F][0-9A-F]{2}$");   // Instruction D

    return regex_match(instruction, load_store) ||
           regex_match(instruction, move) ||
           regex_match(instruction, arithmetic) ||
           regex_match(instruction, floating) ||
           regex_match(instruction, bitwise) ||
           regex_match(instruction, rotate) ||
           regex_match(instruction, jump) ||
           regex_match(instruction, halt) ||
           regex_match(instruction, jump_greater);
}
// Registers Implementation
Registers::Registers(int count) : reg(count, 0) {
    reg[0] = 0x00;
    reg[1] = 0x00;
    reg[2] = 0x00;
    reg[3] = 0x00;
    reg[4] = 0x00;
    reg[5] = 0x00;
    reg[6] = 0x00;
    reg[7] = 0x00;
    reg[8] = 0x00;
    reg[9] = 0x00;
    reg[10] = 0x00;
    reg[11] = 0x00;
    reg[12] = 0x00;
    reg[13] = 0x00;
    reg[14] = 0x00;
    reg[15] = 0x00;
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

void Registers::print()  const{
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
        if (i % 2 == 0)
        {
            cout<< "\n" << hex << setw(2) << setfill('0') << i << ": ";
        }

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

void cu::jump_if_equal(Registers &registers, int &program_counter, int R, int XY,Memory memory) {

    if (registers.get(R) == registers.get(0))
    {

        program_counter = XY ;
        string s1 = to_string(memory.get(program_counter));
        string s2 = to_string(memory.get(program_counter+1));
        string s = s1+s2;
        if(s=="00")
        {
            return;
        }

        execute_instruction(s,registers,memory,program_counter);

    }


}

void cu::jump_if_greater(Registers &registers, int &program_counter, int R, int XY) {
    if (registers.get(R) > registers.get(0)) {
        program_counter = XY - 1;
    }
}

void cu::halt() {
    cout << "Program halted." << endl;

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
void alu::binary_rotation(Registers &registers, int R, int X) {
    int value_r = registers.get(R);
    int value_x = registers.get(X);
    value_r &= 0xFF;
    int result = ((value_r >> value_x) | (value_r << (8 - value_x))) & 0xFF;
    registers.set(R, result);
}

