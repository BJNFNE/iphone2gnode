#include <cstdlib>   // for EXIT_SUCCESS, EXIT_FAILURE
#include <cstdio>    // for fprintf, stderr
#include <cstring>   // for strcmp
#include "regex_replace.h"

int test() {
    char * r;
    const char * e;

    // Test case 1
    r = regex_simplereplace("abcdefghijkl", "g", "h", 0, 0);
    e = "abcdefhhijkl";
    if (std::strcmp(r, e) != 0) {
        std::fprintf(stderr, "Test case 1 failed:\nExpected: %s\nActual:   %s\n", e, r);
        std::free(r);
        return EXIT_FAILURE;
    }
    std::free(r);

    // Test case 2
    r = regex_simplereplace("10 quick fox", "x", "xes waiting for chickens", 0, 0);
    e = "10 quick foxes waiting for chickens";
    if (std::strcmp(r, e) != 0) {
        std::fprintf(stderr, "Test case 2 failed:\nExpected: %s\nActual:   %s\n", e, r);
        std::free(r);
        return EXIT_FAILURE;
    }
    std::free(r);

    // Test case 3
    r = regex_simplereplace("10 quick fox", " ", "_", 0, 0);
    e = "10_quick fox";
    if (std::strcmp(r, e) != 0) {
        std::fprintf(stderr, "Test case 3 failed:\nExpected: %s\nActual:   %s\n", e, r);
        std::free(r);
        return EXIT_FAILURE;
    }
    std::free(r);

    // Test case 4
    r = regex_simplereplace("20 quick fox", " ", "_", 0, 1);
    e = "20_quick_fox";
    if (std::strcmp(r, e) != 0) {
        std::fprintf(stderr, "Test case 4 failed:\nExpected: %s\nActual:   %s\n", e, r);
        std::free(r);
        return EXIT_FAILURE;
    }
    std::free(r);

    // Test case 5
    r = regex_simplereplace("Rapid roadrunner", "r", ":", 1, 1);
    e = ":apid :oad:unne:";
    if (std::strcmp(r, e) != 0) {
        std::fprintf(stderr, "Test case 5 failed:\nExpected: %s\nActual:   %s\n", e, r);
        std::free(r);
        return EXIT_FAILURE;
    }
    std::free(r);

    // Test case 6
    r = regex_simplereplace("Rapid roadrunner", "r", ":", 0, 1);
    e = "Rapid :oad:unne:";
    if (std::strcmp(r, e) != 0) {
        std::fprintf(stderr, "Test case 6 failed:\nExpected: %s\nActual:   %s\n", e, r);
        std::free(r);
        return EXIT_FAILURE;
    }
    std::free(r);

    // Test case 7
    r = regex_simplereplace("metete tete que te metas tete", "((te)+ )+", "T! ", 0, 1);
    e = "meT! que T! metas tete";
    if (std::strcmp(r, e) != 0) {
        std::fprintf(stderr, "Test case 7 failed:\nExpected: %s\nActual:   %s\n", e, r);
        std::free(r);
        return EXIT_FAILURE;
    }
    std::free(r);

    // Test case 8
    r = regex_simplereplace("abc<span tag='x'>def</span>ghi", "<span[^>]+>([^<]+)</span>", "%1", 1, 1);
    e = "abcdefghi";
    if (std::strcmp(r, e) != 0) {
        std::fprintf(stderr, "Test case 8 failed:\nExpected: %s\nActual:   %s\n", e, r);
        std::free(r);
        return EXIT_FAILURE;
    }
    std::free(r);

    // Test case 9
    r = regex_simplereplace("<div>para1</div>para2", "<div>([^<]+)</div>", "%1\n", 1, 1);
    e = "para1\npara2";
    if (std::strcmp(r, e) != 0) {
        std::fprintf(stderr, "Test case 9 failed:\nExpected: %s\nActual:   %s\n", e, r);
        std::free(r);
        return EXIT_FAILURE;
    }
    std::free(r);

    // Test case 10
    r = regex_simplereplace("<div>Botella</div><div>Cuartito</div>Caguama", "<div>([^<]+)</div>", "%1\n", 1, 1);
    e = "Botella\nCuartito\nCaguama";
    if (std::strcmp(r, e) != 0) {
        std::fprintf(stderr, "Test case 10 failed:\nExpected: %s\nActual:   %s\n", e, r);
        std::free(r);
        return EXIT_FAILURE;
    }
    std::free(r);

    return EXIT_SUCCESS;
}

int main() {
    if (test() != EXIT_SUCCESS) {
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
