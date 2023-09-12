#include "../include/config_parser.h"
#include <stdio.h>
#include <string.h>

int parseConfigFile(const char *filename, struct KeyValuePair *config, int max_entries) {
    int count = 0;
    char line[MAX_LINE_LENGTH];
    char *separator;

    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        perror("Failed to open file");
        return -1;
    }

    while (fgets(line, sizeof(line), file) != NULL && count < max_entries) {
        if (line[0] == '#' || line[0] == '\n') {
            continue;
        }

        separator = strchr(line, '=');
        if (separator != NULL) {
            *separator = '\0';
            strncpy(config[count].key, line, sizeof(config[count].key));
            strncpy(config[count].value, separator + 1, sizeof(config[count].value));
            count++;
        }
    }

    fclose(file);
    return count;
}