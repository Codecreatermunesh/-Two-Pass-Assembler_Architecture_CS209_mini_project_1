

# Two-Pass Assembler and SIMPLE Emulator - CS209 Mini Project 1

## Project Overview
This project involves developing a two-pass assembler for the SIMPLE instruction set and an emulator for the SIMPLE machine. The key objectives are:
- Parsing and assembling SIMPLE assembly programs.
- Diagnosing and reporting assembly errors.
- Generating machine code and creating object and listing files.
- Writing and testing assembly programs, including a bubble sort.
- Implementing a functional emulator for the SIMPLE machine.
-My overall Report On this Project: https://github.com/Codecreatermunesh/-Two-Pass-Assembler_Architecture_CS209_mini_project_1/blob/master/MiniProjectCS209-CS210.pdf
## Features
- **Two-Pass Assembler**:
  - Reads assembly source code and assigns labels and opcodes.
  - Diagnoses errors like unknown instructions, missing labels, and duplicate labels.
  - Generates a binary object file and a listing file.
  - Supports multiple formats for the listing file (memory dump or detailed).
  - Compatible with ISO C89 standard.
- **SIMPLE Emulator**:
  - Simulates execution of assembled programs.
  - Operates on a machine with four 32-bit registers (A, B, PC, SP).
  - Supports stack-based operations and instruction execution.

## Instructions
### Requirements
- GCC or compatible compiler (ISO C89 compliant).
- Provided sample programs for testing.

### Assembler Usage
1. Compile the assembler:  
   ```bash
   gcc -std=c89 -pedantic -Wall -W -o assembler assembler.c
