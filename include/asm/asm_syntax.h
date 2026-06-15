#ifndef ASM_SYNTAX_H
#define ASM_SYNTAX_H

#include <strings.h>
#include <string.h>

#define RESERVED_SYMBOLS \
    X(SP) \
    X(LR) \
    X(PC)

#define IS_RESERVED_SYMBOL(name) \
    (strcasecmp(name, "SP") == 0 || \
     strcasecmp(name, "LR") == 0 || \
     strcasecmp(name, "PC") == 0)

#endif
