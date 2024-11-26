#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define MAX_LINES 100
#define MAX_LINE_LENGTH 100
#define MAX_SYMBOLS 100

// Structure to store each entry of the relocation table
typedef struct {
    int original_address;
    int relocation_factor;
    int relocated_address;
    char instruction[MAX_LINE_LENGTH];
} RelocationEntry;

// Structure to store symbols with linked addresses and types
typedef struct {
    char name[MAX_LINE_LENGTH];
    int address;
    int linked_address; // Field for linked address
    char type[3]; // Field for type of symbol (PD or EX)
} Symbol;

void extract_symbols(char assembly_code[MAX_LINES][MAX_LINE_LENGTH], int line_count, Symbol symbol_table[MAX_SYMBOLS], int *symbol_count) {
    int address = 200; // Assuming the starting address for the program

    for (int i = 0; i < line_count; i++) {
        char *line = assembly_code[i];

        // Ignore empty lines and comments
        if (strlen(line) == 0 || line[0] == ';') {
            continue;
        }

        // Handle START directive
        if (strncmp(line, "START", 5) == 0) {
            sscanf(line, "START %d", &address);  // Update address with the start origin
            continue;
        }

        // Check for labels (symbols) with colons at the start of the line (e.g., ONE: DC 1)
        if (strchr(line, ':') && strchr(line, ':') - line > 0 && line[0] != ' ') {
            char *label_end = strchr(line, ':');
            int label_length = label_end - line;
            strncpy(symbol_table[*symbol_count].name, line, label_length);
            symbol_table[*symbol_count].name[label_length] = '\0'; // Null-terminate the string
            symbol_table[*symbol_count].address = address;

            // Determine the type of symbol
            strncpy(symbol_table[*symbol_count].type, "PD", 3);
            (*symbol_count)++;
        }
        // Check for data symbols (e.g., DS, DC) without colons
        else if (strstr(line, "DS") || strstr(line, "DC")) {
            char symbol_name[MAX_LINE_LENGTH];
            sscanf(line, "%s", symbol_name); // Get the symbol name
            strncpy(symbol_table[*symbol_count].name, symbol_name, MAX_LINE_LENGTH);
            symbol_table[*symbol_count].address = address;

            // Assume data symbols are PD (can modify as needed)
            strncpy(symbol_table[*symbol_count].type, "PD", 3);
            (*symbol_count)++;
        }

        // Increment address for the current instruction
        address++;
    }
}

void calculate_relocation(int link_origin, int start_origin, char assembly_code[MAX_LINES][MAX_LINE_LENGTH], int line_count, FILE *output_file) {
    // Calculate the relocation factor
    int relocation_factor = link_origin - start_origin;

    // Initialize variables
    int address = start_origin;
    RelocationEntry relocation_table[MAX_LINES];
    int entry_count = 0;

    // Process each line of assembly code
    for (int i = 0; i < line_count; i++) {
        char *line = assembly_code[i];

        // Ignore empty lines and comments
        if (strlen(line) == 0 || line[0] == ';') {
            continue;
        }

        // Handle START directive
        if (strncmp(line, "START", 5) == 0) {
            sscanf(line, "START %d", &address);  // Update address with the start origin
            continue;
        }

        // Store the original address and calculate the relocated address
        relocation_table[entry_count].original_address = address;
        relocation_table[entry_count].relocation_factor = relocation_factor;
        relocation_table[entry_count].relocated_address = address + relocation_factor;
        strncpy(relocation_table[entry_count].instruction, line, MAX_LINE_LENGTH);

        // Increment address for next instruction (assuming 1 address unit per instruction)
        entry_count++;
        address++;
    }

    // Print the relocation table to the output file
    fprintf(output_file, "%-20s %-20s %-20s %s\n", "Original Address", "Relocation Factor", "translated Address", "Instruction");
    fprintf(output_file, "%s\n", "--------------------------------------------------------------------------------");
    for (int j = 0; j < entry_count; j++) {
        fprintf(output_file, "%-20d %-20d %-20d %s\n", relocation_table[j].original_address,
               relocation_table[j].relocation_factor,
               relocation_table[j].relocated_address,
               relocation_table[j].instruction);
    }
}

int main() {
    int link_origin, start_origin;
    char assembly_code[MAX_LINES][MAX_LINE_LENGTH];
    Symbol symbol_table[MAX_SYMBOLS];
    int line_count = 0, symbol_count = 0;

    // Input the link origin and start origin
    printf("Enter the link origin: ");
    scanf("%d", &link_origin);
    printf("Enter the start origin: ");
    scanf("%d", &start_origin);
    getchar(); // To consume the newline character after entering start origin
    
    // Open the input file for reading
    FILE *file = fopen("input.txt", "r");
    if (file == NULL) {
        perror("Error opening file");
        return EXIT_FAILURE;
    }

    // Read the assembly code from the file
    while (line_count < MAX_LINES && fgets(assembly_code[line_count], MAX_LINE_LENGTH, file)) {
        // Remove newline character from the end of the line
        assembly_code[line_count][strcspn(assembly_code[line_count], "\n")] = 0;
        line_count++;
    }

    // Close the input file
    fclose(file);

    // Extract symbols
    extract_symbols(assembly_code, line_count, symbol_table, &symbol_count);

    // Calculate linked addresses for symbols
    for (int j = 0; j < symbol_count; j++) {
        symbol_table[j].linked_address = symbol_table[j].address + (link_origin - start_origin);
    }

    // Open the symbol output file for writing
    FILE *symbol_file = fopen("linktab.txt", "w");
    if (symbol_file == NULL) {
        perror("Error opening symbol output file");
        return EXIT_FAILURE;
    }

    // Write the symbol table to the symbol output file
    fprintf(symbol_file, "%-20s %-20s %-20s %-20s\n", "Symbol", "Address", "translated Address", "Type");
    fprintf(symbol_file, "%s\n", "---------------------------------------------------------------");
    for (int j = 0; j < symbol_count; j++) {
        fprintf(symbol_file, "%-20s %-20d %-20d %-20s\n", symbol_table[j].name, symbol_table[j].address, symbol_table[j].linked_address, symbol_table[j].type);
    }

    // Close the symbol output file
    fclose(symbol_file);

    // Open the relocation output file for writing
    FILE *output_file = fopen("relocationtab.txt", "w");
    if (output_file == NULL) {
        perror("Error opening output file");
        return EXIT_FAILURE;
    }

    // Calculate and write the relocation table to the output file
    calculate_relocation(link_origin, start_origin, assembly_code, line_count, output_file);

    // Close the output file
    fclose(output_file);

    return 0;  
}
