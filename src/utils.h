/**
 * Clue Splash - Graphical term for representation of boot process
 *
 * File: utils.c
 * Headers for graphical routines and all related utilities
 */

#define LOGGER(...) logger(__FILE__, __LINE__, __VA_ARGS__)

struct cp_vt {
    int prev_tty_ndx;
    int prev_kdmode;
    int tty;
    int tty_ndx;
};

typedef struct cp_vt cp_vt;

bool startsWith (char* base, char* str);
bool endsWith (char* base, char* str);
int indexOf_shift (char* base, char* str, int startIndex);
int indexOf (char* base, char* str);
int lastIndexOf (char* base, char* str);
char* strtrim(char *str);
int min(int a, int b);
int max(int a, int b);
