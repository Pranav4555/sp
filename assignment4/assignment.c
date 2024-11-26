#include <stdio.h>
#include <string.h>

#define MAX 100

struct Symbol {
    char name[10];
    int address;
} symbol_table[] = {
    {"BETA", 103},
    {"GAMMA", 104}
};

struct Literal {
    char literal[5];
    int address;
} literal_table[] = {
    {"='5'", 105},
    {"='1'", 107}
};

struct Pool {
    int id;
} pool_table[] = {
    {1}
};

struct IntermediateCode {
    int lc;
    char type[3];
    int opcode;
    char op_type[2];
    int op_id;
};
 struct IntermediateCode intermediate_code[] = {
    //{Location Counter (LC), Instruction Type, Opcode, Operand Type, Operand Value}
    {100, "IS", 1, "L", 1},  
    {101, "IS", 3, "S", 1},  // BREG, BETA
    {102, "IS", 2, "S", 2},  //  BREG, GAMMA
      {103, "IS", 3, "L", 2}   // ='5'
    

};


int search_symbol_table(int id) {
    if (id > 0 && id <= sizeof(symbol_table) / sizeof(symbol_table[0])) {
        return symbol_table[id - 1].address;
    }
    return -1;
}

int search_literal_table(int id) {
    if (id > 0 && id <= sizeof(literal_table) / sizeof(literal_table[0])) {
        return literal_table[id - 1].address;
    }
    return -1;
}

void generate_machine_code() {
    int location_counter = 100;

    printf("Machine Code:\n");
    for (int i = 0; i < sizeof(intermediate_code) / sizeof(intermediate_code[0]); i++) {
        int address = -1;

        if (strcmp(intermediate_code[i].type, "IS") == 0) {
            int opcode = intermediate_code[i].opcode;
            int reg = intermediate_code[i].op_type[0] == 'S' ? 1 : 0;

            if (intermediate_code[i].op_id != 0) {
                if (strcmp(intermediate_code[i].op_type, "S") == 0) {
                    address = search_symbol_table(intermediate_code[i].op_id);
                } else if (strcmp(intermediate_code[i].op_type, "L") == 0) {
                    address = search_literal_table(intermediate_code[i].op_id);
                } else if (strcmp(intermediate_code[i].op_type, "C") == 0) {
                    address = intermediate_code[i].op_id;
                }
            }

            if (address != -1) {
                printf("%03d %02d %02d %03d\n", location_counter, opcode, reg, address);
            } else if (intermediate_code[i].op_id != 0) {
                printf("Error: Address not found for ID %d\n", intermediate_code[i].op_id);
            }
            location_counter++;
        } else if (strcmp(intermediate_code[i].type, "AD") == 0) {
            if (intermediate_code[i].opcode == 1 && intermediate_code[i].op_id != 0) {
                location_counter = intermediate_code[i].op_id;
            }
        }
    }

    printf("\nPool Table:\n");
    for (int i = 0; i < sizeof(pool_table) / sizeof(pool_table[0]); i++) {
        printf("Literal ID: %d\n", pool_table[i].id);
    }
}

int main() {
    generate_machine_code();
    return 0;
}
