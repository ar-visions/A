#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h> // they should call this stduni  or stdunix; could be mistaken for unicode std, which we also need and dont have


#define BUFFER_SIZE         4096
#define MAX_IMPORT_NAME     256
#define INCLUDE_PREFIX      "#include <"
#define INCLUDE_SUFFIX      ">"

void verify(int condition, const char *message) {
    if (!condition) {
        fprintf(stderr, "Error: %s\n", message);
        exit(2);
    }
}

void process_file(const char *src, const char *dst) {
    FILE *src_file = fopen(src, "r");
    verify(src_file != NULL, "source not found");

    char temp_file[] = "include.XXXXXX";
    int temp_fd = mkstemp(temp_file);
    if (temp_fd == -1) {
        perror("mkstemp failed");
        return;
    }

    FILE *dst_file = fdopen(temp_fd, "w");
    verify(dst_file != NULL, "destination not found");

    //FILE *dst_file = fopen(dst, "w");
    //verify(dst_file != NULL, "destination not found");

    char buffer[BUFFER_SIZE];
    while (fgets(buffer, sizeof(buffer), src_file)) {
        char *include_start = strstr(buffer, INCLUDE_PREFIX);
        if (include_start) {
            char *include_end = strstr(include_start, INCLUDE_SUFFIX);
            if (include_end) {
                // Extract filename inside #include <>
                char import_name[MAX_IMPORT_NAME];
                size_t name_length = include_end - (include_start + strlen(INCLUDE_PREFIX));
                if (name_length >= MAX_IMPORT_NAME) {
                    fprintf(stderr, "Error: Import name too long\n");
                    exit(2);
                }

                strncpy(import_name, include_start + strlen(INCLUDE_PREFIX), name_length);
                import_name[name_length] = '\0';

                // Open the import file
                FILE *import_file = fopen(import_name, "r");
                if (!import_file) {
                    fprintf(stderr, "Error: Failed to open import file: %s\n", import_name);
                    exit(2);
                }

                // Write import file contents to destination
                while (fgets(buffer, sizeof(buffer), import_file)) {
                    fputs(buffer, dst_file);
                }
                fclose(import_file);
                continue;
            }
        }
        // Write original content if no include directive found
        fputs(buffer, dst_file);
    }

    fclose(src_file);
    fclose(dst_file);
    if (access(dst, F_OK) == 0)
        unlink(dst);
    // Rename the temporary file to `dst`
    if (rename(temp_file, dst) != 0) {
        perror("Failed to rename temp file");
        exit(1);
    }
}

int main(int argc, char *argv[]) {
    verify(argc == 3, "usage: A-include <src> <dst> (source file with #include <import-file>)");
    const char* src = argv[1];
    const char* dst = argv[2];
    process_file(src, dst);
    return 0;
}
