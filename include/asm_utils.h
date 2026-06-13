#ifndef ASM_UTILS_H
#define ASM_UTILS_H

#include <stddef.h>
#include <stdint.h>

int is_label_definition(const char *line);
int is_comment_or_blank(const char *line);
int extract_label_name(const char *line, char *name, size_t max_len);
int is_variable_definition(const char *line);
int extract_variable_name_and_value(const char *line, char *name, size_t max_name_len, uint32_t *out_value);

#endif