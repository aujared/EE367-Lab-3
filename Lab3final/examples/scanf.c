#include <stdio.h>

int main(void) {
    char str[100];
    int i =0;

    int c;
    while ((c = getchar()) != 10) {
        str[i] = c;
        i++;

    }
    printf("got %s \n" , str);
}