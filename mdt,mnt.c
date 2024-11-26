#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LINE_LENGTH 256
#define MAX_LINES 1000
#define MAX_MDT_LINES 100
#define MAX_MNT_LINES 100

void writeMDTFile(char mdt[MAX_MDT_LINES][MAX_LINE_LENGTH], int mdtCount) {
    FILE *outFile = fopen("MDT.txt", "w");
    if (outFile == NULL) {
        printf("Unable to open MDT file for writing.\n");
        return;
    }
    for (int i = 0; i < mdtCount; i++) {
        fprintf(outFile, "%s\n", mdt[i]); // Ensure each line ends with a newline character
    }
    fclose(outFile);
    printf("MDT written to onlyMDT.txt\n");
}

void writeMNTFile(char mnt[MAX_MNT_LINES][MAX_LINE_LENGTH], int mntCount) {
    FILE *outFile = fopen("MNT.txt", "w");
    if (outFile == NULL) {
        printf("Unable to open MNT file for writing.\n");
        return;
    }
    for (int i = 0; i < mntCount; i++) {
        fprintf(outFile, "%s\n", mnt[i]); // Ensure each line ends with a newline character
    }
    fclose(outFile);
    printf("MNT written to onlyMNT.txt\n");
}

void replaceParameters(char *line, char params[][MAX_LINE_LENGTH], int paramCount) {
    for (int i = 0; i < paramCount; i++) {
        char param[MAX_LINE_LENGTH];
        snprintf(param, MAX_LINE_LENGTH, "&%s", params[i]); // Parameter format: &X, &Y
        char placeholder[MAX_LINE_LENGTH];
        snprintf(placeholder, MAX_LINE_LENGTH, "#%d", i + 1); // Positional format: #1, #2

        char *pos;
        while ((pos = strstr(line, param)) != NULL) {
            char temp[MAX_LINE_LENGTH];
            strncpy(temp, line, pos - line);
            temp[pos - line] = '\0';
            strcat(temp, placeholder);
            strcat(temp, pos + strlen(param));
            strcpy(line, temp);
        }
    }
}

void processMacros(const char *inputFileName) {
    FILE *inFile = fopen(inputFileName, "r");
    if (inFile == NULL) {
        printf("Unable to open input file.\n");
        return;
    }

    char lines[MAX_LINES][MAX_LINE_LENGTH];
    int lineCount = 0;

    // Read the entire file into the 2D array
    while (fgets(lines[lineCount], MAX_LINE_LENGTH, inFile)) {
        lineCount++;
        if (lineCount >= MAX_LINES) {
            printf("File too large to process.\n");
            fclose(inFile);
            return;
        }
    }
    fclose(inFile);

    char mdt[MAX_MDT_LINES][MAX_LINE_LENGTH];
    char mnt[MAX_MNT_LINES][MAX_LINE_LENGTH];

    int mdtIndex = 1;
    int mdtCount = 0;
    int mntCount = 0;

    for (int i = 0; i < lineCount; i++) {
        if (strstr(lines[i], "MACRO") != NULL) {
            // Extract macro name and add to MNT
            if (i + 1 < lineCount) {
                char macroName[MAX_LINE_LENGTH];
                char params[MAX_MNT_LINES][MAX_LINE_LENGTH];
                int paramCount = 0;

                // Parse macro name and parameters
                sscanf(lines[i + 1], "%s", macroName);
                char *token = strtok(lines[i + 1] + strlen(macroName), ", ");
                while (token != NULL) {
                    strncpy(params[paramCount++], token + 1, MAX_LINE_LENGTH); // Skip '&'
                    token = strtok(NULL, ", ");
                }

                // Add macro name to MNT
                snprintf(mnt[mntCount++], MAX_LINE_LENGTH, "%d   %s", mdtIndex, macroName);

                // Add macro header to MDT
                snprintf(mdt[mdtCount++], MAX_LINE_LENGTH, "%d   %s", mdtIndex++, lines[i + 1]);

                i += 2; // Skip MACRO and macro name lines

                // Process macro body until MEND
                while (i < lineCount && strstr(lines[i], "MEND") == NULL) {
                    char processedLine[MAX_LINE_LENGTH];
                    strncpy(processedLine, lines[i], MAX_LINE_LENGTH);
                    replaceParameters(processedLine, params, paramCount);

                    // Add processed line to MDT
                    snprintf(mdt[mdtCount++], MAX_LINE_LENGTH, "%d   %s", mdtIndex++, processedLine);
                    i++;
                }

                if (i < lineCount && strstr(lines[i], "MEND") != NULL) {
                    snprintf(mdt[mdtCount++], MAX_LINE_LENGTH, "%d   %s", mdtIndex++, lines[i]);
                }
            }
        }
    }

    writeMNTFile(mnt, mntCount);
    writeMDTFile(mdt, mdtCount);
}

int main() {
    const char *inputFileName = "2macro.txt";
    processMacros(inputFileName);
    return 0;
}
