#include "functions.h"

void execute_command(char **args, int background)
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
        // Parent process
        if (!background)
        {
            struct timeval start, end;
            gettimeofday(&start, NULL);

            int status;
            waitpid(pid, &status, 0);

            gettimeofday(&end, NULL);
            double elapsed = (end.tv_sec - start.tv_sec) +
                             (end.tv_usec - start.tv_usec) / 1000000.0;

            printf("%s : %.0fs\n", args[0], elapsed);
        }
        else
        {
            printf("[%d]\n", pid);
        }
    }
}