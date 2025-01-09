#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

#define BUFSIZE 1024
#define EXIT_FAILIURE -2

char *read_line(void);
char **split_line(char *);
int launch(char **);
int loop(void);
int osh_execute(char **);

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
        status = osh_execute(args);
        
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

/*
  Builtins
*/

int osh_cd(char **args);
int osh_help(char **args);
int osh_exit(char **args);

char *builtin_str[] = {
    "cd",
    "help",
    "exit",
};

int (*builtin_func[]) (char **) = {
    &osh_cd,
    &osh_help,
    &osh_exit,
};

int osh_num_builtins() {
    return sizeof(builtin_str) / sizeof(char *);
}

int osh_cd(char **args)
{
    if (args[1] == NULL) {
        fprintf(stderr, "osh: expected argument to \"cd\"\n");
    } else {
        if (chdir(args[1]) != 0) {
            perror("osh");
        }
    }
    return 1;
}

int osh_help(char **args)
{
    int i;
    printf("A simple shell called OSH(it!)\n");
    printf("The folowing builtin commands are availible\n");
    for (i = 0; i < osh_num_builtins(); i++) {
        printf("  %s\n", builtin_str[i]);
    }
    
    printf("Use the \"man\" command for information on non-builtin commands.\n");
}

int osh_exit(char **args)
{
    return 0;
}

int osh_execute(char **args)
{
    int i;

    if (args[0] == NULL) {
        return 1;
    }
    
    for (i = 0; i < osh_num_builtins(); i++) {
        if (strcmp(args[0], builtin_str[i]) == 0) {
            return (*builtin_func[i])(args);
        }
    }
    
    return launch(args);
}