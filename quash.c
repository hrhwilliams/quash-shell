#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <unistd.h>
#include <signal.h>
#include <sys/types.h>

#include <readline/readline.h>
#include <readline/history.h>

#include "tokenizer.h"

typedef struct _Process {
    char **p_argv;
    char *input_fp;
    char *output_fp;
    int p_argc;
    int input_fd;
    int output_fd;
    int append;
} Process;

typedef struct _Job {
    Process *processes;
} Job;

// command < file.txt > out.txt 2> error.txt
/*
 * WordList := WORD
 *           | WORD WordList
 *
 * Command := WordList
 *          | WordList RedirectionList
 *
 * RedirectionList := Redirection*
 *
 * Redirection := '>' WORD        -> redirect STDOUT to FILENAME
 *              | '<' WORD        -> redirect FILENAME to STDIN
 *              | NUMBER '>' WORD -> redirect FD to FILENAME
 *              | NUMBER '<' WORD -> redirect FILENAME to FD
 *              | '>>' WORD -> redirect FD to FILENAME, appending
 *              | NUMBER '>>' WORD -> redirect FD to FILENAME, appending
 *              | '<>' WORD -> redirect FILENAME to SDTIN, and STDOUT to FILENAME
 *              | '&>' WORD -> redirect STDOUT and STDERR to FILENAME
 *              | '&>>' WORD -> redirect STDOUT and STDERR to FILENAME, appending
 *              | '>&' WORD -> redirect STDOUT and STDERR to FILENAME
 *              | '>>&' WORD -> redirect STDOUT and STDERR to FILENAME, appending
 *              | '>&' NUMBER -> redirect STDOUT to FD
 *              | NUMBER '>&' NUMBER -> redirect $1 to $2
 * 
 * Pipe := Command '|' Command -> pipe stdout of $1 into stdin of $2
 *       | Command '|&' Command -> pipe stdout and stderr of $1 into stdin of $2
 *       | Command
 * 
 * Conditional := Pipe
 *              | Pipe '&&' Conditional  -> only execute right-hand side if left-hand side returns 0, and returns 0
 *              | Pipe '||' Conditional  -> execute right-hand side if left-hand side returns nonzero, and returns 1
 */

void handle_sigint(int n) {

}

void init_signal_handlers() {
    struct sigaction sa;
    sa.sa_handler = handle_sigint;
    sigaction(SIGINT, &sa, NULL);
}

void newline() {
    puts("");
}

int interactive_prompt() {
    const char *prompt = "$ ";
    char *line;
    TokenDynamicArray tokens;

    for (;;) {
        line = readline(prompt);

        if (line == NULL) {
            newline();
            break;
        } else if (*line == '\0') {
            continue;
        }

        add_history(line);

        tokens = tokenize(line);

        for (size_t i = 0; i < tokens.length; i++) {
            print_token(tokens.tuples[i]);
            printf(" ");
        }
        printf("\n");

        free(line);
        free_token_array(&tokens);  
    }

    return 0;
}

int main() {
    init_signal_handlers();
    interactive_prompt();

    return 0;
}
