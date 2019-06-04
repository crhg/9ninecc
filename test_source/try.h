#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void try_ret(int expected, int actual, char *name);
void try_out_start(void);
void try_out_check(char *expected, char *name);
void try_printf(char *fmt, ...);
