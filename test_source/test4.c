#include <stdlib.h>

int data[] = { 10, 20, 30, 40 };
int *getdata() {
     return data;
}

int **getdata2() {
    int **r;

    r = malloc(sizeof(int *) * 4);
    for (int i = 0; i < 4; i++) {
        r[i] = data + (3-i);
    }

    return r;
}


