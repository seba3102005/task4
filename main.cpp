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
        case '6':
        {

            int value=alu2.floatToBinary(registers, R, X, Y) ;
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
        {
            cu2.jump_if_equal(registers,program_counter, R, XY,memory);
            break;
        }
        case 'C':
            cu2.halt();
            break;
        case 'D':
            cu2.jump_if_greater(registers, program_counter, R, XY);
            break;


        default:
            cout << "Unknown instruction: " << instruction << endl;
    }


    cout<<"executed"<<endl;

}

int main() {

    while(true)
    {
        Registers registers(16);
        Memory memory(256);
        vector<string> instructions;

        cout<<"1)Load the instruction\n2)Execute the instruction\n3)display the status\n4)Exit the program"<<endl;
        string choice;
        cin>>choice;
        while(choice!="1" && choice!="2" && choice!="3" && choice!="4")
        {
            cout<<"invalid choice ,please enter a valid choice\n";
            cin>>choice;

        }

        ifstream file1;
        if(choice=="1")
        {
            string file_name;
            cout<<"enter the file's name without any extensions"<<endl;

            cin>>file_name;
            file_name = file_name.append(".txt");
            cerr<<file_name<<endl;
            file_name.append(".txt");
            while (file1.is_open())
            {

                cerr << "Error opening file." << endl;
                cout<<"please enter an existing file name"<<endl;
                cin>>file_name;
                file_name.append(".txt");
                file1.open(file_name);

            }
        }
        else if(choice =="2")
        {
            string instruction;
            int instruction_address = 0;

            while (file1 >> instruction)
            {
                string h = instruction.substr(0, instruction.length() / 2);
                string h2 = instruction.substr(instruction.length() / 2);
                int first_value = stoi(h, nullptr, 16);
                int second_value = stoi(h2, nullptr, 16);
                memory.set(instruction_address, first_value);
                memory.set(instruction_address + 1, second_value);
                instructions.push_back(instruction);
                instruction_address += 2;
            }
           file1.close();

            while (program_counter < instructions.size()) {
                cout << "Executing instruction: " << instructions[program_counter]
                     << " (PC: " << program_counter << ")" << endl;

                execute_instruction(instructions[program_counter], registers, memory, program_counter);

                program_counter++;
            }

        }
        else if(choice=="3")
        {
            cout<<"After Execution1"<<endl;
            registers.print();
            memory.print();
        }
        else if(choice =="4")
        {
            cout<<"Exitting the program"<<endl;
            break;
        }






    }

    return 0;
}
