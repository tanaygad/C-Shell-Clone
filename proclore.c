#include "functions.h"

void proclore_process(pid_t pid)
{
    char proc_path[PATH_MAX];
    snprintf(proc_path, PATH_MAX, "/proc/%d", pid);

    struct stat file_stat;
    if (stat(proc_path, &file_stat) != 0)
    {
        perror("Error getting process information");
        return;
    }

    char status_path[PATH_MAX];
    snprintf(status_path, PATH_MAX, "/proc/%d/status", pid);

    FILE *status_file = fopen(status_path, "r");
    if (status_file == NULL)
    {
        perror("Error opening status file");
        return;
    }

    char line[256];
    char status[3];
    char vm_size[64];
    char executable_path[PATH_MAX];
    // printf("%s\n",executable_path);
    while (fgets(line, sizeof(line), status_file))
    {
        if (strncmp(line, "State:", 6) == 0)
        {
            sscanf(line, "%*s %s", status);
        }
        else if (strncmp(line, "VmSize:", 7) == 0)
        {
            sscanf(line, "%*s %s", vm_size);
        }
        else if (strncmp(line, "Name:", 5) == 0)
        {
            sscanf(line, "%*s %s", executable_path);
        }
    }
    pid_t process_group = getpgid(pid);
    bool is_foreground = (tcgetpgrp(STDIN_FILENO) == process_group);
    //---------------------------------------------------------------
    char procPath[50] = "";
    sprintf(procPath, "/proc/%d/stat", pid);
    FILE *procFile = fopen(procPath, "r");
    char buffer[PATH_MAX];
    (fgets(buffer, PATH_MAX, procFile));
    int wordCount = 0;
    int tpgidd;
    char *token = strtok(buffer, " ");
    while (token != NULL)
    {
        if (wordCount == 7)
        {
            tpgidd = atoi(token);
        }
        wordCount++;
        token = strtok(NULL, " ");
    }
    if (tpgidd == pid)
    {
        is_foreground = true;
    }
    //-----------------------------------------------------------------
    fclose(status_file);

    // Get process group

    // Determine if the process is in the foreground or background
    sprintf(procPath, "/proc/%d/exe", pid);
    readlink(procPath, executable_path, PATH_MAX);

    printf("pid : %d\n", pid);
    printf("process status : %s%s\n", status, is_foreground ? "+" : "");
    printf("Process Group : %d\n", process_group);
    printf("Virtual memory : %s\n", vm_size);
    printf("executable path : %s\n", executable_path);
}