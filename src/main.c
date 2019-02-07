#include <stdio.h>
#include <stdlib.h>

#include "hash.h"

int main()
{
    printf("%lu\n", hash_djb2("wtf is this string?"));
    printf("%lu\n", hash_djb2("oWo"));
    printf("%lu\n", hash_djb2("yaddabadda duu"));
    printf("%lu\n", hash_sdbm("wtf is this string?"));
    printf("%lu\n", hash_sdbm("oWo"));
    printf("%lu\n", hash_sdbm("yaddabadda duu"));
    return EXIT_SUCCESS;
}
