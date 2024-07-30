#include "functions.h"

void execute_command(char **args, int background, char **activities_saved, int *activities_saved_count, int *bgprocessids, char **bgcommands, int *bgcount)
{

    pid_t pid = fork();

    if (pid < 0)
    {
        perror("Fork failed");
    }
    else if (pid == 0)
    {
        // Child process
        if (execvp(args[0], args) == -1)
        {
            perror("Error executing command");
            exit(EXIT_FAILURE);
        }
    }
    else
    {
        running_foreground_process = pid;
        waitpid(pid, NULL, WUNTRACED);
        running_foreground_process = 0;

        char file_name[4096];
        sprintf(file_name, "/proc/%d/stat", pid);
        int fd = open(file_name, O_RDONLY);
        char buff[1024];
        int b = read(fd, buff, 1024);
        buff[b] = '\0';
        char state;
        int count = 0;
        char *token = strtok(buff, " ");
        // printf("hello\n");
        while (token != NULL)
        {
            count++;
            if (count == 3)
            {
                state = token[0];
                break;
            }
            token = strtok(NULL, " ");
        }
        // printf("%c\n", state);
        if (state == 'T')
        {
            bgprocessids[*bgcount] = pid;
            bgcommands[(*bgcount)++] = strdup(args[0]);
            sprintf(activities_saved[*activities_saved_count], "%d ", pid);
            strcat(activities_saved[*activities_saved_count], ": ");
                for (int i = 0; i < 100; i++)
                {
                    //printf("%s\n", tokens[i]);
                    if (args[i]==NULL) break;
                    strcat(activities_saved[*activities_saved_count], args[i]);
                    strcat(activities_saved[*activities_saved_count], " ");
                }
            (*activities_saved_count)++;
        }
        // Parent process
        if (!background)
        {
            struct timeval start, end;
            gettimeofday(&start, NULL);

            // int status;
            // waitpid(pid, &status, WUNTRACED);

            gettimeofday(&end, NULL);
            double elapsed = (end.tv_sec - start.tv_sec) +
                             (end.tv_usec - start.tv_usec) / 1000000.0;

            // printf("%s : %.0fs\n", args[0], elapsed);
        }
        else
        {
            printf("[%d]\n", pid);
        }
    }
}