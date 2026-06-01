
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <regex.h>

int main(int argc, char *argv[]) {
    if (argc != 4) {
        fprintf(stderr, "Usage: %s <regex> <text> <replacement>\n", argv[0]);
        return 1;
    }

    const char *pattern = argv[1];
    const char *text = argv[2];
    const char *replacement = argv[3];
    size_t text_len = strlen(text);
    size_t repl_len = strlen(replacement);

    regex_t regex;
    int ret = regcomp(&regex, pattern, REG_EXTENDED);
    if (ret != 0) {
        char errbuf[256];
        regerror(ret, &regex, errbuf, sizeof(errbuf));
        fprintf(stderr, "Regex compilation failed: %s\n", errbuf);
        return 1;
    }

    size_t out_size = text_len + 1;
    char *out = malloc(out_size);
    if (!out) {
        perror("malloc");
        regfree(&regex);
        return 1;
    }
    size_t out_len = 0;

    size_t prev_pos = 0;
    size_t offset = 0;
    regmatch_t pmatch[1];

    while (regexec(&regex, text + offset, 1, pmatch, 0) == 0) {
        size_t match_start = offset + pmatch[0].rm_so;
        size_t match_end   = offset + pmatch[0].rm_eo;

        size_t chunk_len = match_start - prev_pos;
        size_t needed = out_len + chunk_len + repl_len + (text_len - match_end) + 1;
        if (needed > out_size) {
            out_size = needed * 2;
            char *new_out = realloc(out, out_size);
            if (!new_out) {
                perror("realloc");
                free(out);
                regfree(&regex);
                return 1;
            }
            out = new_out;
        }
        memcpy(out + out_len, text + prev_pos, chunk_len);
        out_len += chunk_len;

        memcpy(out + out_len, replacement, repl_len);
        out_len += repl_len;

        if (match_start != match_end) {
            prev_pos = match_end;
            offset = match_end;
        } else {
            if (match_start < text_len) {
                if (out_len + 1 + (text_len - (match_start + 1)) + 1 > out_size) {
                    out_size = (out_len + 1 + (text_len - (match_start + 1)) + 1) * 2;
                    char *new_out = realloc(out, out_size);
                    if (!new_out) {
                        perror("realloc");
                        free(out);
                        regfree(&regex);
                        return 1;
                    }
                    out = new_out;
                }
                out[out_len++] = text[match_start];
                prev_pos = match_start + 1;
                offset = match_start + 1;
            } else {
                break;
            }
        }
    }

    size_t remaining = text_len - prev_pos;
    if (out_len + remaining + 1 > out_size) {
        out_size = out_len + remaining + 1;
        char *new_out = realloc(out, out_size);
        if (!new_out) {
            perror("realloc");
            free(out);
            regfree(&regex);
            return 1;
        }
        out = new_out;
    }
    memcpy(out + out_len, text + prev_pos, remaining);
    out_len += remaining;
    out[out_len] = '\0';

    printf("%s\n", out);

    free(out);
    regfree(&regex);
    return 0;
}
