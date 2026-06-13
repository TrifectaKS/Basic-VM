#include "asm_utils.h"
#include <ctype.h>
#include <string.h>
#include <stdlib.h>

int is_label_definition(const char *line) {
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

int is_comment_or_blank(const char *line) {
    size_t i = 0;
    while (isspace(line[i])) i++;
    return (line[i] == ';' || line[i] == '\n' || line[i] == '\r' || line[i] == '\0');
}

int extract_label_name(const char *line, char *name, size_t max_len) {
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

int is_variable_definition(const char *line) {
    size_t len = strlen(line);
    if (len == 0) return 0;
    
    size_t i = 0;
    while (i < len && isspace(line[i])) i++;
    if (i >= len || (!isalpha(line[i]) && line[i] != '_')) return 0;
    
    size_t start = i;
    while (i < len && (isalnum(line[i]) || line[i] == '_')) i++;
    
    while (i < len && isspace(line[i])) i++;
    
    if (i >= len || line[i] != '=') return 0;
    
    i++;
    
    while (i < len && isspace(line[i])) i++;
    
    if (i >= len) return 0;
    
    return 1;
}

int extract_variable_name_and_value(const char *line, char *name, size_t max_name_len, uint32_t *out_value) {
    size_t i = 0;
    while (isspace(line[i])) i++;
    
    size_t start = i;
    while (i < max_name_len && (isalnum(line[i]) || line[i] == '_')) i++;
    
    size_t name_len = i - start;
    if (name_len >= max_name_len) name_len = max_name_len - 1;
    
    strncpy(name, line + start, name_len);
    name[name_len] = '\0';
    
    for (size_t j = 0; name[j]; j++) {
        name[j] = tolower(name[j]);
    }
    
    while (i < strlen(line) && isspace(line[i])) i++;
    if (i >= strlen(line) || line[i] != '=') return 0;
    i++;
    
    while (i < strlen(line) && isspace(line[i])) i++;
    if (i >= strlen(line)) return 0;
    
    char *endptr = NULL;
    unsigned long value = strtoul(line + i, &endptr, 0);
    if (*endptr != '\0' && *endptr != ';' && *endptr != '\n' && *endptr != '\r') {
        return 0;
    }
    
    *out_value = (uint32_t)value;
    return 1;
}