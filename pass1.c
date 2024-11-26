#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#define MAX_SYMBOLS 100
#define MAX_LITERALS 100
#define MAX_POOLS 10
#define MAX_CODE_LINES 100
#define MAX_OPCODES 50
typedef struct
{ char
symbol[10];int
address;
int opcode;
} Symbol;
int symbol_opcode_counter = 1;
typedef struct
{ char
literal[10];
int address; int opcode;
} Literal;
typedef struct
{int
start_idx;
} Pool;
int search_symbol(char *symbol); // Declare search_symbol to avoid implicit declaration warnings

typedef struct {
char mnemonic[10];
char type[3];
int code;
} Opcode;
typedef struct
{char reg[5];
int code;
} Register;
Register regtab[4] = {
{"AREG", 1}, {"BREG", 2}, {"CREG", 3}, {"DREG", 4}
};
Symbol symtab[MAX_SYMBOLS];
int symtab_count = 0;
Literal littab[MAX_LITERALS];
int littab_count = 0;
Pool pooltab[MAX_POOLS];
int pooltab_count = 1;
char intermediate_code[MAX_CODE_LINES][50];
int intermediate_count = 0;
Opcode optab[MAX_OPCODES];
int optab_count = 0;
int location_counter = 0;
int search_register(char *reg)
{int i;
for (i = 0; i < 4; i++) {
printf("Comparing: '%s' with '%s'\n", reg, regtab[i].reg);
if (strcmp(regtab[i].reg, reg) == 0) {
return regtab[i].code;
}
}
return -1;
}
void add_symbol(char *symbol, int address) {
    int idx = search_symbol(symbol); // Call search_symbol to check if the symbol exists
    if (idx == -1) {
        // If symbol does not exist, add it to the symbol table
        strcpy(symtab[symtab_count].symbol, symbol);
        symtab[symtab_count].address = address;
        symtab[symtab_count].opcode = symbol_opcode_counter++;
        symtab_count++;
    } else {
        // If symbol already exists, update its address
        symtab[idx].address = address;
    }
}

void add_literal(char *literal)
{ strcpy(littab[littab_count].literal, literal);littab[littab_count].address = -1;
littab[littab_count].opcode = 1;
littab_count++;
}
int search_symbol(char *symbol)
{int i;
for (i = 0; i < symtab_count; i++) {
if (strcmp(symtab[i].symbol, symbol) == 0)
{return i;
}
}
return -1;
}
void load_opcode_table(char *filename)
{FILE *fp = fopen(filename, "r");
if (fp == NULL) {
printf("Error: Cannot open opcode table file\n");
exit(1);
}
char mnemonic[10], type[3];
int code;
while (fscanf(fp, "%s %s %d", mnemonic, type, &code) != EOF)
{strcpy(optab[optab_count].mnemonic, mnemonic);
strcpy(optab[optab_count].type, type);
optab[optab_count].code = code;
optab_count++;
}
fclose(fp);
}
Opcode* search_opcode(char *mnemonic)
{int i;
for (i = 0; i < optab_count; i++) {
if (strcmp(optab[i].mnemonic, mnemonic) == 0) {
    return &optab[i];
}
}
return NULL;
}
void trim_comma(char *str)
{int len = strlen(str);
if (len > 0 && str[len - 1] == ',')
{str[len - 1] = '\0';
}
}
void to_uppercase(char *str)
{int i = 0;
while (str[i]) {
if (str[i] >= 'a' && str[i] <= 'z') {
str[i] = str[i] - ('a' - 'A');
}
i++;
}
}
void process_line(char *line) {
char token1[10], token2[10], token3[10];
sscanf(line, "%s %s %s", token1, token2, token3);
int i;
trim_comma(token2);
to_uppercase(token2);
token2[strcspn(token2, "\n")] = 0;
token3[strcspn(token3, "\n")] = 0;
if (strcmp(token1, "START") == 0)
{location_counter = atoi(token2);
sprintf(intermediate_code[intermediate_count++], "(AD,01) (C,%02d)", location_counter);
} else if (strcmp(token1, "END") == 0) {
for (i = pooltab[pooltab_count - 1].start_idx; i < littab_count; i++)
{if (littab[i].address == -1) {
littab[i].address = location_counter;
sprintf(intermediate_code[intermediate_count++], "%02d (DL,02) (L,%d)", location_counter,i+ 1);
}
location_counter++;
}
for (i = 0; i < symtab_count; i++)
{if (symtab[i].address == -1) {
sprintf(intermediate_code[intermediate_count++], "%02d (DL,01) (S,%s)", location_counter,symtab[i].symbol);
location_counter++;
}
}
} else if (strcmp(token1, "LTORG") == 0) {
for (i = pooltab[pooltab_count - 1].start_idx; i < littab_count; i++)
{if (littab[i].address == -1) {
littab[i].address = location_counter;
sprintf(intermediate_code[intermediate_count++], "%02d (DL,02) (L,%d)", location_counter,i+ 1);
}
location_counter++;
}
pooltab[pooltab_count++].start_idx = littab_count;
} else if (strcmp(token2, "DC") == 0) {
    add_symbol(token1, location_counter);
sprintf(intermediate_code[intermediate_count++], "%02d (DL,01) (C,%s)", location_counter, token3);
location_counter++;
} else if (strcmp(token2, "DS") == 0)
{ add_symbol(token1, location_counter);int size =
atoi(token3);
sprintf(intermediate_code[intermediate_count++], "%02d (DL,02) (C,%d)", location_counter, size);location_counter += size;
} else {
Opcode *opcode = search_opcode(token1);
if (opcode != NULL) {
int reg_code = search_register(token2);
if (reg_code != -1) {
if (token3[0] == '=')
{ add_literal(token3);
sprintf(intermediate_code[intermediate_count++], "%02d (IS,%02d) (R,%d) (L,%d)", location_counter, opcode->code, reg_code, littab_count);
} else {
int symbol_idx = search_symbol(token3);
if (symbol_idx != -1) {
sprintf(intermediate_code[intermediate_count++], "%02d (IS,%02d) (R,%d) (S,%d)", location_counter, opcode->code, reg_code, symbol_idx);
} else {
printf("Error: Undefined symbol %s\n", token3);
}
}
location_counter++;
} else {
printf("Error: Undefined register %s\n", token2);
}
} else if (token1[0] == '=')
{add_literal(token1);
sprintf(intermediate_code[intermediate_count++], "%02d (DL,01) (C,%s)", location_counter, token1);
location_counter++;
} else {
printf("Error: Undefined operation %s\n", token1);
}
}
}
void display_tables(char *symbol_file, char *literal_file, char *pool_file, char *intermediate_file){FILE *symbol_output = fopen(symbol_file, "w");
FILE *literal_output = fopen(literal_file, "w");
FILE *pool_output = fopen(pool_file, "w");
FILE *intermediate_output = fopen(intermediate_file, "w");
int i;
if (symbol_output == NULL || literal_output == NULL || pool_output == NULL || intermediate_output== NULL) {
printf("Error: Cannot open one or more output files\n");
return;
}
fprintf(symbol_output, "Symbol Table:\n");
fprintf(symbol_output, "Opcode \tSymbol\tAddress\n");
for (i = 0; i < symtab_count; i++) {
fprintf(symbol_output, "%02d\t%s\t%d\n", symtab[i].opcode, symtab[i].symbol, symtab[i].address);}
fprintf(literal_output, "Literal Table:\n");
fprintf(literal_output, "Opcode \tLiteral\tAddress\n");
for (i = 0; i < littab_count; i++) {
fprintf(literal_output, "%02d\t%s\t%d\n", littab[i].opcode, littab[i].literal, littab[i].address); //Updated format
}
fprintf(pool_output, "Pool Table:\n");
for (i = 0; i < pooltab_count; i++) {
fprintf(pool_output, "%d\n", pooltab[i].start_idx);
}
fprintf(intermediate_output, "Intermediate Code:\n");
for (i = 0; i < intermediate_count; i++) {
fprintf(intermediate_output, "%s\n", intermediate_code[i]);
}
fclose(symbol_output);
fclose(literal_output);
fclose(pool_output);
fclose(intermediate_output);
}
int main(int argc, char *argv[]) {
char input_file[100] = "C:\\Users\\prana\\OneDrive\\Desktop\\assig2\\assig2\\asinput.txt";
char opcode_file[100] = "C:\\Users\\prana\\OneDrive\\Desktop\\assig2\\assig2\\asopcode_table.txt";
char symbol_file[100] = "C:\\Users\\prana\\OneDrive\\Desktop\\assig2\\assig2\\assymbol_table.txt";
char literal_file[100] = "C:\\Users\\prana\\OneDrive\\Desktop\\assig2\\assig2\\asliteral_table.txt";
char pool_file[100] = "C:\\Users\\prana\\OneDrive\\Desktop\\assig2\\assig2\\aspool_table.txt";
char intermediate_file[100] = "C:\\Users\\prana\\OneDrive\\Desktop\\assig2\\assig2\\asintermediate_code.txt";

if (argc >= 2)
{ strcpy(input_file, argv[1]);
}
if (argc >= 3)
{ strcpy(opcode_file, argv[2]);
}
if (argc >= 4)
{ strcpy(symbol_file, argv[3]);
}
if (argc >= 5)
{ strcpy(literal_file, argv[4]);
}
if (argc >= 6)
{ strcpy(pool_file, argv[5]);
}
if (argc >= 7)
{ strcpy(intermediate_file, argv[6]);
}
load_opcode_table(opcode_file);
FILE *input = fopen(input_file, "r");
if (input == NULL) {
printf("Error: Cannot open input file\n");
return 1;
}
char line[50];
while (fgets(line, sizeof(line), input) != NULL)
{process_line(line);
}
fclose(input);
display_tables(symbol_file, literal_file, pool_file, intermediate_file);
return 0;
}