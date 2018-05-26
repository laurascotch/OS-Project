#ifndef HEADER_FILE
#define HEADER_FILE

void stampa_file(int outputFile, char *str_cmd, int dbytes, int nbytes, char datemes[1024], char message[1024], int resultcode, int code);
void stampa_cmd(char *str_cmd, struct command *buf, int index);
void esegui(struct command *buf, int oF, int eF, int code);
void pipeHandler(struct command *buf, int index, int oF, int eF, int code);

#endif
