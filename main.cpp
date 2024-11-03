#include "cpu_simulator.h"
#include <iostream>
#include <fstream>
#include <string>
#include <vector>

using namespace std;

int program_counter = 0;

void execute_instruction(const string &instruction, Registers &registers, Memory &memory, int &program_counter) {
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
        case '6': {
            int value = alu2.floatToBinary(registers, R, X, Y);
            registers.set(R, value);
            break;
        }
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
            cu2.jump_if_equal(registers, program_counter, R, XY, memory);
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
}


int main() {
    Registers registers(16);
    Memory memory(256);
    vector<string> instructions;
    bool instructionsLoaded = false;

    while (true) {
        bool flag=false;
        cout << "\n--- CPU Simulator Menu ---\n";
        cout << "1. Load instructions from file\n";
        cout << "2. Execute program\n";
        cout << "3. Display registers and memory\n";
        cout << "4. Exit\n";
        cout << "Choose an option: ";
        int choice;
        cin >> choice;

        if (choice == 1) {
            cout << "Enter the filename of instructions: ";
            string filename;
            cin >> filename;
            ifstream file(filename);

            if (!file) {
                cerr << "Error opening file." << endl;
                continue;
            }

            instructions.clear();
            program_counter = 0;
            int instruction_address = 0;
            string instruction;

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
            instructionsLoaded = true;
            cout << "Instructions loaded successfully.\n";
        }
        else if (choice == 2) {
            if (!instructionsLoaded) {
                cout << "Please load instructions first.\n";
                continue;
            }
            while (program_counter < instructions.size()) {

                if(!flag)
                {
                    cout << "Executing instruction: " << instructions[program_counter]
                      << " (PC: " << program_counter << ")" << endl;
                    execute_instruction(instructions[program_counter], registers, memory, program_counter);
                }


                if(instructions[program_counter][0]=='C')
                {
                    flag=true;
                }
                program_counter++;
            }
            cout << "Program executed successfully.\n";
        }
        else if (choice == 3) {
            cout << "\n--- Registers ---\n";
            registers.print();
            cout << "\n--- Memory ---\n";
            memory.print();
        }
        else if (choice == 4) {
            cout << "Exiting program.\n";
            break;
        }
        else {
            cout << "Invalid choice. Please select a valid option.\n";
        }
    }

    return 0;
}
