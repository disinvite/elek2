#ifndef TESTER_H
#define TESTER_H

typedef struct test_s {
    char *desc;
    int (*func)(void);
} test_t;

void testmain(test_t *test);

#endif