#include "functions.h"

void execute_warp(char **args, char *prev_dir, char *current_dir, char *home_dir, int token_count)
{
    char new_dir[PATH_MAX];
    strcpy(new_dir, "");

    if (token_count == 1)
    {
        strcpy(new_dir, home_dir);
        goto skip;
    }
    for (int i = 1; i < token_count; i++)
    {
        char *expanded_path = strdup(args[i]);
        // char *expanded_path = expand_tilde(args[1]);

        if (expanded_path == NULL)
        {
            return;
        }

        if (strcmp(expanded_path, "-") == 0)
        {
            strcpy(new_dir, prev_dir);
        }
        if (expanded_path[0] == '~' )
        {
            // char* home_dir = getenv("HOME");
            strcpy(new_dir, home_dir);
            printf("%s\n", new_dir);
            // expanded_path += 2;
        }
        if (expanded_path[0] == '/')
        {
            strcpy(new_dir, expanded_path);
        }
        if (!(strcmp(expanded_path, "-") == 0) && !(expanded_path[0] == '~' ) && !(expanded_path[0] == '/'))
        {
            snprintf(new_dir, PATH_MAX, "%s/%s/%s", new_dir, current_dir, expanded_path);
        }
        // printf("%s\n",new_dir);
        getcwd(current_dir, sizeof(current_dir));
        strcpy(prev_dir, current_dir);
        // printf("%s\n",current_dir);
        // printf("%s\n",prev_dir);

        chdir(new_dir);
        getcwd(new_dir, sizeof(new_dir));
        strcpy(current_dir, new_dir);
        printf("%s\n", new_dir);
    }
skip:;
    if (token_count == 1)
    {
        chdir(new_dir);
        getcwd(new_dir, sizeof(new_dir));
        strcpy(current_dir, new_dir);
        printf("%s\n", new_dir);
    }
}