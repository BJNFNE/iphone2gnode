#include <cstdlib> // for malloc, free
#include <cassert>
#include <cstring> // for strlen, strcmp
#include <cstdio> // for printf

#include "filter.hpp"

char * quote(char const * s) {
    int i = 0;
    int o = 0;
    char * r = static_cast<char *>(std::malloc(std::strlen(s) * 2 + 1));
    while (s[i] != '\0') {
        switch(s[i]) {
        case '\n':
            r[i+o] = '\\';
            r[i+o+1] = 'n';
            ++o;
            break;
        default:
            r[i+o] = s[i];
        }
        ++i;
    }
    r[i+o] = '\0';
    return r;
}

int main() {
    int test = 0;
    const int test_n = 7;

    char const * input[test_n];
    char const * expected[test_n];

    input[0] = "";
    expected[0] = "";

    input[1] = "a";
    expected[1] = "a";

    input[2] = "<div>para1</div>";
    expected[2] = "para1\n";

    input[3] = "<div>para1</div><div>para2</div>";
    expected[3] = "para1\npara2\n";

    input[4] = "<div>para1 <span attr='val'>text</span></div><div>para2</div>";
    expected[4] = "para1 text\npara2\n";

    input[5] = "<div>para1 <span attr='val'>text</span>more<br>text</div><div>para2</div>";
    expected[5] = "para1 textmoretext\npara2\n";

    input[6] = "<div>para1 <span attr='val'>text</span>mo&lt;re<br>text</div><div>para2</div>";
    expected[6] = "para1 textmo<retext\npara2\n";

    for (test = 0; test < test_n; ++test) {
        char *result = nullptr;

        char *qi;
        char *qe;
        char *qr;

        /* Test information */
        qi = quote(input[test]);
        qe = quote(expected[test]);
        std::printf("  [%4d] Test %3u: decode_alloc(\"%s\") should return \"%s\"\n", __LINE__, test, qi, qe);

        /* Test execution */
        result = decode_alloc(input[test]);

        /* Test assertions */
        qr = quote(result);
        assert(result != nullptr);
        if (std::strcmp(result, expected[test]) != 0) {
            std::printf("        FAILED: Instead, got \"%s\"\n", qr);

            std::free(result);
            std::free(qi);
            std::free(qe);
            std::free(qr);
            return EXIT_FAILURE;
        };

        std::free(result);
        std::free(qi);
        std::free(qe);
        std::free(qr);

    }

    return EXIT_SUCCESS;
}
