#include <cstring> // for strdup, strcmp
#include <cstdlib> // for free

extern "C" {
    #include "regex-replace.h"
}

char * decode_alloc(char const * const _i) {
    char *ret = NULL;
    char *ret2 = NULL;

    ret = regex_simplereplace(_i, "<br>", "", 1, 1);
    ret2 = strdup(ret);
    free(ret);

    ret = regex_simplereplace(ret2, "<span [^>]+>([^<]+)</span>", "%1", 1, 1);
    free(ret2);
    ret2 = strdup(ret);
    free(ret);

    ret = regex_simplereplace(ret2, "<div>([^<]+)</div>", "%1\n", 1, 1);
    free(ret2);
    ret2 = strdup(ret);
    free(ret);

    ret = regex_simplereplace(ret2, "&lt;", "<", 1, 1);
    free(ret2);
    ret2 = strdup(ret);
    free(ret);

    ret = regex_simplereplace(ret2, "&nbsp;", " ", 1, 1);
    free(ret2);

    return ret;
}
