#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <string.h>

#define PAGE_SIZE 4096

void print_middle_line(const char *filename) {
    int fd;
    struct stat file_stat;
    char *buffer;
    ssize_t bytes_read;
    off_t mid_point;
    off_t current_pos;
    off_t line_start;
    off_t line_end;
    int found_newline;
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
    
    if (file_stat.st_size == 0) {
        write(STDERR_FILENO, "Error: File is empty\n", 21);
        close(fd);
        exit(1);
    }
    
    // Calculate middle point
    mid_point = file_stat.st_size / 2;
    
    // Seek to the middle point
    if (lseek(fd, mid_point, SEEK_SET) == -1) {
        write(STDERR_FILENO, "Error: lseek failed\n", 20);
        close(fd);
        exit(1);
    }
    
    // Allocate buffer
    buffer = malloc(PAGE_SIZE);
    if (!buffer) {
        write(STDERR_FILENO, "Error: Memory allocation failed\n", 32);
        close(fd);
        exit(1);
    }
    
    // Find start of the line containing the middle point
    // Search backwards for newline character
    current_pos = mid_point;
    found_newline = 0;
    
    while (current_pos > 0 && !found_newline) {
        off_t seek_pos = (current_pos >= PAGE_SIZE) ? current_pos - PAGE_SIZE : 0;
        size_t read_size = current_pos - seek_pos;
        
        if (lseek(fd, seek_pos, SEEK_SET) == -1) {
            break;
        }
        
        bytes_read = read(fd, buffer, read_size);
        if (bytes_read <= 0) {
            break;
        }
        
        // Search backwards for newline
        for (i = bytes_read - 1; i >= 0; i--) {
            if (buffer[i] == '\n') {
                line_start = seek_pos + i + 1;
                found_newline = 1;
                break;
            }
        }
        
        if (!found_newline) {
            current_pos = seek_pos;
        }
    }
    
    if (!found_newline) {
        line_start = 0;
    }
    
    // Find end of the line
    if (lseek(fd, line_start, SEEK_SET) == -1) {
        write(STDERR_FILENO, "Error: lseek failed\n", 20);
        free(buffer);
        close(fd);
        exit(1);
    }
    
    line_end = line_start;
    found_newline = 0;
    
    while (!found_newline) {
        bytes_read = read(fd, buffer, PAGE_SIZE);
        if (bytes_read <= 0) {
            line_end = file_stat.st_size;
            break;
        }
        
        for (i = 0; i < bytes_read; i++) {
            line_end++;
            if (buffer[i] == '\n') {
                found_newline = 1;
                break;
            }
        }
    }
    
    // Print the line
    if (lseek(fd, line_start, SEEK_SET) == -1) {
        write(STDERR_FILENO, "Error: lseek failed\n", 20);
        free(buffer);
        close(fd);
        exit(1);
    }
    
    off_t line_length = line_end - line_start;
    while (line_length > 0) {
        size_t to_read = (line_length < PAGE_SIZE) ? line_length : PAGE_SIZE;
        bytes_read = read(fd, buffer, to_read);
        if (bytes_read <= 0) {
            break;
        }
        write(STDOUT_FILENO, buffer, bytes_read);
        line_length -= bytes_read;
    }
    
    free(buffer);
    close(fd);
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        write(STDERR_FILENO, "Usage: program <filename>\n", 26);
        return 1;
    }
    
    print_middle_line(argv[1]);
    return 0;
}