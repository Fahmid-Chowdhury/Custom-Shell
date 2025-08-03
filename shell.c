#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <signal.h>
#include <sys/stat.h>

#define MAX_INPUT 1024
#define MAX_ARGS 100
#define HISTORY_SIZE 100
 
char* history[HISTORY_SIZE];
int history_count = 0;
 
void handle_sigint(int sig) {
    printf("\nUse 'exit' to quit the shell.\n");
}
 
void add_to_history(const char* cmd) {
    if (history_count < HISTORY_SIZE) {
        history[history_count++] = strdup(cmd);
    }
}
 
// Parse a single command segment
void parse_single_command(char *input, char **args, char **in_file, char **out_file, int *append) {
    *in_file = NULL;
    *out_file = NULL;
    *append = 0;
    int i = 0;

    while (*input != '\0') {
    
        while (*input == ' ' || *input == '\t') input++;
        if (*input == '\0') break;

        if (*input == '>') {
            input++;
            if (*input == '>') {
                *append = 1;
                input++;
            }
            while (*input == ' ' || *input == '\t') input++;
            *out_file = input;
            while (*input && *input != ' ' && *input != '\t') input++;
            if (*input) *input++ = '\0';
            continue;
        }

        if (*input == '<') {
            input++;
            while (*input == ' ' || *input == '\t') input++;
            *in_file = input;
            while (*input && *input != ' ' && *input != '\t') input++;
            if (*input) *input++ = '\0';
            continue;
        }

        if (*input == '"' || *input == '\'') {
            char quote = *input++;
            args[i++] = input;
            while (*input && *input != quote) input++;
            if (*input) *input++ = '\0';
        } else {
            args[i++] = input;
        //  printf("--%s\n", input);
            while (*input && *input != ' ' && *input != '\t' && *input != '<' && *input != '>' && *input != '"' && *input != '\'') input++;
            if (*input) *input++ = '\0';
        }
    }
    args[i] = NULL;

    //  print args : debug
    // for (int j = 0; j < i; j++) {
    //     printf("%d: %s\n", j, args[j]);
    // }
    
}

int execute_pipeline(char *cmd) {
    char *pipes[10];
    int num_pipes = 0;
    pipes[num_pipes++] = strtok(cmd, "|");
    while ((pipes[num_pipes++] = strtok(NULL, "|")));
    num_pipes--;

    int pipefd[2], prevfd = -1;
    int status = 0;

    for (int i = 0; i < num_pipes; ++i) {
        char *args[MAX_ARGS], *in_file = NULL, *out_file = NULL;
        int append = 0;
        parse_single_command(pipes[i], args, &in_file, &out_file, &append);

        // Graceful cd handling
        if (args[0] && strcmp(args[0], "cd") == 0) {
            if (args[1] == NULL) {
                fprintf(stderr, "cd: missing argument\n");
                return 0;
            }
            struct stat st;
            if (stat(args[1], &st) == -1 || !S_ISDIR(st.st_mode)) {
                fprintf(stderr, "cd: no such directory: %s\n", args[1]);
                return 0;
            }
            if (chdir(args[1]) != 0) {
                perror("cd failed");
                return 0;
            }
            return 1;
        }

        if (i < num_pipes - 1) pipe(pipefd);
        pid_t pid = fork();
        if (pid == 0) {
            if (prevfd != -1) {
                dup2(prevfd, STDIN_FILENO);
                close(prevfd);
            }
            if (i < num_pipes - 1) {
                dup2(pipefd[1], STDOUT_FILENO);
                close(pipefd[1]);
            }
            if (pipefd[0]) close(pipefd[0]);

            if (in_file) {
                int fd = open(in_file, O_RDONLY);
                if (fd < 0) { perror("open input failed"); exit(1); }
                dup2(fd, STDIN_FILENO);
                close(fd);
            }
            if (out_file) {
                int flags = O_WRONLY | O_CREAT | (append ? O_APPEND : O_TRUNC);
                int fd = open(out_file, flags, 0644);
                if (fd < 0) { perror("open output failed"); exit(1); }
                dup2(fd, STDOUT_FILENO);
                close(fd);
            }

            execvp(args[0], args);
            perror("exec failed");
            exit(1);
        } else if (pid > 0) {
            wait(&status);
            if (prevfd != -1) close(prevfd);
            if (i < num_pipes - 1) {
                close(pipefd[1]);
                prevfd = pipefd[0];
            }
        } else {
            perror("fork failed");
            return 1;
        }
    }
    return WIFEXITED(status) && WEXITSTATUS(status) == 0;
}


void handle_pipeline_and_chaining(char *input) {
    char *segments[20];
    int seg_count = 0;
    char *token = strtok(input, ";");
    
    while (token != NULL) {
        segments[seg_count] = token;
        seg_count++;
        token = strtok(NULL, ";");
    }
 
    for (int i = 0; i < seg_count; ++i) {
        char *cmds[20];
        int and_count = 0;
        cmds[and_count++] = strtok(segments[i], "&&");


        while ((cmds[and_count++] = strtok(NULL, "&&")));

        and_count--;

        // print cmds : debug
        // for (int j = 0; j < and_count; ++j) {
        //     printf("%s\n", cmds[j]);
        // }

        int success = 1;
        for (int j = 0; j < and_count; ++j) {
            if (success) {
                success = execute_pipeline(cmds[j]);
            }
        }
    }
}

void display_welcome() {
    
    printf("\n\033[1;35m");
    printf("✨ Welcome to Bracu Badda Shell ✨\n");
    printf("Type 'exit' to return to Mohakhali\n\n");
    printf("\033[0m");
    
}

int main() {
    char input[MAX_INPUT];
    signal(SIGINT, handle_sigint);
    
    display_welcome();
    while (1) {
        printf("\033[1;33m");
        printf("sh> ");
        printf("\033[0m");

        if (!fgets(input, sizeof(input), stdin)) break;
        input[strcspn(input, "\n")] = '\0';
        if (strlen(input) == 0) continue;
 
        add_to_history(input);
 
        if (strcmp(input, "exit") == 0) break;
        if (strcmp(input, "history") == 0) {
            for (int i = 0; i < history_count; ++i)
                printf("%d: %s\n", i + 1, history[i]);
            continue;
        }
 
        handle_pipeline_and_chaining(input);
    }
    return 0;
}
 