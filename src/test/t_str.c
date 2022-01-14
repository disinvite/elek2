#include "test/tester.h"
#include "common/string.h"

#include <mem.h>
#include <string.h>

// what's correct here? (0,0) or (1,1)?
int empty_string(void) {
    int x, y;
    string_getdim("", &x, &y);
    if (x || y)
        return 1;

    return 0;
}

int basic_string_xdim(void) {
    int x, y;
    string_getdim("test", &x, &y);
    if (x != 4)
        return 1;

    return 0;
}

int multiline_string_xdim(void) {
    int x, y;
    string_getdim("test\nsecond line", &x, &y);
    if (x != 11)
        return 1;

    return 0;
}

int string_n_lines(void) {
    int x, y;
    string_getdim("one line", &x, &y);
    if (y != 1)
        return 1;

    string_getdim("two\nlines", &x, &y);
    if (y != 2)
        return 1;

    return 0;
}

int multiple_newlines(void) {
    int x, y;
    string_getdim("two\n\nspaces", &x, &y);
    if (y != 3)
        return 1;

    return 0;
}

test_t tests[] = {
    {"correct vals for empty string", &empty_string},
    {"basic string xdim", &basic_string_xdim},
    {"multiline string xdim", &multiline_string_xdim},
    {"string n lines", &string_n_lines},
    {"not confused by consecutive newlines", &multiple_newlines},
    {0, 0} // EOL
};

void main(void) {
    testmain(tests);
}
