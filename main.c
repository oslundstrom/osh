#include <stdio.h>
#include <string.h>

#define BUFSIZE 1024
#define EXIT_FAILIURE -2

int main(int argc, char **argv)
{
    loop();
    
    return 0;
}

int loop(void)
{
    char *line;
    char **args;
    int status;

    do {
        printf("# ");
        line = read_line();
        args = split_line(line);
        status = execute(args);
        
        free(line);
        free(args);
    } while (status);
}

char *read_line(void)
{
    int bufsize = BUFSIZE;
    int position = 0;
    char *buffer = malloc(sizeof(char) * bufsize);
    int c;

    if (!buffer) 
    {
        fprintf(stderr, "osh: allocation error\n");
        exit(EXIT_FAILIURE);
    }

    while(1) 
    {
        c = getchar();

        if (c == EOF || c == '\n') {
            buffer[position] = '\0';
            return buffer;
        } else {
            buffer[position] = c;
        }
        position++;

        if (position >= bufsize) {
            bufsize += BUF;
            buffer = realloc(buffer, bufsize);
            if (!buffer) {
                fprintf(stderr, "osh: allocation error\n");
            }
        }
    }
}

#define TOK_BUFSIZE 64
#define TOK_DELIM " \t\r\n\a"
char **split_line(char *line)
{
    int bufsize = TOK_BUFSIZE, position = 0;
    char **tokens = malloc(bufsize * sizeof(char*));
    char *token;
    
    if (!tokens) {
        fprintf(stderr, "osh: allocation error\n");
        exit(EXIT_FAILIURE);
    }
    
    token = strtok(line, TOK_DELIM);
    while (token != NULL) {
        tokens[position] = token;
        position++;
        
        if (position >= bufsize) {
            bufsize += TOK_BUFSIZE;
            tokens = realloc(tokens, bufsize * sizeof(char*));
            if (!tokens) {
                fprintf(stderr, "osh: allocation error\n");
                exit(EXIT_FAILURE);
            }
        }
        
        token = strtok(NULL, TOK_DELIM);
    }
    
}