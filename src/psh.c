#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "unistd.h"
#include "sys/wait.h"

#define INC(var) (var)++
#define trimLF(s) (s)[strcspn((s), "\n")] = '\0'
#define SIZE_OF_STR sizeof(char *)

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
    if (strcmp(buffer, "\n") != 0) {
        trimLF(buffer);
    }
    return buffer;
}

#define TOKEN_SIZE 64;
char **split(char *line, const char *delims) {
    size_t tokenSize = TOKEN_SIZE;
    char **tokens = (char **) malloc(tokenSize * SIZE_OF_STR);

    uint position = 0;
    char *token = strtok(line, delims);
    while (token != NULL) {
        tokens[position] = token;
        INC(position);

        if (position >= tokenSize) {
            tokenSize += TOKEN_SIZE;
            tokens = realloc(tokens, tokenSize * SIZE_OF_STR);
            if (tokens == NULL) {
                perror("Failed to parse arguments.");
                exit(EXIT_FAILURE);
            }
        }
        token = strtok(NULL, delims);
    }
    tokens[position] = NULL;
    return tokens;

}

void execute(char **const args) {
    int status = 0;
    const pid_t pid = fork();

    if (pid == 0) {
       if (execvp(args[0], args) == -1) {
            printf("Unknown command: %s\n", args[0]);
       }
       exit(EXIT_FAILURE);
    } else if (pid < 0) {
        perror("Error");
    } else {
        do {
            waitpid(pid, &status, WUNTRACED);
        } while (!WIFEXITED(status) && !WIFSIGNALED(status));
    }
}

#define SPLIT_DELIMS " \t\r\n\a"
#define PROMPT ">>> "
void mainLoop(void) {
    char **args;
    char *line;

    while (1) {
        printf(PROMPT);

        line = readLine();
        if (strcmp(line, "\n") == 0) {
            continue;
        }
        args = split(line, SPLIT_DELIMS);
        execute(args);

        free(line);
        free(args);
    }
}

int main(int argc, char const *argv[]) {
    mainLoop();
    return EXIT_SUCCESS;
}
