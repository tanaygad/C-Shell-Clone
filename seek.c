#include "functions.h"

void print_colored(const char *text, const char *color)
{
    printf("%s%s" COLOR_RESET, color, text);
}

void seek_directory(const char *target_name, const char *target_directory, int directories_only, int files_only, int execute_flag, int *found, int *founddir, char *save_file)
{

    FILE *sfile = fopen(save_file, "w");
    DIR *dir = opendir(target_directory);
    if (dir == NULL)
    {
        perror("Error opening directory");
        return;
    }

    struct dirent *entry;

    while ((entry = readdir(dir)) != NULL)
    {

        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
        {
            continue; // Skip current directory and parent directory entries
        }

        char full_path[PATH_MAX];
        snprintf(full_path, PATH_MAX, "%s/%s", target_directory, entry->d_name);

        struct stat file_stat;
        if (stat(full_path, &file_stat) != 0)
        {
            perror("Error getting file status");
            continue;
        }

        int is_directory = S_ISDIR(file_stat.st_mode);
        int is_file = S_ISREG(file_stat.st_mode);

        // Check if the entry matches the target name
        if (strncmp(entry->d_name, target_name, strlen(target_name)) == 0)
        {
            if ((is_directory && !files_only) || (is_file && !directories_only))
            {
                (*found)++;
                if (execute_flag)
                {
                    if (is_directory)
                    {
                        (*founddir) = 1;
                    }
                    else if (is_file)
                    {
                        fprintf(sfile, "%s\n", full_path);
                    }
                }
                else
                {
                    fprintf(sfile, "%s/%s\n", target_directory, entry->d_name);
                }
            }
        }

        if (is_directory)
        {
            seek_directory(target_name, full_path, directories_only, files_only, execute_flag, found, founddir, save_file);
        }
    }
    fclose(sfile);
    closedir(dir);
}