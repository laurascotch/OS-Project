#ifndef HEADER_FILE
#define HEADER_FILE

#include <stdio.h>

typedef struct command{
    char **args;
};

int aiuto();
int funz_cd(struct command *buf);
void argCheck(int argc, char **argv, char *outputPath, char *errPath, int *code, int *maxlen);
void fileOpen(char *outputPath, char *errPath, int *oF, int *eF);
struct command *readline(int *index);
int min(int a, int b);

#endif
