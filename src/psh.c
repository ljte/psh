#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "unistd.h"
#include "sys/wait.h"
#include "limits.h"

#define INC(var) (var)++
#define trimLF(s) (s)[strcspn((s), "\n")] = '\0'
#define SIZE_OF_STR sizeof(char *)

char PREV_DIR[PATH_MAX];
void __chdir__(const char *const dir) {
    char temp[PATH_MAX];
    if (getcwd(temp, sizeof(temp)) == NULL) {
        perror("cd");
    }
    if (chdir(dir) != 0) {
        perror("cd");
    }
    strncpy(PREV_DIR, temp, PATH_MAX);
}

#define ROOT "/"
int cd(char **args) {
    if (args[1] == NULL) {
        __chdir__(ROOT);
    } else if (strcmp(args[1], "-") == 0) {
        __chdir__(PREV_DIR);
    } else {
        __chdir__(args[1]);
    }
    return 1;
}

int _exit_(char **args) {
    return 0;
}

const char *const builtins[] = {"cd", "exit"};
int (*builtinHandler[]) (char **) = {&cd, &_exit_};

#define NUMBER_OF_BUILTINS sizeof(builtins) / sizeof(char *)

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

int execBuiltin(char **args) {
    for (int i = 0; i < NUMBER_OF_BUILTINS; INC(i)) {
        if (strcmp(args[0], builtins[i]) == 0) {
            return builtinHandler[i](args);
        }
    }
    return -1;
}

int execCmd(char **args) {
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
    return 1;
}

int execute(char **args) {
    int code = execBuiltin(args);
    if (code != -1) {
        return code;
    }
    return execCmd(args);
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
        if (execute(args) == 0) {
            free(line);
            free(args);
            exit(EXIT_SUCCESS);
        }

        free(line);
        free(args);
    }
}

int main(int argc, char const *argv[]) {
    mainLoop();
    return EXIT_SUCCESS;
}
