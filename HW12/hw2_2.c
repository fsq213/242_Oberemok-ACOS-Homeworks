#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <string.h>

#define PAGE_SIZE 4096

void print_lines_from(const char *filename, int start_line, int num_lines) {
    int fd;
    struct stat file_stat;
    char *buffer;
    ssize_t bytes_read;
    int current_line = 1;
    int lines_printed = 0;
    int in_line = 0;
    int i;
    
    // Open file
    fd = open(filename, O_RDONLY);
    if (fd == -1) {
        write(STDERR_FILENO, "Error: Cannot open file\n", 24);
        exit(1);
    }
    
    // Get file size
    if (fstat(fd, &file_stat) == -1) {
        write(STDERR_FILENO, "Error: Cannot get file stats\n", 29);
        close(fd);
        exit(1);
    }
    
    // Allocate buffer of page size
    buffer = malloc(PAGE_SIZE);
    if (!buffer) {
        write(STDERR_FILENO, "Error: Memory allocation failed\n", 32);
        close(fd);
        exit(1);
    }
    
    // Read file in chunks
    while ((bytes_read = read(fd, buffer, PAGE_SIZE)) > 0) {
        for (i = 0; i < bytes_read; i++) {
            if (buffer[i] == '\n') {
                if (current_line >= start_line && lines_printed < num_lines) {
                    write(STDOUT_FILENO, "\n", 1);
                    lines_printed++;
                }
                current_line++;
                in_line = 0;
            } else {
                if (current_line >= start_line && lines_printed < num_lines) {
                    write(STDOUT_FILENO, &buffer[i], 1);
                    in_line = 1;
                }
            }
        }
    }
    
    // Handle last line if file doesn't end with newline
    if (in_line && current_line >= start_line && lines_printed < num_lines) {
        write(STDOUT_FILENO, "\n", 1);
    }
    
    free(buffer);
    close(fd);
}

int main(int argc, char *argv[]) {
    if (argc != 4) {
        write(STDERR_FILENO, "Usage: program <filename> <start_line> <num_lines>\n", 50);
        return 1;
    }
    
    int start_line = atoi(argv[2]);
    int num_lines = atoi(argv[3]);
    
    if (start_line < 1 || num_lines < 1) {
        write(STDERR_FILENO, "Error: Line numbers must be positive\n", 37);
        return 1;
    }
    
    print_lines_from(argv[1], start_line, num_lines);
    return 0;
}