#include <stdio.h>
#include <sys/types.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

#define MAX_INPUT_SIZE 1024
#define MAX_TOKEN_SIZE 64
#define MAX_NUM_TOKENS 64

char **tokenize(char *line)
{
    char **tokens = (char **)malloc(MAX_NUM_TOKENS * sizeof(char *));
    char *token = (char *)malloc(MAX_TOKEN_SIZE * sizeof(char));
    int i, tokenIndex = 0, tokenNo = 0;

    for (i = 0; i < strlen(line); i++)
    {
        char readChar = line[i];

        if (readChar == ' ' || readChar == '\n' || readChar == '\t')
        {
            token[tokenIndex] = '\0';
            if (tokenIndex != 0)
            {
                tokens[tokenNo] = (char *)malloc(MAX_TOKEN_SIZE * sizeof(char));
                strcpy(tokens[tokenNo++], token);
                tokenIndex = 0;
            }
        }
        else
        {
            token[tokenIndex++] = readChar;
        }
    }

    free(token);
    tokens[tokenNo] = NULL;
    return tokens;
}

int main(int argc, char *argv[])
{
    char line[MAX_INPUT_SIZE];
    char **tokens;
    int i;

    FILE *fp;
    if (argc == 2)
    {
        fp = fopen(argv[1], "r");
        if (fp == NULL)
        {
            printf("File doesn't exist.\n");
            return -1;
        }
    }

    while (1)
    {
        bzero(line, sizeof(line));
        if (argc == 2)
        {
            if (fgets(line, sizeof(line), fp) == NULL)
            {
                break;
            }
            line[strlen(line) - 1] = '\0';
        }
        else
        {
            printf("$ ");
            scanf("%[^\n]", line);
            getchar();
        }

        line[strlen(line)] = '\n';
        tokens = tokenize(line);

        if (tokens[0] == NULL){
            continue;
        }

        int background = 0;
        if (tokens[strlen(tokens[0]) - 1] == '&'){
            background = 1;
            tokens[strlen(tokens[0]) - 1] = '\0';
        }

        int parallel = 0;
        if (strcmp(tokens[0], "&&&") == 0){
            parallel = 1;
        }

        if (strcmp(tokens[0], "cd") == 0){
            if (tokens[1] == NULL){
                chdir(getenv("HOME"));
            }
            else if (chdir(tokens[1]) != 0){
                perror("Error changing directory");
            }

            for (i = 0; tokens[i] != NULL; i++){
                free(tokens[i]);
            }
            free(tokens);
            continue;
        }

        pid_t pid = fork();

        if (pid < 0){
            perror("Fork failed");
            exit(EXIT_FAILURE);
        }
        else if (pid == 0){
            if (execvp(tokens[0], tokens) == -1){
                perror("Error executing command");
                exit(EXIT_FAILURE);
            }
        }
        else{
            if (!background){
                int status;
                waitpid(pid, &status, 0);

                if (WIFEXITED(status)){
                    printf("Child process %d terminated with status: %d\n", pid, WEXITSTATUS(status));
                }
                else if (WIFSIGNALED(status)){
                    printf("Child process %d terminated by signal: %d\n", pid, WTERMSIG(status));
                }
                else{
                    printf("Child process %d terminated abnormally\n", pid);
                }
            }
            else{
                printf("Started background process %d\n", pid);
            }

            if (parallel || background)
            {
                while (waitpid(-1, NULL, WNOHANG) > 0)
                {
                    printf("Shell: Background process finished\n");
                }
            }
        }

        for (i = 0; tokens[i] != NULL; i++)
        {
            free(tokens[i]);
        }
        free(tokens);
    }

    if (argc == 2){
        fclose(fp);
    }

    return 0;
}
