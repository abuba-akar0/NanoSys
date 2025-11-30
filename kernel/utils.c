#include "utils.h"

// String comparison
int strcmp(const char* s1, const char* s2) {
    while (*s1 && (*s1 == *s2)) {
        s1++;
        s2++;
    }
    return *(const unsigned char*)s1 - *(const unsigned char*)s2;
}

// Check if character is a digit
int isdigit(char c) {
    return c >= '0' && c <= '9';
}

// String to integer conversion
int atoi(const char* str) {
    int res = 0;
    int sign = 1;
    int i = 0;

    if (str[0] == '-') {
        sign = -1;
        i++;
    }

    for (; str[i] != '\0'; ++i) {
        if (isdigit(str[i])) {
            res = res * 10 + (str[i] - '0');
        } else {
            break; 
        }
    }
    return sign * res;
}
