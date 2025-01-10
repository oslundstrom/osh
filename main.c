#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

#include "osh.h"
#define BUFSIZE 1024

#ifndef TESTING
int main(int argc, char **argv)
{
    loop();
    
    return 0;
}
#endif

void free_arr(char **arr)
{
    if (!arr)
        return;
        
    for (int i = 0; arr[i] != NULL; i++)
        free(arr[i]);
    free(arr);
}

int loop(void)
{
    char *line;
    char **commands;
    char **args;
    int status = 1;
    int i;

    do {
        printf("$ ");
        
        // Get input
        line = read_line();
        if (!line)
            return EXIT_SUCCESS;
        
        // Split into separate commands (e.g., by semicolons)
        commands = split_to_cmds(line);
        if (!commands) {
            free(line);
            return EXIT_FAILURE;
        }

        // Execute each command in the line
        for (i = 0; commands[i] != NULL && status; i++) {
            args = split_to_args(commands[i]);
            if (!args)
                continue;
                
            status = osh_execute(args); // TODO: handle intermediate status
            free(args);
        }
        
        // Cleanup
        free(line);
        free(commands);

    } while (status);

    return EXIT_SUCCESS;
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
        exit(EXIT_FAILURE);
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
char **split_tokens(char *line, char *delim)
{
    int bufsize = TOK_BUFSIZE, position = 0;
    char **tokens = malloc(bufsize * sizeof(char*));
    char *token;
    
    if (!tokens) {
        fprintf(stderr, "osh: allocation error\n");
        exit(EXIT_FAILURE);
    }
    
    token = strtok(line, delim);
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
        
        token = strtok(NULL, delim); // By having NULL as first arg, we continue from the end of last token
    }
    tokens[position] = NULL; // Sentinel termination value of the pointer array
    return tokens;
}

#define ARGS_DELIM " \t\r\n\a"
char **split_to_args(char *line)
{
    return split_tokens(line, ARGS_DELIM);
}


#define CMD_BUFSIZE 64
#define CMD_DELIM "&&,||,|,;"
char **split_to_cmds(char *line)
{
    int bufsize = CMD_BUFSIZE, position = 0;
    char **cmds = malloc(bufsize * sizeof(char *));
    char *token;
    
    if (!cmds) {
        fprintf(stderr, "osh: allocation error\n");
    }
    
    char *delims = CMD_DELIM;
    token = strsplit(line, delims);
    while (token != NULL) {
        cmds[position] = token;
        position++;
        
        if (position >= bufsize) {
            bufsize += TOK_BUFSIZE;
            cmds = realloc(cmds, bufsize * sizeof(char*));
            if (!cmds) {
                fprintf(stderr, "osh: allocation error\n");
                exit(EXIT_FAILURE);
            }
        }
        
        token = strsplit(NULL, delims); // By having NULL as first arg, we continue from the end of last token
    }
    cmds[position] = NULL; // Sentinel termination value of the pointer array
    return cmds;
}

char * strsplit(char *s, const char *delim)
{
    static char *olds;
    return strsplit_r(s, delim, &olds);
}

/* Split string into tokens that are seperated by the full 
   delim string */
char * strsplit_r(char *s, const char *delims, char **save_ptr)
{
    if (s == NULL)
        s = *save_ptr;

    if (*s == '\0') {
        *save_ptr = s;
        return NULL;
    }

    char *my_delims = malloc(strlen(delims) + 1);
    if (!my_delims) {
        return NULL;
    }
    
    strcpy(my_delims, delims);
    char **tokens;
    tokens = split_tokens(my_delims, ",");

    char *loc = NULL;
    char *newloc;
    char *delim;
    int i = 0;
    while (tokens[i]) {
        newloc = strstr(s, tokens[i]);
        if (newloc == NULL) {
            i++;
            continue;
        }
        if (loc == NULL) {
            loc = newloc;
            delim = tokens[i];
        } else if (newloc < loc) {
            loc = newloc;
            delim = tokens[i];
        }
        i++;
    }

    if (!(loc == NULL))
    {
        // Terminate the token, and push forward the save_ptr past the delim
        *loc = '\0';
        *save_ptr = loc + strlen(delim);
        return s;
    }

    // Delimiter was not found, the whole string is returned
    *save_ptr = s + strlen(s);
    
    free(tokens);
    free(my_delims);
    
    return s;
}

int launch(char **args)
{
    pid_t pid, wpid;
    int status;
    
    pid = fork();
    if (pid == 0) {
        // Child process
        if (execvp(args[0], args) == -1) {
            // Exit with errno value
            exit(errno);
        }
        exit(EXIT_SUCCESS);
    } else if (pid < 0) {
        perror("osh");
    } else {
        do {
            wpid = waitpid(pid, &status, WUNTRACED);
            if (wpid == -1) {
                perror("waitpid");
                return -1;
            }
        } while (!WIFEXITED(status) && !WIFSIGNALED(status));
        
        if (WIFEXITED(status)) {
            // Get child's exit code (which we set to errno)
            int child_errno = WEXITSTATUS(status);
            fprintf(stderr, "Child errno: %d (%s)\n", child_errno, strerror(child_errno));
        }
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
    printf("A simple shell called O(h)Sh(it!)\n");
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