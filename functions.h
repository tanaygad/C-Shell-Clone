#include <dirent.h>
#include <stdbool.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <errno.h>
#include <pwd.h>
#include <grp.h>
#include <time.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <sys/time.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h> // Add this line to include the necessary header
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <dirent.h> // Add this line to include the necessary header for directory operations
#include <stdbool.h>
#include <sys/stat.h>
#include <errno.h>
#include <pwd.h>
#include <grp.h>
#include <time.h>
#include <limits.h>

// #define PATH_MAX 1024
#define MAX_INPUT_LENGTH 1024
#define MAX_ARGS 1024
#define PATH_MAX 4096

#define COLOR_RESET "\033[0m"
#define COLOR_RED "\033[31m"
#define COLOR_GREEN "\033[32m"
#define COLOR_BLUE "\033[34m"
#define COLOR_WHITE "\033[37m"

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

void peek_directory(char *path, bool show_hidden, bool show_details)
{
    struct dirent **entries;
    int num_entries;

    num_entries = scandir(path, &entries, NULL,alphasort);
    if (num_entries < 0) {
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

void execute_warp(char **args, char * prev_dir, char *current_dir, char *home_dir, int token_count)
{
    char new_dir[PATH_MAX];

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
        else if (expanded_path[0] == '~' || token_count == 1)
        {
            // char* home_dir = getenv("HOME");
            strcpy(new_dir, home_dir);
        }
        else if (expanded_path[0] == '/')
        {
            strcpy(new_dir, expanded_path);
        }
        else
        {
            snprintf(new_dir, PATH_MAX, "%s/%s", current_dir, expanded_path);
        }
        free(expanded_path);
        strcpy(prev_dir,current_dir);
        chdir(new_dir);
        getcwd(new_dir, sizeof(new_dir));
        strcpy(current_dir,new_dir);
        printf("%s\n", new_dir);
    }
skip:;
    if (token_count == 1)
    {
        chdir(new_dir);
        getcwd(new_dir, sizeof(new_dir));
        strcpy(current_dir,new_dir);
        printf("%s\n", new_dir);
    }
}

char* get_process_status(char* status) {
    if (strcmp(status, "R") == 0) {
        return "Running";
    } else if (strcmp(status, "S") == 0) {
        return "Sleeping in an interruptible wait";
    } else if (strcmp(status, "Z") == 0) {
        return "Zombie";
    } else {
        return "Unknown";
    }
}

void proclore_process(pid_t pid) {
    char proc_path[PATH_MAX];
    snprintf(proc_path, PATH_MAX, "/proc/%d", pid);

    struct stat file_stat;
    if (stat(proc_path, &file_stat) != 0) {
        perror("Error getting process information");
        return;
    }

    char status_path[PATH_MAX];
    snprintf(status_path, PATH_MAX, "/proc/%d/status", pid);

    FILE *status_file = fopen(status_path, "r");
    if (status_file == NULL) {
        perror("Error opening status file");
        return;
    }

    char line[256];
    char status[3];
    char vm_size[64];
    char executable_path[PATH_MAX];
    char procPath[50] = "";
    sprintf(procPath, "/proc/%d/status", pid);
    // printf("%s\n",executable_path);
    while (fgets(line, sizeof(line), status_file)) {
        if (strncmp(line, "State:", 6) == 0) {
            sscanf(line, "%*s %s", status);
        } else if (strncmp(line, "VmSize:", 7) == 0) {
            sscanf(line, "%*s %s", vm_size);
        } 
        else if (strncmp(line, "Name:", 5) == 0) {
            sscanf(line, "%*s %s", executable_path);
        }
    }

    fclose(status_file);

    // Get process group
    pid_t process_group = getpgid(pid);

    // Determine if the process is in the foreground or background
    bool is_foreground = (tcgetpgrp(STDIN_FILENO) == process_group);
    sprintf(procPath, "/proc/%d/exe", pid);
    readlink(procPath, executable_path, PATH_MAX);

    printf("pid : %d\n", pid);
    printf("process status : %s%s\n", status, is_foreground ? "+" : "");
    printf("Process Group : %d\n", process_group);
    printf("Virtual memory : %s\n", vm_size);
    printf("executable path : %s\n", executable_path);
}

void print_colored(const char *text, const char *color) {
    printf("%s%s" COLOR_RESET, color, text);
}

void seek_directory(const char *target_name, const char *target_directory, int directories_only, int files_only, int execute_flag) {
    DIR *dir = opendir(target_directory);
    if (dir == NULL) {
        perror("Error opening directory");
        return;
    }

    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL) {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
            continue;  // Skip current directory and parent directory entries
        }

        char full_path[PATH_MAX];
        snprintf(full_path, PATH_MAX, "%s/%s", target_directory, entry->d_name);

        struct stat file_stat;
        if (stat(full_path, &file_stat) != 0) {
            perror("Error getting file status");
            continue;
        }

        int is_directory = S_ISDIR(file_stat.st_mode);
        int is_file = S_ISREG(file_stat.st_mode);

        // Check if the entry matches the target name
        if (strcmp(entry->d_name, target_name) == 0) {
            if ((is_directory && !files_only) || (is_file && !directories_only)) {
                if (execute_flag) {
                    if (is_directory) {
                        if (chdir(full_path) != 0) {
                            perror("Error changing directory");
                        }
                    } else if (is_file) {
                        FILE *file = fopen(full_path, "r");
                        if (file == NULL) {
                            perror("Error opening file");
                        } else {
                            char line[MAX_INPUT_LENGTH];
                            while (fgets(line, sizeof(line), file)) {
                                printf("%s", line);
                            }
                            fclose(file);
                        }
                    }
                } else {
                    printf("%s/%s\n", target_directory, entry->d_name);
                }
            }
        }

        if (is_directory) {
            seek_directory(target_name, full_path, directories_only, files_only, execute_flag);
        }
    }

    closedir(dir);
}