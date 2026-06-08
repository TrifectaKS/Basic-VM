#include "asm.h"
#include <assert.h>
#include <string.h>
#include <stdarg.h>

static FILE *g_logFile = NULL;
static char g_logPath[512];

static void open_log_file(const char *romFilePath) {
#if defined(DEBUG_LOG)
    snprintf(g_logPath, sizeof(g_logPath), "%s.log", romFilePath);
    g_logFile = fopen(g_logPath, "w");
#endif
}

static void close_log_file(void) {
#if defined(DEBUG_LOG)
    if (g_logFile) {
        fclose(g_logFile);
        g_logFile = NULL;
    }
#endif
}

static void debug_printf(const char *format, ...) {
    va_list args;
    va_start(args, format);
#if defined(DEBUG)
    vfprintf(stderr, format, args);
#endif
#if defined(DEBUG_LOG)
    if (g_logFile) {
        vfprintf(g_logFile, format, args);
    }
#endif
    va_end(args);
}

static void write_error(const char *romFilePath, uint32_t line_num, const char *line, const char *message) {
#if defined(DEBUG_LOG)
    char logPath[512];
    snprintf(logPath, sizeof(logPath), "%s.log", romFilePath);
    FILE *logFile = fopen(logPath, "w");
    if (logFile) {
        fprintf(logFile, "ERROR at line %u:\n", line_num);
        fprintf(logFile, "  %s\n", line);
        fprintf(logFile, "  %s\n", message);
        fclose(logFile);
    }
#endif
#if defined(DEBUG)
    fprintf(stderr, "ERROR at line %u:\n", line_num);
    fprintf(stderr, "  %s\n", line);
    fprintf(stderr, "  %s\n", message);
#endif
}

static int is_label_definition(const char *line) {
    size_t len = strlen(line);
    if (len == 0) return 0;
    
    size_t i = 0;
    while (i < len && isspace(line[i])) i++;
    if (i >= len || (!isalpha(line[i]) && line[i] != '_')) return 0;
    
    size_t start = i;
    while (i < len && (isalnum(line[i]) || line[i] == '_')) i++;
    
    while (i < len && isspace(line[i])) i++;
    
    if (i < len && line[i] == ':') return 1;
    
    if (i < len && line[i] == ':' && (i + 1 >= len || line[i+1] == '\n' || line[i+1] == '\r' || line[i+1] == '\0')) return 1;
    
    return 0;
}

static int is_comment_or_blank(const char *line) {
    size_t i = 0;
    while (isspace(line[i])) i++;
    return (line[i] == ';' || line[i] == '\n' || line[i] == '\r' || line[i] == '\0');
}

static int extract_label_name(const char *line, char *name, size_t max_len) {
    size_t i = 0;
    while (isspace(line[i])) i++;
    
    size_t start = i;
    while (i < max_len && (isalnum(line[i]) || line[i] == '_')) i++;
    
    size_t name_len = i - start;
    if (name_len >= max_len) name_len = max_len - 1;
    
    strncpy(name, line + start, name_len);
    name[name_len] = '\0';
    
    for (size_t j = 0; name[j]; j++) {
        name[j] = tolower(name[j]);
    }
    
    return 0;
}

int asm_main(const char *asmFilePath, const char *romFilePath)
{
    open_log_file(romFilePath);
    debug_printf("=== Assembly started: %s -> %s ===\n", asmFilePath, romFilePath);

    clear_labels();
    
    FILE *asmFile = fopen(asmFilePath, "r");
    if (asmFile == NULL)
    {
        perror("Error opening .asm file");
        close_log_file();
        return 1;
    }

    char asmLineBuffer[256];
    uint32_t program_counter = 0;
    uint32_t line_num = 0;
    
    while (fgets(asmLineBuffer, sizeof(asmLineBuffer), asmFile))
    {
        line_num++;
        if (is_comment_or_blank(asmLineBuffer)) {
            continue;
        }
        
        if (is_label_definition(asmLineBuffer)) {
            char label_name[MAX_LABEL_LENGTH];
            extract_label_name(asmLineBuffer, label_name, MAX_LABEL_LENGTH);
            
            if (add_label(label_name, program_counter) != 0) {
                write_error(romFilePath, line_num, asmLineBuffer, "Duplicate label definition");
                fclose(asmFile);
                clear_labels();
                close_log_file();
                return 1;
            }
            continue;
        }
        
        program_counter += 4;
    }
    
    fseek(asmFile, 0, SEEK_SET);
    
    FILE *romFile = fopen(romFilePath, "wb");
    if (romFile == NULL)
    {
        perror("Error opening .rom file");
        fclose(asmFile);
        clear_labels();
        close_log_file();
        return 1;
    }
    
    program_counter = 0;
    line_num = 0;
    while (fgets(asmLineBuffer, sizeof(asmLineBuffer), asmFile))
    {
        line_num++;
        if (is_comment_or_blank(asmLineBuffer)) {
            continue;
        }
        
        if (is_label_definition(asmLineBuffer)) {
            continue;
        }
        
        debug_printf("----------------\n%s\n", asmLineBuffer);
        Instruction *instruction = get_instruction_by_asm(asmLineBuffer);

        if (instruction == NULL)
        {
            write_error(romFilePath, line_num, asmLineBuffer, "Unknown instruction");
            fclose(asmFile);
            fclose(romFile);
            clear_labels();
            close_log_file();
            return 1;
        }

        if (instruction->opcode == 0x0)
        {
            AssembledOperation result = handle_opcode(instruction, asmLineBuffer, program_counter);
            BytePack pack = pack_bytes(instruction, result.value);
            write_to_file(romFile, pack);
            program_counter += 4;
            continue;
        }

        AssembledOperation result = handle_opcode(instruction, asmLineBuffer, program_counter);

        if (!result.hasValue)
        {
            write_error(romFilePath, line_num, asmLineBuffer, "Failed to assemble instruction");
            fclose(asmFile);
            fclose(romFile);
            clear_labels();
            close_log_file();
            return 1;
        }

        BytePack pack = pack_bytes(instruction, result.value);
        write_to_file(romFile, pack);

        program_counter += 4;
    }

    debug_printf("=== Assembly completed successfully ===\n");

    fclose(asmFile);
    fclose(romFile);
    clear_labels();
    close_log_file();

    return 0;
}
