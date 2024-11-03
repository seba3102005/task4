
# CPU Simulator

## Overview

This CPU Simulator is designed to emulate a simple instruction set architecture (ISA) using C++. The simulator allows users to load instructions into memory and execute them, simulating basic operations such as loading and storing values in registers, arithmetic operations, and control flow with jumps.

## Features

- **Load and Store Instructions:** Transfer data between registers and memory.
- **Arithmetic Operations:** Perform addition using both two's complement and floating-point representations.
- **Control Flow:** Implement conditional jumps based on register comparisons.
- **Halt Execution:** Stop the program when needed.

## Instructions

The simulator supports the following instructions:

1. **Load from Memory:**
   - Format: `RXY`
   - Description: Load the register `R` with the bit pattern found in the memory cell at address `XY`.
   - Example: `14A3` loads the contents of memory cell `A3` into register `4`.

2. **Load Immediate Value:**
   - Format: `RXY`
   - Description: Load the register `R` with the immediate value `XY`.
   - Example: `20A3` loads the value `A3` directly into register `0`.

3. **Store to Memory:**
   - Format: `RXY`
   - Description: Store the bit pattern from register `R` into the memory cell at address `XY`.
   - Example: `35B1` stores the contents of register `5` into memory cell `B1`. If `R` is `0`, it writes to location `00`, which is mapped to the screen.

4. **Move Between Registers:**
   - Format: `0RS`
   - Description: Move the bit pattern from register `R` to register `S`.
   - Example: `40A4` copies the contents of register `A` into register `4`.

5. **Add Two's Complement:**
   - Format: `RST`
   - Description: Add the bit patterns in registers `S` and `T` as two's complement values and store the result in register `R`.
   - Example: `5726` adds the values in registers `2` and `6`, storing the sum in register `7`.

6. **Add Floating Point:**
   - Format: `RST`
   - Description: Add the values in registers `S` and `T` as floating-point numbers and store the result in register `R`.
   - Example: `634E` adds the values in registers `4` and `E` as floating-point numbers, storing the result in register `3`.

7. **Conditional Jump:**
   - Format: `BXY`
   - Description: Jump to instruction at address `XY` if the contents of register `R` equal the contents of register `0`. If not, continue sequential execution.
   - Example: `B43C` compares register `4` with register `0`. If equal, it jumps to instruction at memory address `3C`.

8. **Halt Execution:**
   - Format: `C000`
   - Description: Halt program execution.
   - Example: `C000` stops the program.




3. Follow the on-screen menu to load instructions, execute them, display the current state, or exit the program.

## Menu Options

- **1)** Load the instruction
- **2)** Execute the instruction
- **3)** Display the current status of registers and memory
- **4)** Exit the program

## Example

To execute a simple program:
1. Load the instructions into memory.
2. Execute the loaded instructions.
3. Display the current state of registers and memory after execution.

