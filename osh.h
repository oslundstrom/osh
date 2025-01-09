#ifndef OSH_H
char *read_line(void);
char **split_to_args(char *);
char **split_to_cmds(char *);
char *strsplit(char *, const char *);
char *strsplit_r(char *, const char *, char **);
int launch(char **);
int loop(void);
int osh_execute(char **);
#define OSH_H
#endif // OSH_H
