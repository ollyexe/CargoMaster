#include <stdio.h>
#include "include/config_parser.h"
#include "include/test.h"

#define MAX_LINE_LENGTH 256





int main() {
    struct KeyValuePair config[14];
    int count = parseConfigFile("C:\\Users\\olly\\Desktop\\SO\\Progetto\\src\\resources\\prop.properties", config, 14);
    int i = 0;
    if (count < 0) {
        printf("Error parsing configuration file.\n");
        return 1;
    }


    for (i ; i < count; i++) {
        printf("%s = %s\n", config[i].key, config[i].value);
    }

    tst();
    return 0;
}




