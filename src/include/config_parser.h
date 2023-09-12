#ifndef CONFIG_PARSER_H
#define CONFIG_PARSER_H

#define MAX_LINE_LENGTH 256

struct KeyValuePair {
    char key[MAX_LINE_LENGTH];
    char value[MAX_LINE_LENGTH];
};

int parseConfigFile(const char *filename, struct KeyValuePair *config, int max_entries);

#endif