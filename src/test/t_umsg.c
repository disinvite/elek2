#include "test/tester.h"
#include "ui/msgbar.h"

#include <mem.h>
#include <string.h>

int simple(void) {
    MsgBar_Set("test message");
    if (strcmp(MsgBar_Get(), "test message"))
        return 1;

    return 0;
}

int expire(void) {
    MsgBar_Set("another test");
    while (!MsgBar_Tick());
    if (strlen(MsgBar_Get()) > 0)
        return 1;

    return 0;
}

test_t tests[] = {
    {"simple test", &simple},
    {"tick expire", &expire},
    {0, 0} // EOL
};

void main(void) {
    testmain(tests);
}
