#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <iomanip>
#include <bitset>
#include <algorithm>
#include <cmath>

using namespace std;
int program_counter = 0;

// Registers Class
class Registers {
private:
    vector<int> reg;
public:
    Registers(int count) : reg(count, 0) {
        reg[0] = 0x00; // R0 initialized to zero
        reg[1] = 0x01; // R1 initialized to 1
        reg[2] = 0x02; // R2 initialized to 2
        reg[3] = 0x03; // R3 initialized to 3
        reg[4] = 0x04; // R4 initialized to 4
        reg[5] = 0x05; // R5 initialized to 5
        reg[6] = 0x06; // R6 initialized to 6
        reg[7] = 0x07; // R7 initialized to zero
        reg[8] = 0x08; // R8 initialized to zero
        reg[9] = 0xA3; // R9 initialized to 80 (hex)
        reg[10] = 10.0; // RA initialized to A0
        reg[11] = 0xB0; // RB initialized to B0
        reg[12] = 0xC0; // RC initialized to C0
        reg[13] = 0xD0; // RD initialized to D0
        reg[14] = 0xE0; // RE initialized to E0
        reg[15] = 0xF0; // RF initialized to F0
    }

    int get(int index) const {
        return reg[index];
    }

    void set(int index, int value) {
        if (index >= 0xA && index <= 0xF) { // Check if index is within A to F
            int regIndex = index - 0xA + 10; // Convert to register index (10 to 15)
            reg[regIndex] = reg[value]; // Set the value of the target register to the value of the specified register
        } else if (index >= 0 && index <= 9) {
            reg[index] = value; // Set the value for registers 0 to 9
        }
    }

    void print() const {
        cout << "\nRegisters:" << endl;
        for (int i = 0; i < reg.size(); ++i) {
            cout << "R" << hex << i << ": " << setw(2) << setfill('0') << hex << uppercase << reg[i] << endl;
        }
        cout << "----------------------------\n";
    }
};

class Memory {
private:
    vector<int> memory;
public:
    Memory(int size) : memory(size, 0) {}

    int get(int address) const { return memory[address]; }

    void set(int address, int value) { memory[address] = value; }

    void print() const {
        cout << "\nMemory: " << endl;
        for (int i = 0; i < memory.size(); ++i) {
            if (i % 16 == 0) cout << "\n" << hex << setw(2) << setfill('0') << i << ": ";
            cout << setw(2) << setfill('0') << memory[i] << " ";
        }
        cout << "\n----------------------------\n";
    }
};

class cu {
public:
    void load_from_memory(Registers &registers, Memory &memory, int R, int XY) {
        int value = memory.get(XY);
        registers.set(R, value);
    }

    void load_immediate(Registers &registers, int R, int XY) {
        registers.set(R, XY);
    }

    void store_in_memory(Registers &registers, Memory &memory, int R, int XY) {
        int value = registers.get(R);
        memory.set(XY, value);
    }

    void move(Registers &registers, int destReg, int srcReg) {
        int value = registers.get(srcReg);
        registers.set(destReg, value);
    }

    void jump_if_equal(Registers &registers, int &program_counter, int R, int XY) {
        if (registers.get(R) == registers.get(0)) {
            program_counter = XY - 1;
        }
    }

    void jump_if_greater(Registers &registers, int &program_counter, int R, int XY) {
        if (registers.get(R) > registers.get(0)) {
            program_counter = XY - 1;
        }
    }

    void halt() {
        cout << "Program halted." << endl;
        exit(0);
    }
};

class alu {
public:
    int hex_dec(char destReg) {
        int destReg2 = -1;
        if (destReg >= '0' && destReg <= '9') {
            destReg2 = destReg - '0';
        } else if (destReg >= 'A' && destReg <= 'F') {
            destReg2 = 10 + (destReg - 'A');
        }
        return destReg2;
    }

    int add_twos_complement(Registers &registers, int R, int S, int T) {
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

    void add_floating_point(Registers &registers, int R, char X, char Y) {
        int ans1 = registers.get(hex_dec(X));
        int ans2 = registers.get(hex_dec(Y));
        int ans = ans1 + ans2;
        int num_bits = std::log2(ans) + 1;

        std::bitset<32> binary(ans);
        string s = binary.to_string().substr(32 - num_bits);

        int exponent = 127 + (s.size() - 1);
        bitset<8> exp(exponent);
        string fin_exp = exp.to_string();

        string mantessa = "00000000000000000000000";
        string part = s.substr(1);
        string total_mantessa = part + mantessa;
        total_mantessa = total_mantessa.substr(0, 23);

        string total = '0' + fin_exp + total_mantessa;

        unsigned long long decimalValue = std::stoull(total, nullptr, 2);
        std::ostringstream hexStream;
        hexStream << std::hex << decimalValue;
        std::string hexString = hexStream.str();

        for (auto &c : hexString) {
            c = toupper(c);
        }

        int fin_ans = stoi(hexString, nullptr, 16);
        registers.set(R, fin_ans);
    }

    void bitwise_or(Registers &registers, int R, int S, int T) {
        int value_s = registers.get(S);
        int value_t = registers.get(T);

        int result = value_s | value_t;

        registers.set(R, result);
    }

    void bitwise_and(Registers &registers, int R, int S, int T) {
        int value_s = registers.get(S);
        int value_t = registers.get(T);

        int result = value_s & value_t;

        registers.set(R, result);
    }

    void bitwise_xor(Registers &registers, int R, int S, int T) {
        int value_s = registers.get(S);
        int value_t = registers.get(T);

        int result = value_s ^ value_t;

        registers.set(R, result);
    }
};

void execute_instruction(const string &instruction, Registers &registers, Memory &memory, int &program_counter) {
    cout << "Before Execution:\n";
    registers.print();
    memory.print();

    char op_code = instruction[0];
    int R, XY, R2;
    char X, Y;
    R = stoi(instruction.substr(1, 1), nullptr, 16);
    XY = stoi(instruction.substr(2, 2), nullptr, 16);
    X = instruction[2];
    Y = instruction[3];

    cu cu2;
    alu alu2;

    switch (op_code) {
        case '1':
            cu2.load_from_memory(registers, memory, R, XY);
            break;
        case '2':
            cu2.load_immediate(registers, R, XY);
            break;
        case '3':
            cu2.store_in_memory(registers, memory, R, XY);
            break;
        case '4': {
            int destReg = instruction[3];
            char sourceRegChar = instruction[2];
            int destReg2 = alu2.hex_dec(destReg);
            int srcReg = alu2.hex_dec(sourceRegChar);

            if (srcReg != -1 && destReg2 != -1) {
                cu2.move(registers, destReg2, srcReg);
            } else {
                cout << "Invalid source register." << endl;
            }
            break;
        }
        case '5': {
            R2 = stoi(instruction.substr(1, 1), nullptr, 16);
            R = stoi(instruction.substr(2, 1), nullptr, 10);
            XY = stoi(instruction.substr(3, 1), nullptr, 10);
            int value = alu2.add_twos_complement(registers, R2, R, XY);
            registers.set(R2, value);
            break;
        }
        case '6':
            alu2.add_floating_point(registers, R, X, Y);
            break;
        case '7': {
            int S = alu2.hex_dec(instruction[2]);
            int T = alu2.hex_dec(instruction[3]);
            alu2.bitwise_or(registers, R, S, T);
            break;
        }
        case '8': {
            int S = alu2.hex_dec(instruction[2]);
            int T = alu2.hex_dec(instruction[3]);
            alu2.bitwise_and(registers, R, S, T);
            break;
        }
        case '9': {
            int S = alu2.hex_dec(instruction[2]);
            int T = alu2.hex_dec(instruction[3]);
            alu2.bitwise_xor(registers, R, S, T);
            break;
        }
        case 'B':
            cu2.jump_if_equal(registers, program_counter, R, XY);
            break;
        case 'C':
            cu2.halt();
            break;
        case 'D':
            cu2.jump_if_greater(registers, program_counter, R, XY);
            break;
        default:
            cout << "Unknown instruction: " << instruction << endl;
    }

    cout << "After Execution:\n";
    registers.print();
    memory.print();
}

int main() {
    Registers registers(16);
    Memory memory(256);
    vector<string> instructions;

    ifstream file("instructions.txt");
    if (!file) {
        cerr << "Error opening file." << endl;
        return 1;
    }

    string instruction;
    int instruction_address = 0;

    while (file >> instruction) {
        string h = instruction.substr(0, instruction.length() / 2);
        string h2 = instruction.substr(instruction.length() / 2);
        int first_value = stoi(h, nullptr, 16);
        int second_value = stoi(h2, nullptr, 16);
        memory.set(instruction_address, first_value);
        memory.set(instruction_address + 1, second_value);
        instructions.push_back(instruction);
        instruction_address += 2;
    }
    file.close();

    while (program_counter < instructions.size()) {
        cout << "Executing instruction: " << instructions[program_counter]
             << " (PC: " << program_counter << ")" << endl;

        execute_instruction(instructions[program_counter], registers, memory, program_counter);
        program_counter++;
    }

    return 0;
}
