#include "cpu_simulator.h"

Instruction history[MAX_HISTORY];  // Instruction history
int history_size = 0;

FILE *history_file;  // File pointer for history file

// CPU simulator functions
void reset_cpu(CPU *cpu) {
    memset(cpu->registers, 0, sizeof(cpu->registers));
    memset(cpu->memory, 0, sizeof(cpu->memory));
    cpu->ip = 0;
    cpu->running = 1;
    cpu->start = 0;
    history_size = 0;

    // Initialize history file
    history_file = fopen("history.txt", "w");
    if (history_file == NULL) {
        printf("Error opening history file.\n");
        return;
    }
    fprintf(history_file, "CPU Reset\n");
    fclose(history_file);
}

void log_state(CPU *cpu) {
    printf("CPU State:\n");
    for (int i = 0; i < NUM_REGS - 1; i++) {
        printf("R%d: %d, ", i, cpu->registers[i]);
    }
     printf("WSR: %d, IP: %d\n", cpu->registers[7], cpu->ip);

    printf("Memory: ");
    for (int i = 0; i < MEM_SIZE; i++) {
        if (cpu->memory[i] != 0) printf("[%d]: %d ", i, cpu->memory[i]);
    }
    printf("\n");
}

void save_instruction_to_file(Instruction instr) {
    history_file = fopen("history.txt", "a");
    if (history_file == NULL) {
        printf("Error opening history file.\n");
        return;
    }
    fprintf(history_file, "%s %s %s %s\n", instr.instruction, instr.args[0], instr.args[1], instr.args[2]);
    fclose(history_file);
}

void save_history(Instruction instr) {
    if (history_size < MAX_HISTORY) {
        history[history_size++] = instr;
    }
    save_instruction_to_file(instr);
}

/*void discard_instructions_from_file(int n) {
    if (n > history_size) n = history_size;

    FILE *temp_file = fopen("history_temp.txt", "w");
    if (temp_file == NULL) {
        printf("Error opening temp file.\n");
        return;
    }
    for (int i = 0; i < history_size - n; i++) {
        fprintf(temp_file, "%s %s %s %s\n", history[i].instruction, history[i].args[0], history[i].args[1], history[i].args[2]);
    }
    fclose(temp_file);
    remove("history.txt");
    rename("history_temp.txt", "history.txt");
}*/

void execute_DISC(CPU *cpu, Instruction instr) {
    int n = instr.args[0][0] != '\0' ? atoi(instr.args[0]) : 1;

    if (history_size >= n) {
        // Reduce the instruction history size by 'n' instructions
        history_size -= n;
        cpu->ip -= n;  // Adjust the instruction pointer

        // Open the history file and rewrite only the remaining instructions
        history_file = fopen("history.txt", "w");
        if (history_file == NULL) {
            printf("Error opening history file.\n");
            return;
        }

        // Write the remaining instructions back to the file
        for (int i = 0; i < history_size; i++) {
            fprintf(history_file, "%s %s %s %s\n", history[i].instruction, history[i].args[0], history[i].args[1], history[i].args[2]);
        }

        fclose(history_file);
        printf("Discarded %d instruction(s) from history.\n", n);
    } else {
        printf("Not enough instructions to discard.\n");
    }
}


// Command implementations
void execute_MOV(CPU *cpu, Instruction instr) {
    int reg = atoi(&instr.args[0][1]);
    if (reg >= NUM_REGS || reg < 0) {
        printf("Error: Invalid register index.\n");
        return;
    }
    int value = atoi(instr.args[1]);
    cpu->registers[reg] = value;
}

void execute_ADD(CPU *cpu, Instruction instr) {
    int dest = atoi(&instr.args[0][1]);
    int src1 = atoi(&instr.args[1][1]);
    int src2 = atoi(&instr.args[2][1]);
    if (dest >= NUM_REGS || src1 >= NUM_REGS || src2 >= NUM_REGS || dest < 0 || src1 < 0 || src2 < 0) {
        printf("Error: Invalid register index.\n");
        return;
    }
    cpu->registers[dest] = cpu->registers[src1] + cpu->registers[src2];
}

void execute_SUB(CPU *cpu, Instruction instr) {
    int dest = atoi(&instr.args[0][1]);
    int src1 = atoi(&instr.args[1][1]);
    int src2 = atoi(&instr.args[2][1]);
    if (dest >= NUM_REGS || src1 >= NUM_REGS || src2 >= NUM_REGS || dest < 0 || src1 < 0 || src2 < 0) {
        printf("Error: Invalid register index.\n");
        return;
    }
    cpu->registers[dest] = cpu->registers[src1] - cpu->registers[src2];
}

void execute_LOAD(CPU *cpu, Instruction instr) {
    int reg = atoi(&instr.args[0][1]);
    int address = atoi(instr.args[1]);
    if (reg >= NUM_REGS || reg < 0) {
        printf("Error: Invalid register index.\n");
        return;
    }
    if (address >= MEM_SIZE || address < 0) {
        printf("Error: Memory access out of bounds.\n");
        return;
    }
    cpu->registers[reg] = cpu->memory[address];
}

void execute_STORE(CPU *cpu, Instruction instr) {
    int reg = atoi(&instr.args[0][1]);
    int address = atoi(instr.args[1]);
    if (reg >= NUM_REGS || reg < 0) {
        printf("Error: Invalid register index.\n");
        return;
    }
    if (address >= MEM_SIZE || address < 0) {
        printf("Error: Memory access out of bounds.\n");
        return;
    }
    cpu->memory[address] = cpu->registers[reg];
}

void execute_START(CPU *cpu) {
    reset_cpu(cpu);
    cpu->registers[7] = 1;  // Set WSR to 1
}

void execute_EXIT(CPU *cpu) {
    if (cpu->registers[7] == 1) {
        cpu->registers[7] = 0;  // Reset WSR
    }
        printf("Terminating CPU Simulator...\n");
        cpu->running = 0;  // Terminate simulation
    }

void execute_LAYO(CPU *cpu) {
    log_state(cpu);
}

// Central instruction execution function
void execute_instruction(CPU *cpu, Instruction instr) {

    save_history(instr);

   // if (strcmp(instr.instruction, "START") == 0) {
     //   execute_START(cpu);

    if (strcmp(instr.instruction, "MOV") == 0) {
        execute_MOV(cpu, instr);

    } else if (strcmp(instr.instruction, "ADD") == 0) {
        execute_ADD(cpu, instr);

    } else if (strcmp(instr.instruction, "SUB") == 0) {
        execute_SUB(cpu, instr);

    } else if (strcmp(instr.instruction, "LOAD") == 0) {
        execute_LOAD(cpu, instr);

    } else if (strcmp(instr.instruction, "STORE") == 0) {
        execute_STORE(cpu, instr);

    } else if (strcmp(instr.instruction, "EXIT") == 0) {
        execute_EXIT(cpu);

    } else if (strcmp(instr.instruction, "DISC") == 0) {
        execute_DISC(cpu, instr);

    } else if (strcmp(instr.instruction, "LAYO") == 0) {
        execute_LAYO(cpu);

    } else {
        printf("Unknown instruction: %s\n", instr.instruction);
    }

    cpu->ip++;
}

// Parse input into an Instruction struct
void parse_input(char *input, Instruction *instr) {
    input[strcspn(input, "\n")] = '\0';  // Strip trailing newline
    sscanf(input, "%s %s %s %s", instr->instruction, instr->args[0], instr->args[1], instr->args[2]);
}

// Main CPU loop
void cpu_loop() {
    CPU cpu;

    char input[50];
    Instruction instr;
    int i;

    printf("Enter START to begin: ");
    while(cpu.start) {
        fgets(input, sizeof(input), stdin);
        parse_input(input, &instr);
        if (strcmp(instr.instruction, "START") == 0) {
             execute_START(&cpu);
            } else {
                printf("CPU not started yet.\n");
                printf("Please enter START to begin: ");
            }
    }

    while (cpu.running) {
        printf("Enter instruction: ");
        fgets(input, sizeof(input), stdin);
        parse_input(input, &instr);
        execute_instruction(&cpu, instr);
    }
}

