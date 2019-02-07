#include <stdio.h>
#include <stdlib.h>

#include "hash.h"

int main()
{
    printf("%d\n", hash_djb2("wtf is this string?"));
    printf("%d\n", hash_djb2("oWo"));
    printf("%d\n", hash_djb2("yaddabadda duu"));
    printf("%d\n", hash_sdbm("wtf is this string?"));
    printf("%d\n", hash_sdbm("oWo"));
    printf("%d\n", hash_sdbm("yaddabadda duu"));
    return EXIT_SUCCESS;
}
