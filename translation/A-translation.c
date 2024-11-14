// jean-claude van bot made this one
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>

#define MAX_LINE 4096
#define DEBUG 0

typedef struct {
    int brace_level;
    bool in_macro;
    bool in_string;
    bool in_char;
    bool in_single_comment;
    bool in_multi_comment;
    bool line_continuation;
    char prev_char;
} ParserState;

void init_state(ParserState *state) {
    state->brace_level = 0;
    state->in_macro = false;
    state->in_string = false;
    state->in_char = false;
    state->in_single_comment = false;
    state->in_multi_comment = false;
    state->line_continuation = false;
    state->prev_char = '\0';
}

bool should_add_semicolon(const char *line, ParserState *state) {
    int len = strlen(line);
    char last_non_space = '\0';
    
    // find last non-whitespace character
    for (int i = len - 1; i >= 0; i--)
        if (!isspace(line[i])) {
            last_non_space = line[i];
            break;
        }
    
    // dont add if line already ends with ; { or \ macro-continue
    if (last_non_space == ';' || last_non_space == '{' || 
        last_non_space == '\\') { // design: rem: last_non_space == '}'
        return false;
    }
    
    // dont add if we're in a special state
    if (state->in_macro || state->in_single_comment || 
        state->in_multi_comment || state->in_string || 
        state->in_char || state->line_continuation) {
        return false;
    }
    
    // only add if we're inside function body
    return state->brace_level >= 0;
}

void process_char(char c, char n, ParserState *state) {
    // string-literals
    if (c == '"' && state->prev_char != '\\' && !state->in_char && 
        !state->in_single_comment && !state->in_multi_comment) {
        state->in_string = !state->in_string;
    }
    
    // character-literals
    if (c == '\'' && state->prev_char != '\\' && !state->in_string && 
        !state->in_single_comment && !state->in_multi_comment) {
        state->in_char = !state->in_char;
    }

    // silver-style comments
    if (c == '#' && state->prev_char == '#' && !state->in_string && 
        !state->in_char && !state->in_multi_comment) {
        state->in_multi_comment = !state->in_multi_comment;
    }
    else if (c == '#' && !state->in_string && 
        !state->in_char && !state->in_multi_comment) {
        state->in_single_comment = true;
    }
    
    if (c == '/' && state->prev_char == '*' && state->in_multi_comment) {
        state->in_multi_comment = false;
    }
    
    // track brace level when not in comments or strings
    if (!state->in_string && !state->in_char && 
        !state->in_single_comment && !state->in_multi_comment) {
        //if (c == '{') state->brace_level++;
        //if (c == '}') state->brace_level--;
    }
    
    // macros; #  define not allowed with this simple logic
    if (c == '#' && (n != 0 && isalpha(n)) && !state->in_string && !state->in_char && 
        !state->in_single_comment && !state->in_multi_comment) {
        state->in_macro = true;
    }
    
    state->prev_char = c;
}

void process_line(char *line, ParserState *state, FILE *out) {
    int len = strlen(line);
    bool needs_semicolon = false;
    
    // reset single-line comment state
    state->in_single_comment = false;
    
    // process each character
    for (int i = 0; i < len; i++) {
        process_char(line[i], line[i + 1], state);
    }
    
    // check for line continuation
    if (len > 0 && line[len-1] == '\\') {
        state->line_continuation = true;
    } else {
        state->line_continuation = false;
        // reset macro state at end of non-continued line
        if (!state->line_continuation)
             state->in_macro = false;
    }
    
    // determine if we need to add a semicolon
    needs_semicolon = should_add_semicolon(line, state) || 
        (!state->in_macro  && !state->in_multi_comment && !state->in_single_comment &&
         !state->in_string && (strcmp(line, "}\n") == 0 || strcmp(line, "}") == 0));
    
    // write the line, potentially with added semicolon
    if (needs_semicolon) {
        // remove trailing whitespace before adding semicolon
        while (len > 0 && isspace(*(line + len - 1))) len--;
        //strcpy(line + len, ";\n");
    }

    fputs(line, out);
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "usage: %s <input-a> <output-c>\n", argv[0]);
        return 1;
    }
    
    FILE *in = fopen(argv[1], "r");
    if (!in) {
        perror("failed to open input a-file");
        return 1;
    }
    
    // Create output filename by appending "_semi.c"
    FILE *out = fopen(argv[2], "w");
    if (!out) {
        perror("failed to open output c-file");
        fclose(in);
        return 1;
    }
    
    char line[MAX_LINE + 1];
    ParserState state;
    init_state(&state);
    memset(line, 0, sizeof(line));
    while (fgets(line, sizeof(line), in)) {
        process_line(line, &state, out);
        memset(line, 0, sizeof(line));
    }
    
    fclose(in);
    fclose(out);
    printf("Processed file saved as: %s\n", argv[2]);
    return 0;
}