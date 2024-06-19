#include <iostream>
#include <cstdlib>
#include <cstring>
#include <regex>
#include <vector>

#define REPCHAR '%'
#define MAXSUBS 4096

char* regex_simplereplace(const char* string_to_search_in, const char* pattern, const char* replacement, int case_insensitive, int global_replace) {
    char* ret = static_cast<char*>(malloc(1));
    strcpy(ret, "");

    std::regex_constants::syntax_option_type flags = std::regex_constants::ECMAScript;
    if (case_insensitive)
        flags |= std::regex_constants::icase;

    std::regex regex_pattern(pattern, flags);
    const char* pos = string_to_search_in;
    std::cmatch match;

    int from = 0;
    while (std::regex_search(pos, match, regex_pattern)) {
        ret = static_cast<char*>(realloc(ret, strlen(ret) + match.position() + 1));
        strncat(ret, pos, match.position());

        int rep_off = 0;
        int rep_len = strlen(replacement);

        while (rep_off < rep_len) {
            const char* rep_pos = strchr(replacement + rep_off, REPCHAR);
            if (rep_pos == nullptr) {
                ret = static_cast<char*>(realloc(ret, strlen(ret) + strlen(replacement + rep_off) + 1));
                strcat(ret, replacement + rep_off);
                rep_off += strlen(replacement + rep_off);
                continue;
            }

            strncat(ret, replacement + rep_off, rep_pos - (replacement + rep_off));
            if (*(rep_pos + 1) == '\0' || *(rep_pos + 1) == REPCHAR) {
                ret = static_cast<char*>(realloc(ret, strlen(ret) + 1));
                *ret = REPCHAR;
                *(ret + 1) = '\0';
                rep_off += 1;
                continue;
            }

            int replacement_group = 0;
            int replacement_group_nextchar = 1;
            const char* digits = "0123456789";
            const char* n;
            while ((n = strchr(digits, *(rep_pos + replacement_group_nextchar))) != nullptr) {
                if ((n - digits) == 10) {
                    break;
                }
                replacement_group = replacement_group * 10 + (n - digits);
                ++replacement_group_nextchar;
            }

            if (replacement_group < MAXSUBS) {
                int group_start = match.position(replacement_group);
                int group_length = match.length(replacement_group);
                ret = static_cast<char*>(realloc(ret, strlen(ret) + group_length + 1));
                strncat(ret, string_to_search_in + group_start, group_length);
            }

            rep_off += replacement_group_nextchar;
        }

        from = match.position() + match.length();
        pos = string_to_search_in + from;

        if (!global_replace)
            break;
    }

    ret = static_cast<char*>(realloc(ret, strlen(ret) + strlen(pos) + 1));
    strcat(ret, pos);

    return ret;
}

int main() {
    const char* input_string = "Hello world, this is a test string for regex replace.";
    const char* regex_pattern = "world";
    const char* replace_with = "universe";

    char* result = regex_simplereplace(input_string, regex_pattern, replace_with, 0, 1);
    std::cout << "Result: " << result << std::endl;
    free(result);

    return 0;
}
