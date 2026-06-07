#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>

#define MAX_ARGS 64
#define MAX_ARG_LEN 256

int parse_command(char *cmd_str, char *args[]) {
    int arg_count = 0;
    char *token;
    
    while (*cmd_str == ' ') cmd_str++;
    
    token = strtok(cmd_str, " ");
    while (token != NULL && arg_count < MAX_ARGS - 1) {
        args[arg_count] = token;
        arg_count++;
        token = strtok(NULL, " ");
    }
    args[arg_count] = NULL;
    
    return arg_count;
}

int execute_command_with_wc(char *cmd_str) {
    int pipe1[2];
    int pipe2[2];
    pid_t pid1, pid2;
    int status;
    char buffer[64];
    ssize_t bytes_read;
    int byte_count = 0;
    
    if (pipe(pipe1) == -1) {
        perror("pipe1");
        return -1;
    }
    if (pipe(pipe2) == -1) {
        perror("pipe2");
        close(pipe1[0]);
        close(pipe1[1]);
        return -1;
    }
    
    char *args[MAX_ARGS];
    char cmd_copy[MAX_ARG_LEN * MAX_ARGS];
    strcpy(cmd_copy, cmd_str);
    
    if (parse_command(cmd_copy, args) == 0) {
        fprintf(stderr, "Error: Empty command\n");
        close(pipe1[0]);
        close(pipe1[1]);
        close(pipe2[0]);
        close(pipe2[1]);
        return -1;
    }
    
    pid1 = fork();
    if (pid1 == -1) {
        perror("fork");
        close(pipe1[0]);
        close(pipe1[1]);
        close(pipe2[0]);
        close(pipe2[1]);
        return -1;
    }
    
    if (pid1 == 0) {
        close(pipe1[0]);
        dup2(pipe1[1], STDOUT_FILENO);
        close(pipe1[1]);
        close(pipe2[0]);
        close(pipe2[1]);
        
        execvp(args[0], args);
        
        perror("execvp");
        exit(1);
    }
    
    pid2 = fork();
    if (pid2 == -1) {
        perror("fork");
        close(pipe1[0]);
        close(pipe1[1]);
        close(pipe2[0]);
        close(pipe2[1]);
        waitpid(pid1, NULL, 0);
        return -1;
    }
    
    if (pid2 == 0) {
        close(pipe1[1]);
        dup2(pipe1[0], STDIN_FILENO);
        close(pipe1[0]);
        
        close(pipe2[0]);
        dup2(pipe2[1], STDOUT_FILENO);
        close(pipe2[1]);
        
        execlp("wc", "wc", "-c", NULL);
        
        perror("execlp");
        exit(1);
    }
    
    close(pipe1[0]);
    close(pipe1[1]);
    close(pipe2[1]);
    
    bytes_read = read(pipe2[0], buffer, sizeof(buffer) - 1);
    if (bytes_read > 0) {
        buffer[bytes_read] = '\0';
        byte_count = atoi(buffer);
    }
    close(pipe2[0]);
    
    waitpid(pid1, NULL, 0);
    waitpid(pid2, &status, 0);
    
    return byte_count;
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s \"command1\" \"command2\" ...\n", argv[0]);
        return 1;
    }
    
    for (int i = 1; i < argc; i++) {
        int size = execute_command_with_wc(argv[i]);
        if (size >= 0) {
            printf("%d\n", size);
        } else {
            fprintf(stderr, "Error executing: %s\n", argv[i]);
        }
    }
    
    return 0;
}