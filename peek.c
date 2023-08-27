#include "functions.h"

void peek_directory(char *path, bool show_hidden, bool show_details)
{
    struct dirent **entries;
    int num_entries;

    num_entries = scandir(path, &entries, NULL, alphasort);
    if (num_entries < 0)
    {
        perror("Error scanning directory");
        return;
    }
    for (int i = 0; i < num_entries; i++)
    {
        struct dirent *entry = entries[i];
        if (!show_hidden && entry->d_name[0] == '.')
        {
            continue; // Skip hidden files and directories
        }

        char full_path[PATH_MAX];
        snprintf(full_path, PATH_MAX, "%s/%s", path, entry->d_name);

        struct stat file_stat;
        if (stat(full_path, &file_stat) != 0)
        {
            perror("Error getting file status");
            continue;
        }

        // Print details if requested
        if (show_details)
        {
            mode_t mode = file_stat.st_mode;
            printf((S_ISDIR(mode)) ? "d" : "-");
            printf((mode & S_IRUSR) ? "r" : "-");
            printf((mode & S_IWUSR) ? "w" : "-");
            printf((mode & S_IXUSR) ? "x" : "-");
            printf((mode & S_IRGRP) ? "r" : "-");
            printf((mode & S_IWGRP) ? "w" : "-");
            printf((mode & S_IXGRP) ? "x" : "-");
            printf((mode & S_IROTH) ? "r" : "-");
            printf((mode & S_IWOTH) ? "w" : "-");
            printf((mode & S_IXOTH) ? "x" : "-");
            struct passwd *pw = getpwuid(file_stat.st_uid);
            struct group *gr = getgrgid(file_stat.st_gid);
            char time_buf[80];
            strftime(time_buf, sizeof(time_buf), "%b %d %H:%M", localtime(&file_stat.st_mtime));

            printf("%s %2ld %-8s %-8s %8ld %s ",
                   (S_ISDIR(file_stat.st_mode)) ? "d" : "-",
                   (long)file_stat.st_nlink,
                   pw->pw_name,
                   gr->gr_name,
                   (long)file_stat.st_size,
                   time_buf);
            if (S_ISDIR(file_stat.st_mode))
            {
                printf(COLOR_BLUE "%s\n" COLOR_RESET, entry->d_name);
            }
            else if (file_stat.st_mode & S_IXUSR)
            {
                printf(COLOR_GREEN "%s\n" COLOR_RESET, entry->d_name);
            }
            else
            {
                printf(COLOR_WHITE "%s\n" COLOR_RESET, entry->d_name);
            }
        }
        else
        {
            // Apply color codes
            if (S_ISDIR(file_stat.st_mode))
            {
                printf(COLOR_BLUE "%s\n" COLOR_RESET, entry->d_name);
            }
            else if (file_stat.st_mode & S_IXUSR)
            {
                printf(COLOR_GREEN "%s\n" COLOR_RESET, entry->d_name);
            }
            else
            {
                printf(COLOR_WHITE "%s\n" COLOR_RESET, entry->d_name);
            }
        }
    }
}