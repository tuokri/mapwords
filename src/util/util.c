#include <ctype.h>
#include <stdio.h>

#include "util.h"

// Construct a string literal with the preprocessor.
#define STR_(X)
#define STR(X) STR_(X)

int
read_next_word(FILE* f, char* buf, int len)
{
    // Consume all non-allowed characters.
    fscanf(f, "%*[^a-zA-Z']");
    return fscanf(f, "%" STR(len) "[a-zA-Z']", buf);
}

void
str_tolower(char* str)
{
    while (*str)
    {
        *str = (char) tolower(*str);
        str++;
    }
}
