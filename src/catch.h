#ifndef HEADER_FILE
#define HEADER_FILE

void stampa_cmd(char *str_cmd, struct command *buf, int index);
void esegui(struct command *buf, int oF, int eF);
void pipeHandler(struct command *buf, int index, int oF, int eF);

#endif
