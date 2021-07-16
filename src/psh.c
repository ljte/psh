#include "stdio.h"
#include "stdlib.h"
#include "string.h"

#define INC(var) (var)++
#define trimLF(s) (s)[strcspn((s), "\n")] = '\0'

void exitIfNull(const void *ptr, const char *errMsg) {
    if (ptr == NULL) {
    }
}

char *readLine(void) {
    size_t bufferSize = 0;
    char *buffer = NULL;
    if (getline(&buffer, &bufferSize, stdin) < 0) {
        perror("Failed to read line, please retry.");
    }
    trimLF(buffer);
    return buffer;
}

#define TOKEN_SIZE 64;
char **split(char *line, const char *delims) {
    size_t tokenSize = TOKEN_SIZE;
    char **tokens = (char **) malloc(tokenSize * sizeof(char *));

    uint position = 0;
    char *token = strtok(line, delims);
    while (token != NULL) {
        tokens[position] = token;
        INC(position);
        token = strtok(NULL, delims);
    }
    tokens[position] = NULL;
    return tokens;

}

#define SPLIT_DELIMS " \t\r\n\a"
#define PROMPT ">>> "
void lshLoop(void) {
    char **args;
    char *line;

    while (1) {
        printf(PROMPT);

        line = readLine();
        args = split(line, SPLIT_DELIMS);

        free(line);
        free(args);
    }
}

int main(int argc, char const *argv[]) {
    lshLoop();
    return EXIT_SUCCESS;
}
