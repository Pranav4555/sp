#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_LINE_LENGTH 100

// Function to check if a line is empty
int is_empty_line(const char *line) {
    for (int i = 0; line[i] != '\0'; i++) {
        if (!isspace(line[i])) {
            return 0;
        }
    }
    return 1;
}

// Function to find the start address in input.txt
int find_start_address(FILE *input_file) {
    char line[MAX_LINE_LENGTH];
    int start_address = -1;

    while (fgets(line, MAX_LINE_LENGTH, input_file)) {
        if (strstr(line, "START") != NULL) {
            sscanf(line, "%*s %d", &start_address);
            break;
        }
    }
    return start_address;
}

// Function to count non-empty lines in om.txt
int count_code_lines(FILE *input_file) {
    char line[MAX_LINE_LENGTH];
    int count = 0;

    while (fgets(line, MAX_LINE_LENGTH, input_file)) {
        if (!is_empty_line(line)) {
            count++;
        }
    }
    return count;
}

// Function to read any relocation factor from the column in relocationtab.txt
int get_relocation_factor(const char *filename) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        perror("Error opening relocation table file");
        exit(EXIT_FAILURE);
    }
    
    char line[MAX_LINE_LENGTH];
    int relocation_factor = 0;
    
    // Skip the header line
    fgets(line, MAX_LINE_LENGTH, file);

    // Read the first valid relocation factor from the data rows
    while (fgets(line, MAX_LINE_LENGTH, file)) {
        if (sscanf(line, "%*d %d", &relocation_factor) == 1) {
            break;  // Stop after reading the first valid relocation factor
        }
    }

    printf("Relocation Factor: %d\n", relocation_factor);  // Print for confirmation

    fclose(file);
    return relocation_factor;
}

// Function to copy the relocation table from relocationtab.txt to object_module.txt
void append_relocation_table(const char *relocation_filename, FILE *output_file) {
    FILE *relocation_file = fopen(relocation_filename, "r");
    if (!relocation_file) {
        perror("Error opening relocation table file");
        exit(EXIT_FAILURE);
    }
    
    char line[MAX_LINE_LENGTH];
    
    fprintf(output_file, "\nRelocation Table:\n");
    // Copy each line from relocation table to the object module file
    while (fgets(line, MAX_LINE_LENGTH, relocation_file)) {
        fprintf(output_file, "%s", line);
    }

    fclose(relocation_file);
}

// Function to copy the link table from linktab.txt to object_module.txt
void append_link_table(const char *symbol_table_filename, FILE *output_file) {
    FILE *symbol_table_file = fopen(symbol_table_filename, "r");
    if (!symbol_table_file) {
        perror("Error opening symbol table file");
        exit(EXIT_FAILURE);
    }
    
    char line[MAX_LINE_LENGTH];
    
    fprintf(output_file, "\nLink Table:\n");
    // Copy each line from symbol table to the object module file
    while (fgets(line, MAX_LINE_LENGTH, symbol_table_file)) {
        fprintf(output_file, "%s", line);
    }

    fclose(symbol_table_file);
}

// Function to copy machine code from mc.txt to object_module.txt
void append_machine_code(const char *machine_code_filename, FILE *output_file) {
    FILE *machine_code_file = fopen(machine_code_filename, "r");
    if (!machine_code_file) {
        perror("Error opening machine code file");
        exit(EXIT_FAILURE);
    }
    
    char line[MAX_LINE_LENGTH];
    
    fprintf(output_file, "\nMachine Code:\n");
    // Copy each line from the machine code file to the object module file
    while (fgets(line, MAX_LINE_LENGTH, machine_code_file)) {
        fprintf(output_file, "%s", line);
    }

    fclose(machine_code_file);
}

int main() {
    // File pointers
    FILE *input_file = fopen("input.txt", "r");
    FILE *output_file = fopen("object_module.txt", "w");
    if (!input_file || !output_file) {
        perror("Error opening files");
        exit(EXIT_FAILURE);
    }

    // Step 1: Find the translated start address
    int start_address = find_start_address(input_file);
    if (start_address == -1) {
        fprintf(stderr, "START address not found in om.txt\n");
        exit(EXIT_FAILURE);
    }

    // Step 2: Calculate the code size (number of non-empty lines)
    rewind(input_file);  // Reset file pointer to start counting lines
    int code_size = count_code_lines(input_file);

    // Step 3: Get the relocation factor and calculate final start address
    int relocation_factor = get_relocation_factor("relocationtab.txt");
    int adjusted_start_address = start_address + relocation_factor;

    // Step 4: Write the header to object_module.txt
    fprintf(output_file, "Header:\n");
    fprintf(output_file, "Translated Address: %d\n", start_address);
    fprintf(output_file, "Code Size: %d\n", code_size);
    fprintf(output_file, "Start Address: %d\n", adjusted_start_address);

    // Step 5: Append the machine code to object_module.txt
    append_machine_code("mc.txt", output_file);

    // Step 6: Append the relocation table to object_module.txt
    append_relocation_table("relocationtab.txt", output_file);

    // Step 7: Append the link table (symbol table) to object_module.txt
    append_link_table("linktab.txt", output_file);

    // Close files
    fclose(input_file);
    fclose(output_file);

    printf("Object module with header, machine code, relocation table, and link table created successfully in object_module.txt.\n");
    return 0;
}

