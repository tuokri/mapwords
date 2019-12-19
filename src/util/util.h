#ifndef MAPWORDS_UTIL_H
#define MAPWORDS_UTIL_H

// Read next word from file into buffer (limited by len).
int
read_next_word(FILE* f, char* buf, int len);

// Convert string to lowercase inplace.
void
str_tolower(char* str);

#endif //MAPWORDS_UTIL_H
