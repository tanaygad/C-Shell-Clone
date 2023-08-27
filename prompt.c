#include "functions.h"

void prompt(char *initial)
{
    char cwd[4096];
    getcwd(cwd, sizeof(cwd));
    char *username = getenv("USER");
    char systemname[4096];
    gethostname(systemname, sizeof(systemname));

    if (strcmp(cwd, initial) == 0)
        printf("\033[1;35m<%s@%s:~>\033[0m", username, systemname);
    else
        printf("\033[1;35m<%s@%s:%s>\033[0m", username, systemname, cwd);
}