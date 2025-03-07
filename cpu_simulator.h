#ifndef CPU_SIMULATOR_H
#define CPU_SIMULATOR_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define NUM_REGS 8
#define MEM_SIZE 256
#define MAX_HISTORY 1000

typedef struct {
    char instruction[10];
    char args[3][10];
} Instruction;

typedef struct {
    int registers[NUM_REGS];  // R0-R6 + WSR
    int memory[MEM_SIZE];
    int ip;  // Instruction pointer
    int running;  // CPU state
    int start;
} CPU;

// Function prototypes
void reset_cpu(CPU *cpu);
void log_state(CPU *cpu);
void save_history(Instruction instr);
void save_instruction_to_file(Instruction instr);
void discard_instructions_from_file(int n);
void execute_DISC(CPU *cpu, Instruction instr);
void execute_instruction(CPU *cpu, Instruction instr);
void parse_input(char *input, Instruction *instr);
void execute_MOV(CPU *cpu, Instruction instr);
void execute_ADD(CPU *cpu, Instruction instr);
void execute_SUB(CPU *cpu, Instruction instr);
void execute_LOAD(CPU *cpu, Instruction instr);
void execute_STORE(CPU *cpu, Instruction instr);
void execute_START(CPU *cpu);
void execute_EXIT(CPU *cpu);
void execute_LAYO(CPU *cpu);
void cpu_loop();

#endif // CPU_SIMULATOR_H
