#include <stdio.h> 
#include <stdlib.h> 
#include <string.h>

typedef struct { int opcode; int address;
} TableEntry;


int load_literal_table(const char *filename, TableEntry **table) { FILE *file = fopen(filename, "r");
if (!file) {
printf("Error: Could not open literal table file %s\n", filename); return 0;
}


int count = 0; char line[100];
while (fgets(line, sizeof(line), file) != NULL) { count++;
}


rewind(file);


*table = (TableEntry *)malloc(count * sizeof(TableEntry)); int index = 0;
while (fgets(line, sizeof(line), file) != NULL) { int opcode, address;
char literal[10];
if (sscanf(line, "%d %*s %d", &opcode, &address) == 2) {

(*table)[index].opcode = opcode; (*table)[index].address = address; index++;
}
}


fclose(file); return count;
}


int load_symbol_table(const char *filename, TableEntry **table) { FILE *file = fopen(filename, "r");
if (!file) {
printf("Error: Could not open symbol table file %s\n", filename); return 0;
}


int count = 0; char line[100];
while (fgets(line, sizeof(line), file) != NULL) { count++;
}


rewind(file);


*table = (TableEntry *)malloc(count * sizeof(TableEntry)); int index = 0;
while (fgets(line, sizeof(line), file) != NULL) { int opcode, address;
char symbol[20];
if (sscanf(line, "%d %*s %d", &opcode, &address) == 2) { (*table)[index].opcode = opcode;

(*table)[index].address = address; index++;
}
}


fclose(file); return count;
}


int get_address(TableEntry *table, int size, int opcode) { int i;
for (i = 0; i < size; i++) {
if (table[i].opcode == opcode) { return table[i].address;
}
}
return -1;
}


void generate_machine_code(char *intermediate_file, char *machine_code_file, TableEntry
*literal_table, int literal_count, TableEntry *symbol_table, int symbol_count) { FILE *intermediate_input = fopen(intermediate_file, "r");
FILE *machine_code_output = fopen(machine_code_file, "w");


if (intermediate_input == NULL || machine_code_output == NULL) { printf("Error: Cannot open intermediate or machine code file\n"); return;
}


char line[100]; int lc = 0;

while (fgets(line, sizeof(line), intermediate_input) != NULL) {

int opcode, reg = -1, operand = -1, address = -1; char type1[5], type2[5], reg_type[5];

if (sscanf(line, "(AD,%d) (C,%d)", &opcode, &lc) == 2) { fprintf(machine_code_output, "01 - %d\n", lc); continue;
}


if (sscanf(line, "%d (DL,%d) (C,'%d')", &lc, &opcode, &operand) == 3) { fprintf(machine_code_output, "%03d %02d - %d\n", lc, opcode, operand); continue;
}


if (sscanf(line, "%d (DL,%d) (C,%d)", &lc, &opcode, &operand) == 3) { fprintf(machine_code_output, "%03d %02d - %d\n", lc, opcode, operand); continue;
}


if (sscanf(line, "%d (DL,%d) (L,%d)", &lc, &opcode, &operand) == 3) { address = get_address(literal_table, literal_count, operand);
if (address != -1) {
fprintf(machine_code_output, "%03d %02d - %d\n", lc, opcode, address);
}
continue;
}


if (sscanf(line, "%d (IS,%d) (R,%d) (%[^,],%d)", &lc, &opcode, &reg, type2, &operand) == 5) { if (strcmp(type2, "L") == 0) {
address = get_address(literal_table, literal_count, operand);
} else if (strcmp(type2, "S") == 0) {
address = get_address(symbol_table, symbol_count, operand);
}

if (address != -1) {
fprintf(machine_code_output, "%03d %02d %01d %02d\n", lc, opcode, reg, address);
}
continue;
}


if (sscanf(line, "%d (IS,%d) (%[^,],%d)", &lc, &opcode, type2, &operand) == 4) { if (strcmp(type2, "L") == 0) {
address = get_address(literal_table, literal_count, operand);
} else if (strcmp(type2, "S") == 0) {
address = get_address(symbol_table, symbol_count, operand);
}
if (address != -1) {
fprintf(machine_code_output, "%03d %02d - %02d\n", lc, opcode, address);
}
continue;
}


if (sscanf(line, "%d (AD,%d) %d", &lc, &opcode, &operand) == 3) { fprintf(machine_code_output, "%03d %02d %03d\n", lc, opcode, operand); continue;
}


printf("Warning: Unable to parse line: %s", line);
}


fclose(intermediate_input); fclose(machine_code_output);
}


int main(int argc, char *argv[]) {
char intermediate_file[100] = "asintermediate_code.txt";

char machine_code_file[100] = "asmachine_code.txt";


TableEntry *literal_table = NULL; TableEntry *symbol_table = NULL;
int literal_count = load_literal_table("asliteral_table.txt", &literal_table);
int symbol_count = load_symbol_table("assymbol_table.txt", &symbol_table);


if (argc >= 2) { strcpy(intermediate_file, argv[1]);
}
if (argc >= 3) { strcpy(machine_code_file, argv[2]);
}


generate_machine_code(intermediate_file, machine_code_file, literal_table, literal_count, symbol_table, symbol_count);


free(literal_table); free(symbol_table);

printf("Machine code generated successfully and saved to %s\n", machine_code_file);


return 0;
}
