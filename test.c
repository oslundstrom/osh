#include <stdio.h>
#include <string.h>

int main() {
    char str[] = "hej hej hoj hoj";
    char *t;
    t = strtok(str, " ");
    printf("%s\n", t);
    t = strtok(NULL, " ");
    printf("%s\n", t);
}