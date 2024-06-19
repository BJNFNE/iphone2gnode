#ifndef __REGEX_REPLACE_H
#define __REGEX_REPLACE_H

#ifdef __cplusplus
extern "C" {
#endif

char * regex_simplereplace(const char* target, const char* restrict pattern, const char* replacement, int case_sensitive, int global_replace);

#ifdef __cplusplus
}
#endif

#endif // __REGEX_REPLACE_H
