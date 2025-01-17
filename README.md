The aim of this miniproject is to write a two pass assembler for an extended SIMPLE
instruction set. Then write and test programs in SIMPLE assembly. A final part is to write an
emulator for the SIMPLE machine (replacing the one provided).
Tasks
1. Write a two pass assembler for the assembly language. (75%)
The assembler must,
o Read assembly language from a text file, assigning label values and instruction
opcodes. The format of the assembly language is described.
o Diagnose common assembly errors such as unknown instruction, no such label,
duplicate label.
o Produce an object file of the produced machine code. This file should be a binary
file. Code starts at address zero.
o Produce a listing file. There is a choice of the format of the listing file. It can
either be a simple memory dump, or show the bytes produced for each instruction,
and that instruction's mnemonic. The formats are shown. (Extra marks are
available for the latter type of listing file)
o You must write the assembler in ISO C89. (`gcc -std=c89 -pedantic -W -Wall' is a
good way to check things.) or others are also fine.
2. Test your assembler with the sample programs listed.
3. Test your assembler with additional programs and submit evidence of this.
4. Write a bubble sort program in SIMPLE Assembler. The start of this file is provided, you
have to fill in the blanks.
5. Write an emulator for the SIMPLE machine. This should have some, but not need all, of
the functionality of the emu program provided. Up to 25%
Sample asm and emu (executable) are given.
Assembly Language
This assembly language is for a machine with four registers,
 Two registers, A & B, arranged as an internal stack.
 A program counter, PC 
 A stack pointer, SP
These registers are 32 bits in size. Instructions have either no operands or a single operand. The
operand is a signed 2's complement value. The encoding uses the bottom 8 bits for opcode and
the upper 24 bits for operand.
As with most assembly languages, this is line based (one statement per line). Comments begin
with a `;' and anything on the line after the `;' is ignored. Blank lines and lines containing only a
comment are permitted (and ignored). White space (` ' and tabs) are permitted at the beginning of
a line (and ignored). Label definitions consist of the label name followed by a `:', and an optional
statement (there is not necessarily a space between the `:' and the statement). A label use is just
the label name. For branch instructions label use should calculate the branch displacement. For
non-branch instructions, the label value should be used directly. A valid label name is an
alphanumeric string beginning with a letter . An operand is either a label or a number, the
number can be decimal, hex or octal.
The following are all permitted lines
; a comment
 ; another comment
label1: ; a label on its own
ldc 5 ; an instruction
label2: ldc 5 ; a label and an instruction
 adc 5 ; an instruction
label3:ldc label3 ;look no space between label and mnemonic
Each statement consists of a mnemonic (instruction name) and an optional operand (number or
label).
