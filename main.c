#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

#define BUFSIZE 1024
#define EXIT_FAILIURE -2

char *read_line(void);
char **split_line(char *);
int launch(char **args);

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
        printf("ECHOING: %s\n", line);
        args = split_line(line);
        status = launch(args);
        
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
            bufsize += BUFSIZE;
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
                exit(EXIT_FAILIURE);
            }
        }
        
        token = strtok(NULL, TOK_DELIM); // By having NULL as first arg, we continue from the end of last token
    }
    tokens[position] = NULL; // Sentinel termination value of the pointer array
    return tokens;
}

int launch(char **args)
{
    pid_t pid, wpid;
    int status;
    
    pid = fork();
    if (pid == 0) {
        if (execvp(args[0], args) == -1) {
            perror("osh");
        }
        exit(EXIT_FAILIURE);
    } else if (pid < 0) {
        // Error forking
        perror("osh");
    } else {
        do {
            wpid = waitpid(pid, &status, WUNTRACED);
        } while(!WIFEXITED(status) && !WIFSIGNALED(status));
    }
    
    return 1;
}